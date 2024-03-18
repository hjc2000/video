#include "tool_printf.h"
#include<A3Device.h>
#include<Exception.h>
#include<FileStream.h>
#include<FileTsSource.h>
#include<JoinedStream.h>
#include<TaskCompletionSignal.h>
#include<TsPusher.h>
#include<UsbA3Device.h>
#include<UsbA3Device.h>
#include<chrono>
#include<iostream>
#include<memory>
#include<semaphore>

using namespace std::chrono;
using namespace std::this_thread;

using std::cout;
using std::endl;
using std::shared_ptr;

/// <summary>
///		解析命令行
/// </summary>
/// <param name="argc">命令行参数的数量</param>
/// <param name="argv">命令行参数的数组</param>
/// <param name="pustream"></param>
/// <returns>解析命令行成功会返回 TsSource 对象，否则返回 nullptr</returns>
shared_ptr<TsSource> parser_cmd_source(int32_t argc, char **argv)
{
	///>>> 执行到这里说明参数数量 >= 4

	// 如果参数大于等于 4，则第 3 个参数必须是视频源的协议，第 4 个参数是视频源的 URL
	shared_ptr<TsSource> stream_source;
	if (strcmp(argv[1], "file") == 0)
	{
		stream_source = shared_ptr<TsSource>{ new FileTsSource{} };
		FileTsSource::cross_stream_file_get(argv[2], static_pointer_cast<FileTsSource>(stream_source));
	}
	else
	{
		throw jc::Exception("不支持的协议");
	}

	return stream_source;
}

int main()
{
	#if 01
	// ./app_stream dvbt file qq.ts
	const char *args[] = {
		"./app_stream",
		"file",
		"idol.ts",
	};
	#else
	// ./app_stream dvbt udp udp://127.0.0.1:40000
	const char *args[] = {
		"./app_stream",
		"udp",
		"udp://localhost:1234",
	};
	#endif

	shared_ptr<TsSource> streamsource = parser_cmd_source(4, (char **)args);

	/*
		step 1 :
		- initialized supported device and open
	*/

	UsbContextWrapper usb_contex{};
	UsbDeviceListWrapper usb_list{};
	usb_list.FindDevices(usb_contex);
	vector<shared_ptr<UsbA3Device>> usb_interface_list = UsbA3Device::FindDevices(usb_type_all, usb_list);
	if (usb_interface_list.size() == 0)
	{
		cout << "找不到设备" << endl;
		return -1;
	}

	shared_ptr<UsbA3Device> a3device = usb_interface_list[0];
	chip_info pinfo = a3device->GetChipInfo();
	printf_chip_info(&pinfo);

	int nres = vatek_success;
	nres = streamsource->Start();

	// 设置调制器
	ModulatorParamCollection modulator_param;
	modulator_param.modulator_param_reset(modulator_type::modulator_dvb_t);
	modulator_param.freqkhz = 474000;
	modulator_param._ifmode = dac_ifmode::ifmode_iqoffset;
	modulator_param.iffreq_offset = 0;
	modulator_param._bandwidth_symbolrate = 6;
	modulator_param._type = modulator_type::modulator_dvb_t;
	//modulator_param._dvb_t_param.constellation = constellation_mode::dvb_t_qam64;
	//modulator_param._dvb_t_param.fft = fft_mode::fft_8k;
	//modulator_param._dvb_t_param.guardinterval = guard_interval::guard_interval_1_16;
	//modulator_param._dvb_t_param.coderate = code_rate::coderate_7_8;
	modulator_param.printf_modulation_param();

	a3device->SetStreamMode(RemuxMode::stream_remux);
	a3device->SetPcrAdjustMode(pcr_adjust_mode::pcr_adjust);
	a3device->StartTransformStreamOutput(modulator_param);

	#pragma region 操作R2
	calibration_param calibration_param_to_print;
	cout << "更改前: " << endl;

	calibration_param_to_print = a3device->CalibrationParam();
	cout << calibration_param_to_print.ToString() << endl;

	// 更改
	calibration_param_to_print._dac._ioffset = 5;
	calibration_param_to_print._dac._qoffset = 5;
	calibration_param_to_print._dac._igain = 0;
	calibration_param_to_print._dac._qgain = 0;
	calibration_param_to_print._r2_tune._phaseoffset = 0;
	calibration_param_to_print._r2_tune._imgoffset = 0;
	calibration_param_to_print._r2_tune._ioffset = 5;
	calibration_param_to_print._r2_tune._qoffset = 5;
	a3device->SetCalibrationParam(calibration_param_to_print);
	//a3device->SaveCalibrationParam();

	cout << "R2Power = " << (int)a3device->R2Power() << endl;
	a3device->SetR2Power(0);
	cout << "R2Power = " << (int)a3device->R2Power() << endl;

	// 更改后再次读取并打印
	cout << endl;
	cout << "更改后: " << endl;
	calibration_param_to_print = a3device->CalibrationParam();
	cout << calibration_param_to_print.ToString() << endl;
	#pragma endregion

	TsPusher ts_pusher{ a3device };
	ts_pusher.Start();
	CancellationTokenSource cancel_copying_stream;
	auto cancel_copying_stream_token = cancel_copying_stream.Token();
	TaskCompletionSignal copying_stream_thread_stopped{ false };
	thread([&]()
	{
		shared_ptr<FileStream> fstream = FileStream::Open("idol.ts");
		JoinedStream jstream;
		jstream._stream_end_callback = [&]()
		{
			fstream->SetPosition(0);
			jstream.AppendStream(fstream);
		};

		try
		{
			jstream.CopyTo(&ts_pusher, cancel_copying_stream_token);
		}
		catch (jc::Exception &e)
		{
			cout << CODE_POS_STR << e.what() << endl;
			cout << "线程退出" << endl;
		}

		copying_stream_thread_stopped.SetResult();
	}).detach();

	/*
		step 3 :
			- source_sync_get_buffer would call from internal when usb_stream had valid buffer
			- main loop could polling usb_stream status and check information
			- when finished close usb_stream
	*/

	cout << endl;
	cout << "USB Stream Start." << endl << endl;
	cout << "Press any key to stop." << endl << endl;
	cout << "=====================================" << endl;
	int count = 0;
	int error = 0;

	CancellationTokenSource cancel_getting_state;
	auto cancel_getting_state_token = cancel_getting_state.Token();
	TaskCompletionSignal getting_state_thread_stopped{ false };
	std::thread([&]()
	{
		try
		{
			while (!cancel_getting_state_token->IsCancellationRequested())
			{
				printf(
					"Data:[%d]  Current:[%d]" "\r\n",
					a3device->DataBitrate(),
					a3device->CurBitrate()
				);

				if (a3device->CurBitrate() == 0)
				{
					error++;
				}

				count++;

				if (error >= 30)
				{
					cout << "A3 Fail. Press any key to stop.\r\n";
				}

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
		catch (jc::Exception &e)
		{
			cout << CODE_POS_STR << e.what() << endl;
			cout << "线程退出" << endl;
		}

		getting_state_thread_stopped.SetResult();
	}).detach();

	/*
		setp 4 :
		before quit demo stop and free both device and source
	*/

	std::getchar();
	ts_pusher.Close();
	cancel_copying_stream.Cancel();
	cancel_getting_state.Cancel();
	copying_stream_thread_stopped.Wait();
	getting_state_thread_stopped.Wait();
	a3device->Reboot();

	printf_app_end();
	return 0;
}
