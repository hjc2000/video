#pragma once
#include<A3Device.h>
#include<A3Excepion.h>
#include<jccpp/Json.h>
#include<MemoryStream.h>
#include<R2TuneTable.h>
#include<TsSource.h>
#include<chip_define.h>
#include<chrono>
#include<functional>
#include<halreg_calibration.h>
#include<halreg_playload.h>
#include<halreg_stream.h>
#include<halservice_broadcast.h>
#include<halservice_transform.h>
#include<iostream>
#include<memory>
#include<mutex>
#include<output_modulator.h>
#include<sstream>
#include<thread>
#include<tool_helpful.h>
#include<vatek_base.h>
#include<vector>

using namespace A3;
using std::stringstream;
using std::string;
using std::mutex;
using std::lock_guard;
using std::shared_ptr;
using std::unique_ptr;
using std::function;
using std::vector;
using std::thread;
using std::cout;
using std::endl;

#define USB_DEVICE_MAX_PACKET_LEN 			512
#define TRCAPTURE_MAX_PACKETNUMS			10
#define TRCAPTURE_DEFAILT_TIMEOUT			10000
#define TRSLICE_PACKET_NUMS					(CHIP_STREAM_SLICE_LEN/TS_PACKET_LEN)
#define US_DEF_PREPARE_TIME				10
#define US_CIRCLEBUF_SIZE				(2*1024*1024)
#define US_CIRCLEBUF_NUMS				(US_CIRCLEBUF_SIZE / CHIP_STREAM_SLICE_LEN)

struct calibration_dac :public IJsonSerializable
{
	int8_t _ioffset{};
	int8_t _qoffset{};
	int8_t _igain{};
	int8_t _qgain{};

	Json ToJson() override
	{
		Json json = {
			{"_ioffset",_ioffset},
			{"_qoffset",_qoffset},
			{"_igain",_igain},
			{"_qgain",_qgain},
		};

		return json;
	}
};

class calibration_param :public IJsonSerializable
{
public:
	int32_t _clock{};
	calibration_dac _dac{};
	r2_tune _r2_tune{};

	Json ToJson() override
	{
		return Json{
			{"_clock",_clock},
			{"_dac",_dac.ToJson()},
			{"_r2_tune",_r2_tune.ToJson()},
		};
	}
};

enum usbdevice_type
{
	usb_type_unknown = -1,
	usb_type_broadcast = 0,
	usb_type_rescure = 1,
	usb_type_all = 2,
};

struct usbdevice_id
{
	usbdevice_type type;
	uint16_t vid;
	uint16_t pid;
};

/* put data to usb_cache then used write_ram, write_rom and run code	*/
/* param_0 : address position											*/
#define VATCMD_CLASSV2_PUTDATA					0x42

/* put 4K code then write to mcu_ram									*/
/* param_0 : section number												*/

#define VATCMD_CLASSV2_WRITE_RAM				0x43
/* put 4K code then write to mcu_rom									*/
/* param_0 : section number												*/
#define VATCMD_CLASSV2_WRITE_ROM				0x44

/* put 8K _mcu vector code then run start run code						*/
#define VATCMD_CLASSV2_RUNCODE					0x45		

#define USBDEV_BULK_WRITE_EP					0x01
#define USBDEV_BULK_READ_EP						0x81

#define USBBULK_TAG_LEN							8

/// <summary>
///     PCR 调整模式
/// </summary>
enum pcr_adjust_mode
{
	pcr_disable,
	pcr_adjust,
	pcr_retag,
};

struct filter_rule
{
	uint16_t orange_pid{};
	uint16_t replace_pid{};
};

struct pid_filter
{
	uint32_t filter_nums{};
	filter_rule filters[MUX_FILTER_NAX_NUMS]{};
};

/**
 * @brief 选择 A3 重新封装 ts 的模式。
*/
enum class RemuxMode
{
	/**
	 * @brief 使用此模式得按播放速度向 A3 发送包。
	*/
	stream_bypass,

	/**
	 * @brief 使用此模式得按播放速度向 A3 发送包。
	*/
	stream_smooth,

	/**
	 * @brief ISDBT 如果带宽设置为 6M 以上，不要使用重封装，否则会卡。只能使用 stream_bypass 或 stream_passthrogh。
	*/
	stream_remux,

