#ifndef _OUTPUT_MODULATOR_
#define _OUTPUT_MODULATOR_

#include<R2TuneTable.h>
#include<format>
#include<format>
#include<halreg_define.h>
#include<halreg_output.h>
#include<jccpp/Exception.h>
#include<memory>
#include<tool_helpful.h>
#include<vatek_base.h>

using std::shared_ptr;
using std::static_pointer_cast;

/// <summary>
///     调制器调制方式
/// </summary>
enum modulator_type
{
	modulator_unknown = -1,
	modulator_dvb_t = MOD_DVB_T,		/*!< DVB-T              */
	modulator_j83a = MOD_J83A,			/*!< J83a (DVB-C)       */
	modulator_atsc = MOD_ATSC,			/*!< ATSC (8vsb)	    */
	modulator_j83b = MOD_J83B,			/*!< J83b (ClearQAM)    */
	modulator_dtmb = MOD_DTMB,			/*!< DTMB               */
	modulator_isdb_t = MOD_ISDB_T,		/*!< ISDB-T             */
	modulator_j83c = MOD_J83C,			/*!< J83c (ISDB-C)      */
	modulator_dvb_t2 = MOD_DVB_T2,		/*!< DVB-T2             */
	modulator_mod_nums,
};

inline std::string ToString(modulator_type type)
{
	switch (type)
	{
	case modulator_type::modulator_dvb_t:
		{
			return "dvb_t";
		}
	case modulator_type::modulator_j83a:
		{
			return "j83a";
		}
	case modulator_type::modulator_atsc:
		{
			return "atsc";
		}
	case modulator_type::modulator_j83b:
		{
			return "j83b";
		}
	case modulator_type::modulator_dtmb:
		{
			return "dtmb";
		}
	case modulator_type::modulator_isdb_t:
		{
			return "isdb_t";
		}
	case modulator_type::modulator_j83c:
		{
			return "j83c";
		}
	case modulator_type::modulator_dvb_t2:
		{
			return "dvb_t2";
		}
	case modulator_type::modulator_unknown:
		{
			return "unknown";
		}
	default:
		{
			throw std::invalid_argument{ "非法参数" };
		}
	}
}

/**
 * @ingroup mod_props
 * Modulation Constellation Mode
 */

enum class atsc_constellation_mode
{
	qam_null = -1,
	_8vsb = ATSC_8VSB,					/*!< 8VSB (ATSC)    */
};

inline std::string ToString(atsc_constellation_mode value)
{
	switch (value)
	{
	case atsc_constellation_mode::_8vsb:
		{
			return "8vsb";
		}
	default:
		{
			throw std::invalid_argument{ "非法参数" };
		}
	}
}

enum class j83c_constellation_mode
{
	j83c_qam64 = J83C_QAM64,			/*!< QAM64 (J83c)	*/
	j83c_qam256 = J83C_QAM256,			/*!< QAM256 (J83c)	*/
};

inline std::string ToString(j83c_constellation_mode value)
{
	switch (value)
	{
	case j83c_constellation_mode::j83c_qam64:
		{
			return "j83c_qam64";
		}
	case j83c_constellation_mode::j83c_qam256:
		{
			return "j83c_qam256";
		}
	default:
		{
			return "Unknown j83c constellation mode";
		}
	}
}

enum class j83b_constellation_mode
{
	j83b_qam64 = J83B_QAM64,			/*!< QAM64 (J83b)	*/
	j83b_qam256 = J83B_QAM256,			/*!< QAM256 (J83b)	*/
};

inline std::string ToString(j83b_constellation_mode value)
{
	switch (value)
	{
	case j83b_constellation_mode::j83b_qam64:
		{
			return "j83b_qam64";
		}
	case j83b_constellation_mode::j83b_qam256:
		{
			return "j83b_qam256";
		}
	default:
		{
			return "Unknown j83b constellation mode";
		}
	}
}

enum class dvbt_constellation_mode
{
	dvb_t_qpsk = DVB_T_QPSK,			/*!< QPSK (DVB-T)	*/
	dvb_t_qam16 = DVB_T_QAM16,			/*!< QAM16 (DVB-T)	*/
	dvb_t_qam64 = DVB_T_QAM64,			/*!< QAM64 (DVB-T)	*/
};

inline std::string ToString(dvbt_constellation_mode value)
{
	switch (value)
	{
	case dvbt_constellation_mode::dvb_t_qpsk:
		{
			return "dvb_t_qpsk";
		}
	case dvbt_constellation_mode::dvb_t_qam16:
		{
			return "dvb_t_qam16";
		}
	case dvbt_constellation_mode::dvb_t_qam64:
		{
			return "dvb_t_qam64";
		}
	default:
		{
			return "Unknown DVB-T constellation mode";
		}
	}
}

enum class isdbt_constellation_mode
{
	isdb_t_dqpsk = ISDB_T_DQPSK,
	isdb_t_qpsk = ISDB_T_QPSK,			/*!< QPSK (ISDB-T)	*/
	isdb_t_qam16 = ISDB_T_QAM16,		/*!< QAM16 (ISDB-T)	*/
	isdb_t_qam64 = ISDB_T_QAM64,		/*!< QAM64 (ISDB-T)	*/
};

