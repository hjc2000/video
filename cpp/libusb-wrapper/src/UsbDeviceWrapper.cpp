#include<UsbDeviceWrapper.h>

libusb_device_descriptor UsbDeviceWrapper::GetDescriptor()
{
	// 控制台输出时，输出 4 位，高位补 0 的方法：
	// cout << std::format("{:04x}", desc.idVendor) << endl;
	libusb_device_descriptor desc;
	int ret = libusb_get_device_descriptor(_wrapped_obj, &desc);
	if (ret < 0)
	{
		throw jc::Exception(UsbErrorCodeToString(ret));
	}

	return desc;
}

void UsbDeviceWrapper::Open()
{
	if (_device_handle)
	{
		std::cout << "设备已打开，不要重复打开" << std::endl;
		return;
	}

	libusb_device_handle *handle = nullptr;
	int ret = libusb_open(_wrapped_obj, &handle);
	if (ret)
	{
		throw jc::Exception(UsbErrorCodeToString(ret));
	}

	_device_handle = shared_ptr<libusb_device_handle>{
		handle,
		[](libusb_device_handle *handle)
		{
			libusb_close(handle);
		}
	};
}

int UsbDeviceWrapper::ControlTransfer(USBRequestType request_type, uint8_t request_cmd,
	uint16_t value, uint16_t index, uint8_t *data, uint16_t length, uint32_t timeout)
{
	return libusb_control_transfer(
		_device_handle.get(),
		request_type,
		request_cmd,
		value,
		index,
		data,
		length,
		timeout
	);
}

uint16_t UsbDeviceWrapper::GetStatus()
{
	uint8_t status_buf[2];
	int have_read = ControlTransfer(
		USBRequestType{
			USBRequestType::DataDirection::DeviceToHost,
			USBRequestType::RequestType::Standard,
			USBRequestType::RecipientType::Device
		},
		libusb_standard_request::LIBUSB_REQUEST_GET_STATUS,
		0,
		0,
		status_buf,
		sizeof(status_buf),
		0
	);

	if (have_read != sizeof(status_buf))
	{
		cout << CODE_POS_STR << "获取状态信息失败" << endl;
		throw jc::Exception();
	}

	return (status_buf[1] << 8) | status_buf[0];
}

int UsbDeviceWrapper::BulkTransfer(uint8_t endpoint, uint8_t *data, int length, uint32_t timeout)
{
	// 已传输的字节数
	int transferred = 0;
	int ret = libusb_bulk_transfer(
		_device_handle.get(),
		endpoint,
		data,
		length,
		&transferred,
		timeout
	);

	if (ret < 0)
	{
		cout << CODE_POS_STR << UsbErrorCodeToString(ret) << endl;
		throw jc::Exception();
	}

	return transferred;
}

std::vector<shared_ptr<UsbConfigDescriptorWrapper>> UsbDeviceWrapper::GetConfigDescriptorList()
{
	std::vector<shared_ptr<UsbConfigDescriptorWrapper>> config_list;
	libusb_device_descriptor descriptor = GetDescriptor();
	for (uint8_t i = 0; i < descriptor.bNumConfigurations; i++)
	{
		libusb_config_descriptor *config = nullptr;
		int ret = libusb_get_config_descriptor(_wrapped_obj, i, &config);
		if (ret)
		{
			std::cout << CODE_POS_STR << UsbErrorCodeToString(ret) << endl;
			continue;
		}

		shared_ptr<UsbConfigDescriptorWrapper> wrapper{ new UsbConfigDescriptorWrapper{config} };
		config_list.push_back(wrapper);
	}

	return config_list;
}