	/**
	 * @brief 使用此模式得按播放速度向 A3 发送包。
	*/
	stream_passthrogh,
};

struct handle_channel;

class A3Device
{
private:
	#pragma region StreamSourceTypeEnum
	enum StreamSourceTypeEnum
	{
		stream_source_unknown = -1,
		stream_source_null = 0x00000000,
		stream_source_usb = 0x00000001, /*!< usb (transform)    */
		stream_source_tsin = 0x00000002, /*!< tsin (transform)   */
		stream_source_sine = 0x00001000,
		stream_source_test = 0x00001001,
	};

	StreamSourceTypeEnum StreamSourceType()
	{
		uint32_t val = ReadMemory(HALREG_TRANSFORM_INPUT);
		return (StreamSourceTypeEnum)val;
	}
	void SetStreamSourceType(StreamSourceTypeEnum value)
	{
		WriteMemory(HALREG_TRANSFORM_INPUT, value);
	}
	#pragma endregion

	#pragma region TransformModeEnum
	/**
	 * @ingroup transform_api
	 * @brief transform operation mode
	 */
	enum TransformModeEnum
	{
		trmode_stopped = 0,
		trmode_enum = 0x00000003,				/*!< enumerate ts stream context */
		trmode_broadcast = 0x00000005,		/*!< start broadcast (transform source) */
	};

	/// <summary>
	///		从 A3 读取当前的传输模式。
	/// </summary>
	/// <returns></returns>
	TransformModeEnum TransformMode()
	{
		uint32_t val = ReadMemory(0x628);
		return (TransformModeEnum)val;
	}
	/// <summary>
	///		设置 A3 当前的传输模式
	/// </summary>
	/// <param name="value"></param>
	void SetTransformMode(TransformModeEnum value)
	{
		WriteMemory(0x601, value);
	}
	#pragma endregion

	void StartRfMixer()
	{
		vk_result nres = vatek_success;
		nres = SendCmdAndGetResult(RFMIXER_CMD_START, HALREG_TRANSFORM_CNTL);
		if (nres < 0)
		{
			throw jc::Exception();
		}
	}

	r2_param GetR2Param()
	{
		r2_param r2_param;
		uint32_t val = ReadMemory(HALREG_R2_EN_CNTL);
		if (val == R2_EN_PATH_0)
		{
			r2_param._r2_control_mode = r2_cntl_path_0;
		}
		else if (val == R2_EN_PATH_1)
		{
			r2_param._r2_control_mode = r2_cntl_path_1;
		}
		else if (val == R2_EN_PARAM)
		{
			r2_param._r2_control_mode = r2_cntl_param;
		}
		else
		{
			throw A3::A3DataException();
		}

		r2_param.r2_flags = ReadMemory(0x1209);
		r2_param._tune._ioffset = ReadMemory(0x120A);
		r2_param._tune._qoffset = ReadMemory(0x120B);
		r2_param._tune._imgoffset = ReadMemory(0x120C);
		r2_param._tune._phaseoffset = ReadMemory(0x120D);
		r2_param._gpiocntl = ReadMemory(0x120E);
		r2_param._pagain = ReadMemory(0x120F);

		return r2_param;
	}
	void SetR2Param(r2_param value)
	{
		if (value._r2_control_mode == r2_cntl_path_0)
		{
			WriteMemory(HALREG_R2_EN_CNTL, R2_EN_PATH_0);
		}
		else if (value._r2_control_mode == r2_cntl_path_1)
		{
			WriteMemory(HALREG_R2_EN_CNTL, R2_EN_PATH_1);
		}
		else if (value._r2_control_mode == r2_cntl_param)
		{
			WriteMemory(HALREG_R2_EN_CNTL, R2_EN_PARAM);
		}

		if (value._r2_control_mode == r2_cntl_param)
		{
			WriteMemory(0x1209, value.r2_flags);
			WriteMemory(0x120A, value._tune._ioffset);
			WriteMemory(0x120B, value._tune._qoffset);
			WriteMemory(0x120C, value._tune._imgoffset);
			WriteMemory(0x120D, value._tune._phaseoffset);
			WriteMemory(0x120E, value._gpiocntl);
			WriteMemory(0x120F, value._pagain);
		}
	}