inline std::string ToString(isdbt_constellation_mode value)
{
	switch (value)
	{
	case isdbt_constellation_mode::isdb_t_dqpsk:
		{
			return "isdb_t_dqpsk";
		}
	case isdbt_constellation_mode::isdb_t_qpsk:
		{
			return "isdb_t_qpsk";
		}
	case isdbt_constellation_mode::isdb_t_qam16:
		{
			return "isdb_t_qam16";
		}
	case isdbt_constellation_mode::isdb_t_qam64:
		{
			return "isdb_t_qam64";
		}
	default:
		{
			return "Unknown ISDB-T constellation mode";
		}
	}
}

enum class j83a_constellation_mode
{
	j83a_qam16 = J83A_QAM16,			/*!< QAM16 (J83a)	*/
	j83a_qam32 = J83A_QAM32,			/*!< QAM32 (J83a)	*/
	j83a_qam64 = J83A_QAM64,			/*!< QAM64 (J83a)	*/
	j83a_qam128 = J83A_QAM128,			/*!< QAM128 (J83a)	*/
	j83a_qam256 = J83A_QAM256,			/*!< QAM256 (J83a)	*/
};

inline std::string ToString(j83a_constellation_mode value)
{
	switch (value)
	{
	case j83a_constellation_mode::j83a_qam16:
		{
			return "j83a_qam16";
		}
	case j83a_constellation_mode::j83a_qam32:
		{
			return "j83a_qam32";
		}
	case j83a_constellation_mode::j83a_qam64:
		{
			return "j83a_qam64";
		}
	case j83a_constellation_mode::j83a_qam128:
		{
			return "j83a_qam128";
		}
	case j83a_constellation_mode::j83a_qam256:
		{
			return "j83a_qam256";
		}
	default:
		{
			throw std::invalid_argument{ "非法参数" };
		}
	}
}

enum class dtmb_constellation_mode
{
	dtmb_qpsk = DTMB_QPSK,				/*!< QPSK (DTMB)    */
	dtmb_qam16 = DTMB_QAM16,			/*!< QAM16 (DTMB)	*/
	dtmb_qam64 = DTMB_QAM64,			/*!< QAM64 (DTMB)	*/
	dtmb_qam4_nr = DTMB_QAM4_NR,
	dtmb_qam32 = DTMB_QAM32,			/*!< QAM32 (DTMB)	*/
};

inline std::string ToString(dtmb_constellation_mode value)
{
	switch (value)
	{
	case dtmb_constellation_mode::dtmb_qpsk:
		{
			return "dtmb_qpsk";
		}
	case dtmb_constellation_mode::dtmb_qam16:
		{
			return "dtmb_qam16";
		}
	case dtmb_constellation_mode::dtmb_qam64:
		{
			return "dtmb_qam64";
		}
	case dtmb_constellation_mode::dtmb_qam4_nr:
		{
			return "dtmb_qam4_nr";
		}
	case dtmb_constellation_mode::dtmb_qam32:
		{
			return "dtmb_qam32";
		}
	default:
		{
			return "Unknown DTMB constellation mode";
		}
	}
}

#define MOD_DVB_T_BIT		(1 << MOD_DVB_T)
#define MOD_J83A_BIT		(1 << MOD_J83A)
#define MOD_ATSC_BIT		(1 << MOD_ATSC)
#define MOD_J83B_BIT		(1 << MOD_J83B)
#define MOD_DTMB_BIT		(1 << MOD_DTMB)
#define MOD_ISDB_T_BIT		(1 << MOD_ISDB_T)
#define MOD_J83C_BIT		(1 << MOD_J83C)
#define MOD_DVB_T2_BIT		(1 << MOD_DVB_T2)
#define MOD_DVB_S_BIT		(1 << MOD_DVB_S)
#define MOD_DVB_S2_BIT		(1 << MOD_DVB_S2)

/// <summary>
///     调制器码率
/// </summary>
enum code_rate
{
	coderate_null = -1,
	coderate_1_2 = DVB_T_CODERATE_1_2,		/*!< 1/2		*/
	coderate_2_3 = DVB_T_CODERATE_2_3,		/*!< 2/3		*/
	coderate_3_4 = DVB_T_CODERATE_3_4,		/*!< 3/4		*/
	coderate_5_6 = DVB_T_CODERATE_5_6,		/*!< 5/6		*/
	coderate_7_8 = DVB_T_CODERATE_7_8,		/*!< 7/8		*/
};

inline std::string ToString(code_rate value)
{
	switch (value)
	{
	case coderate_1_2:
		{
			return "coderate_1_2";
		}
	case coderate_2_3:
		{
			return "coderate_2_3";
		}
	case coderate_3_4:
		{
			return "coderate_3_4";
		}
	case coderate_5_6:
		{
			return "coderate_5_6";
		}
	case coderate_7_8:
		{
			return "coderate_7_8";
		}
	default:
		{
			return "Unknown code rate";
		}
	}
}

/// <summary>
///     调制器保护间隔
/// </summary>
enum guard_interval
{
	guard_interval_null = -1,
	guard_interval_1_32 = DVB_T_GI_1_32,		/*!< 1/32		*/
	guard_interval_1_16 = DVB_T_GI_1_16,		/*!< 1/16		*/
	guard_interval_1_8 = DVB_T_GI_1_8,			/*!< 1/8		*/
	guard_interval_1_4 = DVB_T_GI_1_4,			/*!< 1/4		*/
};

