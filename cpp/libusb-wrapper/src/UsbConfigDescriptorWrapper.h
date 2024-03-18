#pragma once
#include<UsbContextWrapper.h>
#include<Wrapper.h>

class UsbConfigDescriptorWrapper :public Wrapper<libusb_config_descriptor>
{
private:
	UsbConfigDescriptorWrapper(UsbConfigDescriptorWrapper &value) = delete;
	UsbConfigDescriptorWrapper(UsbConfigDescriptorWrapper &&value) = delete;

public:
	UsbConfigDescriptorWrapper(libusb_config_descriptor *descriptor)
	{
		_wrapped_obj = descriptor;
	}

	~UsbConfigDescriptorWrapper()
	{
		libusb_free_config_descriptor(_wrapped_obj);
		_wrapped_obj = nullptr;
	}

	bool operator==(UsbConfigDescriptorWrapper const &another)
	{
		return this == &another;
	}

public:
	uint8_t InterfaceCount()
	{
		return _wrapped_obj->bNumInterfaces;
	}

	libusb_interface const &GetInterface(uint8_t index)
	{
		return _wrapped_obj->interface[index];
	}
};