	uint32_t Flags()
	{
		return ReadMemory(0x641);
	}
	void SetFlags(uint32_t value)
	{
		WriteMemory(0x641, value);
	}

public:
	/// <summary>
	///		向 A3 发送重启命令
	/// </summary>
	/// <returns></returns>
	void Reboot()
	{
		WriteMemory(HALREG_SERVICE_BASE_CNTL, BASE_CMD_REBOOT);
		WriteMemory(HALREG_RESCUE_CNTL, RESCUE_CNTL_REBOOT);
	}

	#pragma region CalibrationParam
	/**
	 * @brief 检查 Calibration 是否被使能。
	 * @return
	*/
	bool CalibrationEnabled()
	{
		chip_info pinfo = GetChipInfo();
		if (pinfo._status != chip_status_running)
		{
			throw A3::ChipStatusException();
		}

		return pinfo._peripheral_en & PERIPHERAL_CALIBRATION;
	}

	/**
	 * @brief 获取 A3 内存中的 calibration_param。（不是非易失储存器中的 calibration_param）。
	 * @return
	*/
	calibration_param CalibrationParam()
	{
		calibration_param calibration;
		calibration._clock = (int32_t)ReadMemory(HALREG_CALIBRATION_CLOCK);

		uint32_t val = ReadMemory(HALREG_CALIBRATION_DAC);
		calibration._dac._ioffset = (uint8_t)(val >> 24);
		calibration._dac._qoffset = (uint8_t)(val >> 16);
		calibration._dac._igain = (uint8_t)(val >> 8);
		calibration._dac._qgain = (uint8_t)(val);

		calibration._r2_tune._ioffset = ReadMemory(0x1280);
		calibration._r2_tune._qoffset = ReadMemory(0x1281);
		calibration._r2_tune._imgoffset = ReadMemory(0x1282);
		calibration._r2_tune._phaseoffset = ReadMemory(0x1283);
		return calibration;
	}
	/**
	 * @brief 设置 calibration_param。立刻生效，但不会保存，重启后会恢复。要保存需要调用
	 * SaveCalibrationParam 方法。
	 *
	 * @param value
	*/
	void SetCalibrationParam(calibration_param const &value)
	{
		WriteMemory(HALREG_CALIBRATION_CNTL, CALIBRATION_EN_TAG | 1);

		WriteMemory(HALREG_CALIBRATION_CLOCK, value._clock);

		uint32_t val = ((((uint8_t)value._dac._ioffset) << 24) |
			(((uint8_t)value._dac._qoffset) << 16) |
			(((uint8_t)value._dac._igain) << 8) |
			((uint8_t)value._dac._qgain));
		WriteMemory(HALREG_CALIBRATION_DAC, val);

		WriteMemory(0x1280, value._r2_tune._ioffset);
		WriteMemory(0x1281, value._r2_tune._qoffset);
		WriteMemory(0x1282, value._r2_tune._imgoffset);
		WriteMemory(0x1283, value._r2_tune._phaseoffset);
	}

	/**
	 * @brief 保存当前 A3 内存里的 calibration_param 到 A3 的非易失储存器。
	*/
	void SaveCalibrationParam()
	{
		WriteMemory(HALREG_CALIBRATION_CNTL, CALIBRATION_EN_TAG | 0);
		SendCmdAndGetResult(BASE_CMD_CALIBRATION_SAVE, HALREG_SERVICE_BASE_CNTL);
		WriteMemory(HALREG_CALIBRATION_CNTL, CALIBRATION_EN_TAG | 1);
	}
	#pragma endregion

	RemuxMode StreamMode()
	{
		return (RemuxMode)ReadMemory(0x640);
	}
	void SetStreamMode(RemuxMode value)
	{
		WriteMemory(0x640, (uint32_t)value);
	}

	rfmixer_status rfmixer_get_status()
	{
		uint32_t val = ReadMemory(HALREG_RF_COM_STATUS);
		rfmixer_status status = rfmixer_fail;

		if (val == RF_STATUS_ACTIVE)
			status = rfmixer_powered;
		else if (val == RF_STATUS_IDLE)
			status = rfmixer_idle;

		return status;
	}