inline std::string ToString(guard_interval value)
{
	switch (value)
	{
	case guard_interval_1_32:
		{
			return "guard_interval_1_32";
		}
	case guard_interval_1_16:
		{
			return "guard_interval_1_16";
		}
	case guard_interval_1_8:
		{
			return "guard_interval_1_8";
		}
	case guard_interval_1_4:
		{
			return "guard_interval_1_4";
		}
	default:
		{
			return "Unknown guard interval";
		}
	}
}

/// <summary>
///     调制器 FFT
/// </summary>
enum fft_mode
{
	fft_null = -1,
	fft_2k = DVB_T_FFT2K,			/*!< 2K(2048)	*/
	fft_8k = DVB_T_FFT8K,			/*!< 8K(8192)	*/
	fft_4k = DVB_T_FFT4K,			/*!< 8K(4096)	*/
};

inline std::string ToString(fft_mode value)
{
	switch (value)
	{
	case fft_2k:
		{
			return "fft_2k";
		}
	case fft_8k:
		{
			return "fft_8k";
		}
	case fft_4k:
		{
			return "fft_4k";
		}
	default:
		{
			return "Unknown FFT mode";
		}
	}
}

/**
 * @ingroup mod_props
 * atsc parameters
 */
struct atsc_param
{
	atsc_constellation_mode constellation{};   /*!< constellation (_8vsb)*/
};

/**
 * @ingroup mod_props
 * J83b Parameters
 */
struct j83b_param
{
	j83b_constellation_mode constellation{};    /*!< constellation qam64,qam256 */
	r2_cntl_mode r2_path{};
};

/**
 * @ingroup mod_props
 * DVB-T parameters
 */

struct dvb_t_param
{
	dvbt_constellation_mode constellation{};
	fft_mode fft{};
	guard_interval guardinterval{};
	code_rate coderate{};
};

/**
 * @ingroup mod_props
 * J83a Parameters
 */
struct j83a_param
{
	j83a_constellation_mode constellation{};   /*!< constellation qam16,qam32,qam64,qam128,qam256 */
};

/**
 * @ingroup mod_props
 * DTMB Interleaving Mode
 */
enum dtmb_time_interleaved
{
	dtmb_interleaved_null = -1,
	dtmb_interleaved_disable = DTMB_TI_DISABLE,
	dtmb_interleaved_240 = DTMB_TI_240,
	dtmb_interleaved_720 = DTMB_TI_720,
};

inline std::string ToString(dtmb_time_interleaved value)
{
	switch (value)
	{
	case dtmb_interleaved_disable:
		{
			return "dtmb_interleaved_disable";
		}
	case dtmb_interleaved_240:
		{
			return "dtmb_interleaved_240";
		}
	case dtmb_interleaved_720:
		{
			return "dtmb_interleaved_720";
		}
	default:
		{
			return "Unknown DTMB time interleaved mode";
		}
	}
}

/**
 * @ingroup mod_props
 * DTMB Carrier Mode
 */
enum dtmb_carrier_mode
{
	dtmb_carrier_null = -1,
	dtmb_carrier_3780 = DTMB_CARRIER_3780,
	dtmb_carrier_1 = DTMB_CARRIER_1,
};

inline std::string ToString(dtmb_carrier_mode value)
{
	switch (value)
	{
	case dtmb_carrier_3780:
		{
			return "dtmb_carrier_3780";
		}
	case dtmb_carrier_1:
		{
			return "dtmb_carrier_1";
		}
	default:
		{
			return "Unknown DTMB carrier mode";
		}
	}
}

/**
 * @ingroup mod_props
 * DTMB Code Rate
 */
enum dtmb_code_rate
{
	dtmb_code_rate_null = -1,
	dtmb_code_rate_0_4 = DTMB_CODERATE_0_4,
	dtmb_code_rate_0_6 = DTMB_CODERATE_0_6,
	dtmb_code_rate_0_8 = DTMB_CODERATE_0_8,
};

inline std::string ToString(dtmb_code_rate value)
{
	switch (value)
	{
	case dtmb_code_rate_0_4:
		{
			return "dtmb_code_rate_0_4";
		}
	case dtmb_code_rate_0_6:
		{
			return "dtmb_code_rate_0_6";
		}
	case dtmb_code_rate_0_8:
		{
			return "dtmb_code_rate_0_8";
		}
	default:
		{
			return "Unknown DTMB code rate";
		}
	}
}

/**
 * @ingroup mod_props
 * DTMB Frame Header
 */
enum dtmb_frame_header
{
	dtmb_framesync_null = -1,
	dtmb_framesync_420 = DTMB_SYNC_420,
	dtmb_framesync_945 = DTMB_SYNC_945,
	dtmb_framesync_595 = DTMB_SYNC_595,
};

inline std::string ToString(dtmb_frame_header value)
{
	switch (value)
	{
	case dtmb_framesync_420:
		{
			return "dtmb_framesync_420";
		}
	case dtmb_framesync_945:
		{
			return "dtmb_framesync_945";
		}
	case dtmb_framesync_595:
		{
			return "dtmb_framesync_595";
		}
	default:
		{
			return "Unknown DTMB frame header";
		}
	}
}

/**
 * @ingroup mod_props
 * dtmb parameters
 */
