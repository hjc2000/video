#include"UsbError.h"

std::string UsbErrorCodeToString(int error_code)
{
	return libusb_error_name(error_code);
}