	/**
	 * @brief A3 开始进行 ts 输出。
	 * 在这之前需要初始化完所有参数。除了 CalibrationParam。CalibrationParam 要在开始输出后才能修改。
	 *
	 * @param modulator_param
	*/
	void StartTransformStreamOutput(ModulatorParamCollection modulator_param)
	{
		if (GetChipInfo()._hal_service != service_transform)
		{
			throw jc::NotSupportedException();
		}

		SetModulatorParam(modulator_param);
		StartRfMixer();

		r2_param r2param{};
		r2param._r2_control_mode = r2_cntl_mode::r2_cntl_path_0;
		SetR2Param(r2param);

		SetStreamSourceType(stream_source_usb);
		SetFlags(USB_EN_ASYNCBUFFER);
		SetTransformMode(TransformModeEnum::trmode_broadcast);

		/* _isdb-t must disable usb_dma because chip memory bandwidth limited.
			DVB-T2 only remux set output_nodma*/
		if (modulator_param._type == modulator_isdb_t ||
			modulator_param._type == modulator_dtmb ||
			(StreamMode() == RemuxMode::stream_remux && modulator_param._type == modulator_dvb_t2))
		{
			EnableDMA(0);
		}
		else
		{
			EnableDMA(1);
		}

		vk_result nres = vatek_success;
		nres = SendCmdAndGetResult(BC_START, HALREG_TRANSFORM_CNTL);
		if (nres < 0)
		{
			throw jc::Exception();
		}

		if (GetChipInfo()._status != chip_status_running)
		{
			throw A3::ChipStatusException();
		}
	}

	vk_result rfmixer_stop(uint32_t cntl)
	{
		return SendCmdAndGetResult(RFMIXER_CMD_STOP, cntl);
	}

	int8_t R2Power()
	{
		return (int8_t)ReadMemory(HALREG_EXT_R2_GAIN);
	}
	void SetR2Power(int8_t value)
	{
		uint32_t nvalue = EXT_R2_GAIN_EN_TAG | (EXT_R2_GAIN_MASK & (uint32_t)value);
		WriteMemory(HALREG_EXT_R2_GAIN, nvalue);
	}

public:
	vk_result vatek_transform_stop()
	{
		if (TransformMode() == trmode_stopped)
		{
			return vatek_badstatus;
		}

		vk_result nres = vatek_success;
		nres = SendCmdAndGetResult(BC_STOP, HALREG_TRANSFORM_CNTL);
		if (is_vatek_success(nres))
		{
			if (TransformMode() == trmode_broadcast)
			{
				nres = rfmixer_stop(HALREG_TRANSFORM_CNTL);
			}
		}

		SetTransformMode(trmode_stopped);
		return nres;
	}

public:
	hal_service_mode ServiceMode()
	{
		return (hal_service_mode)ReadMemory(HALREG_SERVICE_MODE);
	}

	pid_filter _filters{};

private:
	vk_result chip_raise_command(uint32_t cmdaddr, uint32_t cmd)
	{
		uint32_t val = ReadMemory(cmdaddr);
		if (val != 0)
		{
			cout << "chip_raise_command 从 cmdaddr 读出的值不为 0。" << endl;
			return vatek_badstatus;
		}

		WriteMemory(cmdaddr, cmd);

		int64_t tick = DateTime::GetSteadyTimeInMilliseconds();

		while (1)
		{
			if ((DateTime::GetSteadyTimeInMilliseconds() - tick) >= CHIP_RAISE_COMMAND_TIMEOUT)
			{
				return vatek_timeout;
			}

			std::this_thread::sleep_for(milliseconds(500));

			val = ReadMemory(cmdaddr);
			if (val == 0)
			{
				return vatek_success;
			}
		}
	}

public:
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	/// <exception cref="A3DataException"></exception>
	chip_info GetChipInfo()
	{
		chip_info info;
		info._status = ChipStatus();
		info._errcode = GetErrorCode();

		switch (ReadMemory(HALREG_CHIP_ID))
		{
		case HAL_CHIPID_A1:
			{
				info._chip_module = ic_module_a1;
				break;
			}
		case HAL_CHIPID_A3:
			{
				info._chip_module = ic_module_a3;
				break;
			}
		case HAL_CHIPID_B1:
			{
				info._chip_module = ic_module_b1;
				break;
			}
		case HAL_CHIPID_B2:
			{
				info._chip_module = ic_module_b2;
				break;
			}
		case HAL_CHIPID_B3:
			{
				info._chip_module = ic_module_b3_lite;
				break;
			}
		case HAL_CHIPID_B3_PLUS:
			{
				info._chip_module = ic_module_b3_plus;
				break;
			}
		case HAL_CHIPID_B2_PLUS:
			{
				info._chip_module = ic_module_b2_plus;
				break;
			}
		case HAL_CHIPID_E1:
			{
				info._chip_module = ic_module_e1;
				break;
			}
		default:
			{
				throw A3::A3DataException();
			}
		}

		switch (ReadMemory(HALREG_SERVICE_MODE))
		{
		case SERVICE_TAG_RESCUE:
			{
				info._hal_service = service_rescue;
				break;
			}
		case SERVICE_TAG_BROADCAST:
			{
				info._hal_service = service_broadcast;
				break;
			}
		case SERVICE_TAG_TRANSFORM:
			{
				info._hal_service = service_transform;
				break;
			}
		default:
			{
				throw A3::A3DataException();
			}
		}

		info._version = ReadMemory(HALREG_FW_VER);
		info._peripheral_en = ReadMemory(HALREG_PERIPHERAL_EN);
		info._input_support = ReadMemory(HALREG_INPUT_SUPPORT);
		info._output_support = ReadMemory(HALREG_OUTPUT_SUPPORT);

		return info;
	}