struct dtmb_param
{
	dtmb_constellation_mode constellation{};       /*!< constellation */
	dtmb_time_interleaved timeinterleaved{};  /*!< timeinterleaved */
	dtmb_code_rate coderate{};                /*!< coderate */
	dtmb_carrier_mode carriermode{};          /*!< carriermode */
	dtmb_frame_header framesync{};            /*!< framesync */
};

/**
 * @ingroup mod_props
 * ISDB-T Time Interleaving Mode
 */
enum isdb_t_time_interleaved
{
	isdb_t_interleaved_null = -1,
	isdb_t_interleaved_disable = ISDB_T_TI_DISABLE,
	isdb_t_interleaved_mode1 = ISDB_T_TI_MODE1,
	isdb_t_interleaved_mode2 = ISDB_T_TI_MODE2,
	isdb_t_interleaved_mode3 = ISDB_T_TI_MODE3,
};

inline std::string ToString(isdb_t_time_interleaved value)
{
	switch (value)
	{
	case isdb_t_interleaved_disable:
		{
			return "isdb_t_interleaved_disable";
		}
	case isdb_t_interleaved_mode1:
		{
			return "isdb_t_interleaved_mode1";
		}
	case isdb_t_interleaved_mode2:
		{
			return "isdb_t_interleaved_mode2";
		}
	case isdb_t_interleaved_mode3:
		{
			return "isdb_t_interleaved_mode3";
		}
	default:
		{
			return "Unknown ISDB-T time interleaved mode";
		}
	}
}

/**
 * @ingroup mod_props
 * ISDB-T Parameters
 */
struct isdb_t_param
{
	isdbt_constellation_mode constellation{};           /*!< constellation qpsk, qam16, qam64 */
	fft_mode fft{};                               /*!< fft mode */
	guard_interval guardinterval{};               /*!< guard interval */
	code_rate coderate{};                         /*!< code rate */
	isdb_t_time_interleaved timeinterleaved{};    /*!< mode 0,1,2 */
	uint32_t isdb_t_flags{};
};

/**
 * @ingroup mod_props
 * J83c Parameters
 */
struct j83c_param
{
	j83c_constellation_mode constellation{};	/*!< constellation qam64,qam256 */
};

#define T2_AUTO_FRAME_TIME                  90
#define T2_AUTO_NTI                         4

/**
 * @ingroup mod_props
 * DVB-T2 Version
 */
enum t2_version
{
	t2_ver_null = -1,
	t2_ver_111 = T2_VER_1_1_1,                          /*!< version 1.1.1 */
	t2_ver_121 = T2_VER_1_2_1,                          /*!< version 1.2.1 */
	t2_ver_131 = T2_VER_1_3_1,                          /*!< version 1.3.1 */
	t2_ver_131_lite = T2_VER_1_3_1 | T2_PROFILE_LITE,   /*!< version 1.3.1 lite*/
};

inline std::string ToString(t2_version value)
{
	switch (value)
	{
	case t2_ver_111:
		{
			return "1_1_1";
		}
	case t2_ver_121:
		{
			return "1_2_1";
		}
	case t2_ver_131:
		{
			return "1_3_1";
		}
	case t2_ver_131_lite:
		{
			return "1_3_1_lite";
		}
	default:
		{
			throw std::invalid_argument{ "非法参数" };
		}
	}
}


/**
 * @ingroup mod_props
 * DVB-T2 L1 Constellation
 */
enum constellation_t2_l1
{
	t2_l1_null = -1,
	t2_l1_bpsk = T2_L1_BPSK,        /*!< _bpsk */
	t2_l1_qpsk = T2_L1_QPSK,        /*!< _qpsk */
	t2_l1_qam16 = T2_L1_QAM16,      /*!< qam16 */
	t2_l1_qam64 = T2_L1_QAM64,      /*!< qam64 */
};

inline std::string ToString(constellation_t2_l1 value)
{
	switch (value)
	{
	case t2_l1_bpsk:
		{
			return "t2_l1_bpsk";
		}
	case t2_l1_qpsk:
		{
			return "t2_l1_qpsk";
		}
	case t2_l1_qam16:
		{
			return "t2_l1_qam16";
		}
	case t2_l1_qam64:
		{
			return "t2_l1_qam64";
		}
	default:
		{
			return "Unknown T2 L1 constellation mode";
		}
	}
}

/**
 * @ingroup mod_props
 * DVB-T2 PLP Constellation
 */
enum constellation_t2_plp
{
	t2_plp_null = -1,
	t2_plp_qpsk = T2_PLP_QPSK,      /*!< _qpsk */
	t2_plp_qam16 = T2_PLP_QAM16,    /*!< qam16 */
	t2_plp_qam64 = T2_PLP_QAM64,    /*!< qam64 */
	t2_plp_qam256 = T2_PLP_QAM256,  /*!< qam256 */
};

inline std::string ToString(constellation_t2_plp value)
{
	switch (value)
	{
	case t2_plp_qpsk:
		{
			return "t2_plp_qpsk";
		}
	case t2_plp_qam16:
		{
			return "t2_plp_qam16";
		}
	case t2_plp_qam64:
		{
			return "t2_plp_qam64";
		}
	case t2_plp_qam256:
		{
			return "t2_plp_qam256";
		}
	default:
		{
			return "Unknown T2 PLP constellation mode";
		}
	}
}

