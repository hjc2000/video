#include"libusb-wrapper/UsbError.h"

std::string libusb::UsbErrorCodeToString(int error_code)
{
	return libusb_error_name(error_code);
}