	/// <summary>
	///		获取设备名称。
	///		会判断设备是通过 bridge 还是 USB 连接的，从而调用不同的 API 获取名称。
	///		如果是未知的连接类型，则返回 "_unknown" 字符串。
	/// </summary>
	/// <returns>设备名称</returns>
	virtual const char *DeviceName() = 0;

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	/// <exception cref="A3DataException"></exception>
	chip_status ChipStatus()
	{
		uint32_t val = 0;
		val = ReadMemory(HALREG_SYS_STATUS_0);
		chip_status chipstatus = chip_status_fail_hw;

		int32_t i = 0;
		val &= SYS_STATUS_TAG | 0xFF;

		switch (val)
		{
		case SYS_STATUS_BADSTATUS:
			{
				chipstatus = chip_status_badstatus;
				break;
			}
		case SYS_STATUS_UNKNOWN_FAIL:
			{
				chipstatus = chip_status_fail_hw;
				break;
			}
		case SYS_STATUS_SERVICE_FAIL:
			{
				chipstatus = chip_status_fail_service;
				break;
			}
		case SYS_STATUS_LOADER_FAIL:
			{
				chipstatus = chip_status_fail_loader;
				break;
			}
		case SYS_STATUS_TAG:
			{
				chipstatus = chip_status_unknown;
				break;
			}
		case SYS_STATUS_IDLE:
			{
				chipstatus = chip_status_waitcmd;
				break;
			}
		case SYS_STATUS_RUN:
			{
				chipstatus = chip_status_running;
				break;
			}
		default:
			{
				throw A3::A3DataException();
			}
		}

		return chipstatus;
	}

	/// <summary>
	///		发送命令并且获得命令执行结果。
	/// </summary>
	/// <param name="cmd"></param>
	/// <param name="cmdaddr"></param>
	/// <param name="resultaddr"></param>
	/// <returns></returns>
	vk_result SendCmdAndGetResult(uint32_t cmd, uint32_t cmdaddr)
	{
		vk_result nres = chip_raise_command(cmdaddr, cmd);
		if (nres < 0)
		{
			return nres;
		}

		uint32_t err = ReadMemory(HALREG_SYS_ERRCODE);
		if (err & HALREG_ERR_TAG)
			nres = vatek_hwfail;

		return nres;
	}

	/// <summary>
	///		从 A3 读取错误信息。
	/// </summary>
	/// <returns></returns>
	uint32_t GetErrorCode()
	{
		return ReadMemory(0x23);
	}