inline std::string IsdbtFlagsToString(uint32_t flags)
{
	std::string result;

	if (flags & 0x00000001) result += "en_ac_1 ";
	if (flags & 0x00000002) result += "en_ac_2 ";
	if (flags & 0x00000004) result += "en_emergency_flag ";

	if (result.empty())
		return "No flags set";
	else
		return result;
}

/**
 * @ingroup mod_props
 * DVB-T2 Code Rate
 */
enum t2_code_rate
{
	t2_coderate_null = -1,
	t2_coderate_1_2 = T2_CODERATE_1_2,  /*!< 1/2 */
	t2_coderate_3_5 = T2_CODERATE_3_5,  /*!< 3/5 */
	t2_coderate_2_3 = T2_CODERATE_2_3,  /*!< 2/3 */
	t2_coderate_3_4 = T2_CODERATE_3_4,  /*!< 3/4 */
	t2_coderate_4_5 = T2_CODERATE_4_5,  /*!< 4/5 */
	t2_coderate_5_6 = T2_CODERATE_5_6,  /*!< 5/6 */
	t2_coderate_1_3 = T2_CODERATE_1_3,	/*!< 1/3 only _fec-length 16200 */
	t2_coderate_2_5 = T2_CODERATE_2_5,	/*!< 2/5 only _fec-length 16200 */
};

inline std::string ToString(t2_code_rate value)
{
	switch (value)
	{
	case t2_coderate_1_2:
		{
			return "cr_1_2";
		}
	case t2_coderate_3_5:
		{
			return "cr_3_5";
		}
	case t2_coderate_2_3:
		{
			return "cr_2_3";
		}
	case t2_coderate_3_4:
		{
			return "cr_3_4";
		}
	case t2_coderate_4_5:
		{
			return "cr_4_5";
		}
	case t2_coderate_5_6:
		{
			return "cr_5_6";
		}
	case t2_coderate_1_3:
		{
			return "cr_1_3";
		}
	case t2_coderate_2_5:
		{
			return "cr_2_5";
		}
	default:
		{
			return "Unknown code rate";
		}
	}
}


/**
 * @ingroup mod_props
 * DVB-T2 Pilot Pattern
 */
enum t2_pilot_pattern
{
	pilot_pattern_null = -1,
	pilot_pattern_1 = T2_PP_1,      /*!< pp_1 */
	pilot_pattern_2 = T2_PP_2,      /*!< pp_2 */
	pilot_pattern_3 = T2_PP_3,      /*!< pp_3 */
	pilot_pattern_4 = T2_PP_4,      /*!< pp_4 */
	pilot_pattern_5 = T2_PP_5,      /*!< pp_5 */
	pilot_pattern_6 = T2_PP_6,      /*!< pp_6 */
	pilot_pattern_7 = T2_PP_7,      /*!< pp_7 */
	pilot_pattern_8 = T2_PP_8,      /*!< pp_8 */
};

inline std::string ToString(t2_pilot_pattern value)
{
	switch (value)
	{
	case pilot_pattern_1:
		{
			return "pp1";
		}
	case pilot_pattern_2:
		{
			return "pp2";
		}
	case pilot_pattern_3:
		{
			return "pp3";
		}
	case pilot_pattern_4:
		{
			return "pp4";
		}
	case pilot_pattern_5:
		{
			return "pp5";
		}
	case pilot_pattern_6:
		{
			return "pp6";
		}
	case pilot_pattern_7:
		{
			return "pp7";
		}
	case pilot_pattern_8:
		{
			return "pp8";
		}
	default:
		{
			return "Unknown pilot pattern";
		}
	}
}

/**
 * @ingroup mod_props
 * DVB-T2 GUARD INTERVAL
 */
enum t2_guard_interval
{
	t2_gi_1_null = -1,
	t2_gi_1_32 = T2_GI_1_32,        /*!< 1/32 */
	t2_gi_1_16 = T2_GI_1_16,        /*!< 1/16 */
	t2_gi_1_8 = T2_GI_1_8,          /*!< 1/8 */
	t2_gi_1_4 = T2_GI_1_4,          /*!< 1/4 */
	t2_gi_1_128 = T2_GI_1_128,      /*!< 1/128 */
	t2_gi_19_128 = T2_GI_19_128,    /*!< 19/128 */
	t2_gi_19_256 = T2_GI_19_256,    /*!< 19/256 */
};

inline std::string ToString(t2_guard_interval value)
{
	switch (value)
	{
	case t2_gi_1_32:
		{
			return "gi_1_32";
		}
	case t2_gi_1_16:
		{
			return "gi_1_16";
		}
	case t2_gi_1_8:
		{
			return "gi_1_8";
		}
	case t2_gi_1_4:
		{
			return "gi_1_4";
		}
	case t2_gi_1_128:
		{
			return "gi_1_128";
		}
	case t2_gi_19_128:
		{
			return "gi_19_128";
		}
	case t2_gi_19_256:
		{
			return "gi_19_256";
		}
	default:
		{
			throw std::invalid_argument{ "非法参数" };
		}
	}
}

/**
 * @ingroup mod_props
 * DVB-T2 ISSY MODE
 */
enum t2_issy
{
	t2_issy_disable = T2_ISSY_DISABLE,  /*!< disable issy */
	t2_issy_short = T2_ISSY_SHORT,      /*!< short length */
	t2_issy_long = T2_ISSY_LONG,        /*!< long length */
};

