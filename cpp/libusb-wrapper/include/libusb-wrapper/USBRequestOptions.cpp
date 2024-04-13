#include"USBRequestOptions.h"

using namespace libusb;

USBRequestOptions::USBRequestOptions(DataDirection dir, RequestType type, RecipientType recipient)
{
	SetDataDirection(dir);
	SetRequestType(type);
	SetRecipientType(recipient);
}

USBRequestOptions::operator uint8_t() const
{
	return _options_byte;
}

USBRequestOptions::DataDirection USBRequestOptions::GetDataDirection()
{
	return DataDirection(_options_byte & _data_direction_mask);
}

void USBRequestOptions::SetDataDirection(USBRequestOptions::DataDirection value)
{
	// 清除旧的值。将那些位全部清零。
	_options_byte &= ~_data_direction_mask;

	// value 按位与 _data_direction_mask 是为了防止 value 是强制转换过来的，不是真正的枚举值。
	_options_byte |= (uint8_t)value & _data_direction_mask;
}

USBRequestOptions::RequestType USBRequestOptions::GetRequestType()
{
	return RequestType(_options_byte & _request_type_mask);
}

void USBRequestOptions::SetRequestType(USBRequestOptions::RequestType value)
{
	_options_byte &= ~_request_type_mask;
	_options_byte |= (uint8_t)value & _request_type_mask;
}

USBRequestOptions::RecipientType USBRequestOptions::GetRecipientType()
{
	return RecipientType(_options_byte & _recipient_type_mask);
}

void USBRequestOptions::SetRecipientType(USBRequestOptions::RecipientType value)
{
	_options_byte &= ~_recipient_type_mask;
	_options_byte |= (uint8_t)value & _recipient_type_mask;
}