	/// <summary>
	///		从设备中读取调制器参数。
	/// </summary>
	/// <param name="param"></param>
	/// <returns></returns>
	ModulatorParamCollection ModulatorParam()
	{
		ModulatorParamCollection param;
		param._type = (modulator_type)ReadMemory(0x680);
		param._bandwidth_symbolrate = ReadMemory(0x684);
		param._ifmode = (dac_ifmode)ReadMemory(0x681);
		param.iffreq_offset = ReadMemory(0x682);
		param.freqkhz = ReadMemory(HALREG_RF_COM_FREQ);

		vk_result nres = vk_result::vatek_success;
		switch (param._type)
		{
		case modulator_type::modulator_dvb_t:
			{
				param._dvb_t_param.constellation = (dvbt_constellation_mode)ReadMemory(0x690);
				param._dvb_t_param.fft = (fft_mode)ReadMemory(0x691);
				param._dvb_t_param.guardinterval = (guard_interval)ReadMemory(0x692);
				param._dvb_t_param.coderate = (code_rate)ReadMemory(0x693);
				break;
			}
		case modulator_type::modulator_j83a:
			{
				param._j83a_param.constellation = (j83a_constellation_mode)ReadMemory(0x690);
				break;
			}
		case modulator_type::modulator_atsc:
			{
				param._atsc_param.constellation = (atsc_constellation_mode)ReadMemory(0x690);
				break;
			}
		case modulator_type::modulator_j83b:
			{
				param._j83b_param.constellation = (j83b_constellation_mode)ReadMemory(0x690);
				break;
			}
		case modulator_type::modulator_dtmb:
			{
				param._dtmb_param.constellation = (dtmb_constellation_mode)ReadMemory(0x690);
				param._dtmb_param.timeinterleaved = (dtmb_time_interleaved)ReadMemory(0x691);
				param._dtmb_param.coderate = (dtmb_code_rate)ReadMemory(0x692);
				param._dtmb_param.carriermode = (dtmb_carrier_mode)ReadMemory(0x693);
				param._dtmb_param.framesync = (dtmb_frame_header)ReadMemory(0x694);
				break;
			}
		case modulator_type::modulator_isdb_t:
			{
				param._isdb_t_param.constellation = (isdbt_constellation_mode)ReadMemory(0x690);
				param._isdb_t_param.fft = (fft_mode)ReadMemory(0x691);
				param._isdb_t_param.guardinterval = (guard_interval)ReadMemory(0x692);
				param._isdb_t_param.coderate = (code_rate)ReadMemory(0x693);
				param._isdb_t_param.timeinterleaved = (isdb_t_time_interleaved)ReadMemory(0x694);
				param._isdb_t_param.isdb_t_flags = ReadMemory(0x695);
				break;
			}
		case modulator_type::modulator_j83c:
			{
				param._j83c_param.constellation = (j83c_constellation_mode)ReadMemory(0x690);
				break;
			}
		case modulator_type::modulator_dvb_t2:
			{
				param._dvb_t2_param.version = (t2_version)ReadMemory(0x690);
				param._dvb_t2_param.t2_flags = ReadMemory(0x691);
				param._dvb_t2_param.issy = (t2_issy)ReadMemory(0x692);
				param._dvb_t2_param.ti_ni = (uint16_t)ReadMemory(0x693);
				param._dvb_t2_param.l1_constellation = (constellation_t2_l1)ReadMemory(0x694);
				param._dvb_t2_param.plp_constellation = (constellation_t2_plp)ReadMemory(0x695);
				param._dvb_t2_param.fft = (t2_fft_mode)ReadMemory(0x696);
				param._dvb_t2_param.coderate = (t2_code_rate)ReadMemory(0x697);
				param._dvb_t2_param.guardinterval = (t2_guard_interval)ReadMemory(0x698);
				param._dvb_t2_param.pilotpattern = (t2_pilot_pattern)ReadMemory(0x699);
				param._dvb_t2_param.fectype = (t2_fec_length)ReadMemory(0x69A);
				param._dvb_t2_param.network_id = (uint16_t)ReadMemory(0x69B);
				param._dvb_t2_param.system_id = (uint16_t)ReadMemory(0x69C);
				param._dvb_t2_param.fecblock_nums = (uint16_t)ReadMemory(0x69D);
				param._dvb_t2_param.symbol_nums = (uint16_t)ReadMemory(0x69E);
				break;
			}
		default:
			{
				throw jc::Exception("不支持的制式");
			}
		}

		return param;
	}
	/**
	 * @brief 将调制器参数设置到设备。
	 * @param value 调制器参数
	*/
	void SetModulatorParam(ModulatorParamCollection &value)
	{
		WriteMemory(0x680, value._type);
		WriteMemory(0x684, value._bandwidth_symbolrate);
		WriteMemory(0x681, value._ifmode);
		WriteMemory(0x682, value.iffreq_offset);
		WriteMemory(HALREG_RF_COM_FREQ, value.freqkhz);

		switch (value._type)
		{
		case modulator_type::modulator_dvb_t:
			{
				WriteMemory(0x690, (uint32_t)value._dvb_t_param.constellation);
				WriteMemory(0x691, value._dvb_t_param.fft);
				WriteMemory(0x692, value._dvb_t_param.guardinterval);
				WriteMemory(0x693, value._dvb_t_param.coderate);
				break;
			}
		case modulator_type::modulator_j83a:
			{
				WriteMemory(0x690, (uint32_t)value._j83a_param.constellation);
				break;
			}
		case modulator_type::modulator_atsc:
			{
				WriteMemory(0x690, (uint32_t)value._atsc_param.constellation);
				break;
			}
		case modulator_type::modulator_j83b:
			{
				WriteMemory(0x690, (uint32_t)value._j83b_param.constellation);
				break;
			}
		case modulator_type::modulator_dtmb:
			{
				WriteMemory(0x690, (uint32_t)value._dtmb_param.constellation);
				WriteMemory(0x691, value._dtmb_param.timeinterleaved);
				WriteMemory(0x692, value._dtmb_param.coderate);
				WriteMemory(0x693, value._dtmb_param.carriermode);
				WriteMemory(0x694, value._dtmb_param.framesync);
				break;
			}
		case modulator_type::modulator_isdb_t:
			{
				WriteMemory(0x690, (uint32_t)value._isdb_t_param.constellation);
				WriteMemory(0x691, value._isdb_t_param.fft);
				WriteMemory(0x692, value._isdb_t_param.guardinterval);
				WriteMemory(0x693, value._isdb_t_param.coderate);
				WriteMemory(0x694, value._isdb_t_param.timeinterleaved);
				WriteMemory(0x695, value._isdb_t_param.isdb_t_flags);
				break;
			}
		case modulator_type::modulator_j83c:
			{
				WriteMemory(0x690, (uint32_t)value._j83c_param.constellation);
				break;
			}
		case modulator_type::modulator_dvb_t2:
			{
				WriteMemory(0x690, value._dvb_t2_param.version);
				WriteMemory(0x691, value._dvb_t2_param.t2_flags);
				WriteMemory(0x692, value._dvb_t2_param.issy);
				WriteMemory(0x693, value._dvb_t2_param.ti_ni);
				WriteMemory(0x694, value._dvb_t2_param.l1_constellation);
				WriteMemory(0x695, value._dvb_t2_param.plp_constellation);
				WriteMemory(0x696, value._dvb_t2_param.fft);
				WriteMemory(0x697, value._dvb_t2_param.coderate);
				WriteMemory(0x698, value._dvb_t2_param.guardinterval);
				WriteMemory(0x699, value._dvb_t2_param.pilotpattern);
				WriteMemory(0x69A, value._dvb_t2_param.fectype);
				WriteMemory(0x69B, value._dvb_t2_param.network_id);
				WriteMemory(0x69C, value._dvb_t2_param.system_id);
				WriteMemory(0x69D, value._dvb_t2_param.fecblock_nums);
				WriteMemory(0x69E, value._dvb_t2_param.symbol_nums);
				break;
			}
		default:
			{
				throw jc::Exception("不支持的制式");
			}
		}
	}

