#pragma once
#include<jccpp/Wrapper.h>
#include<libusb-wrapper/UsbContextWrapper.h>

namespace libusb
{
	class UsbConfigDescriptorWrapper :public Wrapper<libusb_config_descriptor>
	{
		libusb_config_descriptor *_wrapped_obj = nullptr;
		UsbConfigDescriptorWrapper(UsbConfigDescriptorWrapper &value) = delete;
		UsbConfigDescriptorWrapper(UsbConfigDescriptorWrapper &&value) = delete;

	public:
		UsbConfigDescriptorWrapper(libusb_config_descriptor *descriptor);
		~UsbConfigDescriptorWrapper();

		libusb_config_descriptor *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		libusb_config_descriptor *WrappedObj() const override
		{
			return _wrapped_obj;
		}

		bool operator==(UsbConfigDescriptorWrapper const &another)
		{
			return this == &another;
		}

		uint8_t InterfaceCount()
		{
			return _wrapped_obj->bNumInterfaces;
		}

		libusb_interface const &GetInterface(uint8_t index)
		{
			return _wrapped_obj->interface[index];
		}
	};
}