inline std::string ToString(t2_issy value)
{
	switch (value)
	{
	case t2_issy_disable:
		{
			return "issy_disable";
		}
	case t2_issy_short:
		{
			return "issy_short";
		}
	case t2_issy_long:
		{
			return "issy_long";
		}
	default:
		{
			return "Unknown ISSY type";
		}
	}
}

inline std::string T2FlagsToString(uint32_t flags)
{
	std::string result;

	if (flags & 0x00000001) result += "en_extend_carrier ";
	if (flags & 0x00000002) result += "en_constellation_rotation ";
	if (flags & 0x00000004) result += "en_ts_hem_mode ";
	if (flags & 0x00000008) result += "en_delete_null_packet ";
	if (flags & 0x00000010) result += "en_vbr_coding ";
	if (flags & 0x00000020) result += "en_time_interval ";

	if (result.empty())
		return "No flags set";
	else
		return result;
}


/**
 * @ingroup mod_props
 * DVB-T2 FEC BLOCK LENGTH
 */
enum t2_fec_length
{
	t2_fec_null = -1,
	t2_fec_16200 = T2_FEC_16200,        /*!< _fec-block length 16200 */
	t2_fec_64800 = T2_FEC_64800,        /*!< _fec-block length 64800 */
};

inline std::string ToString(t2_fec_length value)
{
	switch (value)
	{
	case t2_fec_16200:
		{
			return "16200";
		}
	case t2_fec_64800:
		{
			return "64800";
		}
	default:
		{
			return "Unknown FEC length";
		}
	}
}


/**
 * @ingroup mod_props
 * DVB-T2 FFT
 */
enum t2_fft_mode
{
	t2_fft_null = -1,
	t2_fft_1k = T2_FFT_1K,              /*!< 1k (1024) */
	t2_fft_2k = T2_FFT_2K,              /*!< 2k (2048) */
	t2_fft_4k = T2_FFT_4K,              /*!< 4k (4096) */
	t2_fft_8k = T2_FFT_8K,              /*!< 8k (8192) */
	t2_fft_16k = T2_FFT_16K,            /*!< 16k (16384) */
	t2_fft_32k = T2_FFT_32K,
};

inline std::string ToString(t2_fft_mode value)
{
	switch (value)
	{
	case t2_fft_1k:
		{
			return "1k";
		}
	case t2_fft_2k:
		{
			return "2k";
		}
	case t2_fft_4k:
		{
			return "4k";
		}
	case t2_fft_8k:
		{
			return "8k";
		}
	case t2_fft_16k:
		{
			return "16k";
		}
	case t2_fft_32k:
		{
			return "32k";
		}
	default:
		{
			return "Unknown FFT mode";
		}
	}
}


/**
 * @ingroup mod_props
 * DVB-T2 Parameters
 * Only support SISO one PLP
 */
struct dvb_t2_param
{
	t2_version version{};                         /*!< T2 version */
	uint32_t t2_flags{};                          /*!< T2 Function Flags use T2EN_XXXXX define to config) */
	constellation_t2_l1 l1_constellation{};       /*!< L1 constellation */
	constellation_t2_plp plp_constellation{};     /*!< PLP constellation */
	t2_issy issy{};                               /*!< ISSY */
	t2_fft_mode fft{};                            /*!< FFT mode */
	t2_code_rate coderate{};                      /*!< Code Rate */
	t2_guard_interval guardinterval{};            /*!< Guard Interval */
	t2_pilot_pattern pilotpattern{};              /*!< Pilot Pattern */
	t2_fec_length fectype{};                      /*!< FEC-Block length */
	uint16_t network_id{};                        /** Network id  */
	uint16_t system_id{};                         /** System id   */
	uint16_t fecblock_nums{};                     /** FEC block numbers (0 mean auto) */
	uint16_t symbol_nums{};                       /** SYMBOL numbers (0 mean auto) */
	uint16_t ti_ni{};                             /** Interleaving NI number */
	uint16_t recv{};
};

#define J83A_SYMBOL_MIN		5000
#define J83A_SYMBOL_MAX		8000

#define J83B_Q64_SYMBOL		5056941
#define J83B_Q256_SYMBOL	5360537

#define J83C_SYMBOL_RATE	5274

#define J83B_Q64_BITRATE		26970355
#define J83B_Q256_BITRATE		38810705

#define J83_FRAME_PACKETNUMS	60

#define DTMB_6MHZ	(uint32_t)(5.67 * 1000000)
#define DTMB_8MHZ	(uint32_t)(7.56 * 1000000)

#define DTMB_FRAME_CRBASE		3780
#define DTMB_FRAME_CRDATA		3744
#define DTMB_CODE_BASE			7488

#define ISDB_T_TSP_LEN			204
#define ISDB_T_BTS_SYMBOLNUMS	204
#define ISDB_T_DATA_CARRIER		96
#define ISDB_T_SEGMENT_NUMS		13
#define ISDB_T_BTS_BASECLK		408

#define ATSC_8VSB_SYMBOLRATE	10762238	
#define VSB_SEGMENT_SB_NUMS		832
#define VSB_FIELD_SEGMENT_NUMS	313
#define VSB_FRAME_FIELD_NUMS	2

