#pragma once
#include<jccpp/Exception.h>
#include<jccpp/Wrapper.h>
#include<jccpp/define.h>
#include<libusb-1.0/libusb.h>
#include<libusb-wrapper/UsbError.h>

class UsbContextWrapper :public Wrapper<libusb_context>
{
	libusb_context *_wrapped_obj;

public:
	/**
	 * @brief 初始化 USB 上下文。
	 * @exception Exception 初始化失败会抛出异常。
	*/
	UsbContextWrapper()
	{
		int result = libusb_init(&_wrapped_obj);
		if (result < 0)
		{
			cout << CODE_POS_STR << "初始化失败" << endl;
			throw jc::Exception();
		}

		libusb_set_debug(_wrapped_obj, LIBUSB_LOG_LEVEL_INFO);
	}

	~UsbContextWrapper()
	{
		libusb_exit(_wrapped_obj);
		_wrapped_obj = nullptr;
	}

	libusb_context *&WrappedObj() override
	{
		return _wrapped_obj;
	}
	libusb_context *WrappedObj() const override
	{
		return _wrapped_obj;
	}
};