	virtual uint32_t ReadRegister(uint32_t addr) = 0;
	virtual void WriteRegister(int32_t addr, uint32_t val) = 0;

	virtual uint32_t ReadMemory(int32_t addr) = 0;
	virtual void WriteMemory(int32_t addr, uint32_t val) = 0;

	virtual void WriteBlock(uint8_t *pbuf, int32_t len) = 0;
	virtual vk_result ReadBlock(uint8_t *pbuf, int32_t len) = 0;

	/**
	 * @brief A3 缓冲区中剩余的空间可以容纳的包数量。
	 *   注意：本属性在向缓冲区写入包后，需要调用 CommitPackets 方法才能被更新，否则不管向 A3
	 * 缓冲区写入多少，返回的都是一样的值。
	 *
	 * @return
	*/
	uint32_t A3AvailablePacketSpace()
	{
		if (TransformMode() == trmode_stopped)
		{
			throw A3::ChipStatusException();
		}

		return ReadMemory(0x629);
	}

	/**
	 * @brief 向 A3 缓冲区写入一定数量的包后，调用此方法向 A3 提交包。提交后 A3AvailablePacketSpace 属性
	 * 才会被更新。
	 *
	 * RemuxMode 选择了 stream_remux 模式时需要向 A3 发送一定数量的包后调用此函数提交。透传模式不需要。
	*/
	void CommitPackets()
	{
		if (TransformMode() == trmode_stopped)
		{
			throw A3::ChipStatusException();
		}

		WriteMemory(0x629, 0);
	}