#define VSB_FIELD_PACKET_NUMS	(VSB_FIELD_SEGMENT_NUMS - 1)
#define VSB_FRAME_PACKET_NUMS	(VSB_FIELD_PACKET_NUMS * VSB_FRAME_FIELD_NUMS)

enum dac_ifmode
{
	ifmode_disable = 1,        /*!< IQ mode    */
	ifmode_freq = 2,           /*!< IF mode with frequency */
	ifmode_iqoffset = 3,     /*!< IQ mode with offset frequency  */
	ifmode_inv_freq = 4,   /*!< IF mode with frequency and spectrum inversion  */
};

inline std::string ToString(dac_ifmode value)
{
	switch (value)
	{
	case ifmode_disable:
		{
			return "disable";
		}
	case ifmode_freq:
		{
			return "if_freq";
		}
	case ifmode_iqoffset:
		{
			return "iq_offset";
		}
	case ifmode_inv_freq:
		{
			return "if_inv_freq";
		}
	default:
		{
			return "Unknown DAC IF mode";
		}
	}
}

#define DVB_T_BW_MIN                    5
#define DVB_T_BW_MAX                    8

/**
 * @ingroup mod_props
 * Modulator Parameters
 */
class ModulatorParamCollection
{
public:
	uint32_t _bandwidth_symbolrate = 0;      /*!< config bandwidth [DVB-T|T2,ISDB-T,DTMB] or symbol rate [J83a]   */
	modulator_type _type{};                /*!< modulation type */
	dac_ifmode _ifmode{};                  /*!< select current DAC output mode */
	uint32_t iffreq_offset = 0;             /*!< DAC output frequency (KHz)  */

	/**
	 * @brief 如果精度小于等于 1000 kHz，需要设置 iffreq_offset 并将 dac_ifmode 设为 ifmode_iqoffset
	*/
	uint32_t freqkhz{};

	isdb_t_param _isdb_t_param;    /*!< isdb-t parameters*/
	dvb_t_param _dvb_t_param;      /*!< dvb-t parameters*/
	dtmb_param _dtmb_param;        /*!< dtmb parameters*/
	atsc_param _atsc_param;        /*!< atsc parameters*/
	j83a_param _j83a_param;        /*!< j83a parameters*/
	j83b_param _j83b_param;        /*!< j83b parameters*/
	j83c_param _j83c_param;        /*!< j83c parameters*/
	dvb_t2_param _dvb_t2_param;    /*!< dvb-t2 parameters*/

public:
	vk_result modulator_param_reset(modulator_type type)
	{
		if (type == modulator_isdb_t)
		{
			_ifmode = ifmode_iqoffset;
			iffreq_offset = 143;
		}
		else
		{
			_ifmode = ifmode_disable;
			iffreq_offset = 0;
		}

		static const uint32_t default_bw_sb[modulator_mod_nums] = { 6,5720,0,0,8,6,5309,8, };
		_bandwidth_symbolrate = default_bw_sb[type];

		switch (type)
		{
		case modulator_type::modulator_dvb_t:
			{
				_dvb_t_param = dvb_t_param{
					dvbt_constellation_mode::dvb_t_qam64,
					fft_8k,
					guard_interval_1_16,
					coderate_5_6,
				};

				break;
			}
		case modulator_type::modulator_j83a:
			{
				_j83a_param = j83a_param{ j83a_constellation_mode::j83a_qam256, };
				break;
			}
		case modulator_type::modulator_atsc:
			{
				_atsc_param = atsc_param{ atsc_constellation_mode::_8vsb, };
				break;
			}
		case modulator_type::modulator_j83b:
			{
				_j83b_param = j83b_param{ j83b_constellation_mode::j83b_qam256, };
				break;
			}
		case modulator_type::modulator_dtmb:
			{
				_dtmb_param = dtmb_param{
					dtmb_constellation_mode::dtmb_qam64,
					dtmb_interleaved_720,
					dtmb_code_rate_0_8,
					dtmb_carrier_3780,
					dtmb_framesync_945,
				};
				break;
			}
		case modulator_type::modulator_isdb_t:
			{
				_isdb_t_param = isdb_t_param{
					isdbt_constellation_mode::isdb_t_qam64,
					fft_8k,
					guard_interval_1_16,
					coderate_5_6,
					isdb_t_interleaved_mode3,
					0,
				};
				break;
			}
		case modulator_type::modulator_j83c:
			{
				_j83c_param = j83c_param{ j83c_constellation_mode::j83c_qam256, };
				break;
			}
		case modulator_type::modulator_dvb_t2:
			{
				_dvb_t2_param = dvb_t2_param{
					t2_ver_131,
					T2EN_CONSTELLATION_ROTATION | T2EN_INPUT_TS_HEM,
					t2_l1_qam16,
					t2_plp_qam256,
					t2_issy_disable,
					t2_fft_8k,
					t2_coderate_4_5,
					t2_gi_1_32,
					pilot_pattern_7,
					t2_fec_16200,
					0x3085,
					0x8001,
					0,			/*	_fecblock nums, symbol nums and _nti set 0 mean auto	*/
					0,
					0,
					0,
				};
				break;
			}
		default:
			{
				throw jc::Exception("不支持的制式");
			}
		}

		return vatek_success;
	}

