#include<libusb-1.0/libusb.h>
#include<string>

inline std::string UsbErrorCodeToString(int error_code)
{
	return libusb_error_name(error_code);
}
