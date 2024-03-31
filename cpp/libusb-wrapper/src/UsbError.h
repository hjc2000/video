#include<libusb-1.0/libusb.h>
#include<string>

/// <summary>
///		将 liusb 的错误代码转为字符串的说明信息。
/// </summary>
/// <param name="error_code"></param>
/// <returns></returns>
std::string UsbErrorCodeToString(int error_code);