	void printf_modulation_param()
	{
		std::cout << "-------------------------------------\n";
		std::cout << "	   Modulation param\n";
		std::cout << "-------------------------------------\n";
		std::cout << std::format("{:<40} : {}\n", "Frequency(khz)", freqkhz);
		std::cout << std::format("{:<40} : {}\n", "Type", ToString(_type));
		std::cout << std::format("{:<40} : {}\n", "Bandwidth / Symbolrate", _bandwidth_symbolrate);
		std::cout << std::format("{:<40} : {}\n", "IF mode", ToString(_ifmode));
		std::cout << std::format("{:<40} : {}\n", "IF mode offset", iffreq_offset);
		if (_type == modulator_atsc)
		{
			std::cout << std::format("{:<40} : {}\n", "Constellation", ToString(_atsc_param.constellation));
		}
		else if (_type == modulator_dvb_t)
		{
			std::cout << std::format("{:<40} : {}\n", "Constellation", ToString(_dvb_t_param.constellation));
			std::cout << std::format("{:<40} : {}\n", "FFT", ToString(_dvb_t_param.fft));
			std::cout << std::format("{:<40} : {}\n", "Guard Interval", ToString(_dvb_t_param.guardinterval));
			std::cout << std::format("{:<40} : {}\n", "Code Rate", ToString(_dvb_t_param.coderate));
		}
		else if (_type == modulator_isdb_t)
		{
			std::cout << std::format("{:<40} : {}\n", "Constellation", ToString(_isdb_t_param.constellation));
			std::cout << std::format("{:<40} : {}\n", "FFT", ToString(_isdb_t_param.fft));
			std::cout << std::format("{:<40} : {}\n", "Guard Interval", ToString(_isdb_t_param.guardinterval));
			std::cout << std::format("{:<40} : {}\n", "Code Rate", ToString(_isdb_t_param.coderate));
			std::cout << std::format("{:<40} : {}\n", "Time Interleaved", ToString(_isdb_t_param.timeinterleaved));
			std::cout << std::format("{:<40} : {:08x}\n", "ISDB-T Flags", _isdb_t_param.isdb_t_flags);
		}
		else if (_type == modulator_dtmb)
		{
			std::cout << std::format("{:<40} : {}\n", "Constellation", ToString(_dtmb_param.constellation));
			std::cout << std::format("{:<40} : {}\n", "Time Interleaved", ToString(_dtmb_param.timeinterleaved));
			std::cout << std::format("{:<40} : {}\n", "Code Rate", ToString(_dtmb_param.coderate));
			std::cout << std::format("{:<40} : {}\n", "Carrier Mode", ToString(_dtmb_param.carriermode));
			std::cout << std::format("{:<40} : {}\n", "Frame Header", ToString(_dtmb_param.framesync));
		}
		else if (_type == modulator_j83a)
		{
			std::cout << std::format("{:<40} : {}\n", "Constellation", ToString(_j83a_param.constellation));
		}
		else if (_type == modulator_j83b)
		{
			std::cout << std::format("{:<40} : {}\n", "Constellation", ToString(_j83b_param.constellation));
		}
		else if (_type == modulator_j83c)
		{
			std::cout << std::format("{:<40} : {}\n", "Constellation", ToString(_j83c_param.constellation));
		}
		else if (_type == modulator_dvb_t2)
		{
			std::cout << std::format("{:<40} : {}\n", "Version", ToString(_dvb_t2_param.version));
			std::cout << std::format("{:<40} : {:08x}\n", "T2 Flags", _dvb_t2_param.t2_flags);
			std::cout << std::format("{:<40} : {}\n", "L1 Constellation", ToString(_dvb_t2_param.l1_constellation));
			std::cout << std::format("{:<40} : {}\n", "PLP Constellation", ToString(_dvb_t2_param.plp_constellation));
			std::cout << std::format("{:<40} : {}\n", "ISSY", ToString(_dvb_t2_param.issy));
			std::cout << std::format("{:<40} : {}\n", "FFT", ToString(_dvb_t2_param.fft));
			std::cout << std::format("{:<40} : {}\n", "Code Rate", ToString(_dvb_t2_param.coderate));
			std::cout << std::format("{:<40} : {}\n", "Guard Interval", ToString(_dvb_t2_param.guardinterval));
			std::cout << std::format("{:<40} : {}\n", "Pilot Pattern", ToString(_dvb_t2_param.pilotpattern));
			std::cout << std::format("{:<40} : {}\n", "FEC-Block length", ToString(_dvb_t2_param.fectype));
			std::cout << std::format("{:<40} : {:08x}\n", "Network ID", _dvb_t2_param.network_id);
			std::cout << std::format("{:<40} : {:08x}\n", "System ID", _dvb_t2_param.system_id);
			std::cout << std::format("{:<40} : {:08x}\n", "FEC block numbers (0 mean auto)", _dvb_t2_param.fecblock_nums);
			std::cout << std::format("{:<40} : {:08x}\n", "Symbol numbers (0 mean auto)", _dvb_t2_param.symbol_nums);
			std::cout << std::format("{:<40} : {:08x}\n", "ti_ni", _dvb_t2_param.ti_ni);
			std::cout << std::format("{:<40} : {:08x}\n", "recv", _dvb_t2_param.recv);
		}

		std::cout << "=====================================\r\n";
	}
};

#endif
