#include"UsbConfigDescriptorWrapper.h"

using namespace libusb;

UsbConfigDescriptorWrapper::UsbConfigDescriptorWrapper(libusb_config_descriptor *descriptor)
{
	_wrapped_obj = descriptor;
}

UsbConfigDescriptorWrapper::~UsbConfigDescriptorWrapper()
{
	libusb_free_config_descriptor(_wrapped_obj);
	_wrapped_obj = nullptr;
}