	pcr_adjust_mode PcrAdjustMode()
	{
		return (pcr_adjust_mode)ReadMemory(0x642);
	}
	void SetPcrAdjustMode(pcr_adjust_mode value)
	{
		WriteMemory(0x642, value);
	}

	virtual void EnableDMA(bool enable) = 0;

	vk_result StopStream()
	{
		EnableDMA(0);
		return vatek_success;
	}

	uint32_t DataBitrate()
	{
		return ReadMemory(0x626);
	}

	uint32_t CurBitrate()
	{
		/*!< current muxer output bitrate */
		return ReadMemory(0x625);
	}

	uint32_t MuxBitrate()
	{
		/*!< mux output bitrate */
		return ReadMemory(0x623);
	}

	uint32_t QueryBuffer()
	{
		return  ReadMemory(0x627);
	}

	uint32_t ModBitrate()
	{
		/*!< output current data bitrate */
		return  ReadMemory(0x622);
	}

	#pragma region broadcast_status
	/**
	 * @ingroup broadcast_api
	 * broadcast status when running show current status
	 */
	enum broadcast_status
	{
		bcstatus_fail_unknown = BCSTATUS_FAIL_UNKNOWN,		/*!< unknown fail */
		bcstatus_fail_source = BCSTATUS_FAIL_SOURCE,		/*!< stream (input) fail*/
		bcstatus_fail_timeout = BCSTATUS_FAIL_TIMEOUT,		/*!< wait stream (input) timeout */
		bcstatus_fail_drop = BCSTATUS_FAIL_CODECDROP,		/*!< video encoder drop frame */
		bcstatus_fail_buffer = BCSTATUS_FAIL_BUFFER,		/*!< buffer fail */
		bcstatus_fail_muxer = BCSTATUS_FAIL_MUXER,			/*!< muxer fail */
		bcstatus_fail_encode = BCSTATUS_FAIL_ENCODE,		/*!< encoder fail */
		bcstatus_fail_media = BCSTATUS_FAIL_MEDIA,			/*!< media fail */
		bcstatus_fail_demux = BCSTATUS_FAIL_DEMUX,			/*!< _demux fail */
		bcstatus_idle = BCSTATUS_IDLE,						/*!< not running */
		bcstatus_wait_source = BCSTATUS_WAIT_SOURCE,		/*!< wait stream (input) ready */
		bcstatus_broadcast = BCSTATUS_BROADCAST,			/*!< running */
		bcstatus_finish = BCSTATUS_FINISH,					/*!< operation finish */
	};

	broadcast_status BroadCastStatus()
	{
		return (broadcast_status)ReadMemory(0x624);
	}
	#pragma endregion

	/// <summary>
	///		PID 过滤器是否开启。
	/// </summary>
	/// <returns>开启返回 true，关闭返回 false。</returns>
	bool PidFilterEnabled()
	{
		return ReadMemory(HALREG_MUX_FILTER_EN);
	}

	/**
	 * @brief 应用 _filters 字段中的过滤器。
	 * 要求在 StartTransformStreamOutput 之前调用。
	*/
	void UseFilter()
	{
		if (_filters.filter_nums < MUX_FILTER_NAX_NUMS)
		{
			if (_filters.filter_nums == 0)
			{
				WriteMemory(HALREG_MUX_FILTER_EN, 0);
			}
			else
			{
				uint32_t val = MUX_FILTER_EN_TAG | (_filters.filter_nums & MUX_FILTER_MASK);
				WriteMemory(HALREG_MUX_FILTER_EN, val);

				int32_t i = 0;
				for (i = 0; i < (int32_t)_filters.filter_nums; i++)
				{
					if (_filters.filters[i].orange_pid != _filters.filters[i].replace_pid)
					{
						val = FILTER_REPLACE(_filters.filters[i].orange_pid, _filters.filters[i].replace_pid);
					}
					else
					{
						val = FILTER_NO_REPLACE(_filters.filters[i].orange_pid);
					}

					WriteMemory((HALREG_MUX_FILTER_0 + i), val);
				}
			}
		}
	}
};
