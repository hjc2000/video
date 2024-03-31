#include"USBRequestType.h"

USBRequestType::USBRequestType(DataDirection dir, RequestType type, RecipientType recipient)
{
	SetDataDirection(dir);
	SetRequestType(type);
	SetRecipientType(recipient);
}

USBRequestType::operator uint8_t() const
{
	return _b;
}

USBRequestType::DataDirection USBRequestType::GetDataDirection()
{
	return DataDirection(_b & _data_direction_mask);
}

void USBRequestType::SetDataDirection(USBRequestType::DataDirection value)
{
	// 清除旧的值。将那些位全部清零。
	_b &= ~_data_direction_mask;

	// value 按位与 _data_direction_mask 是为了防止 value 是强制转换过来的，不是真正的枚举值。
	_b |= (uint8_t)value & _data_direction_mask;
}

USBRequestType::RequestType USBRequestType::GetRequestType()
{
	return RequestType(_b & _request_type_mask);
}

void USBRequestType::SetRequestType(USBRequestType::RequestType value)
{
	_b &= ~_request_type_mask;
	_b |= (uint8_t)value & _request_type_mask;
}

USBRequestType::RecipientType USBRequestType::GetRecipientType()
{
	return RecipientType(_b & _recipient_type_mask);
}

void USBRequestType::SetRecipientType(USBRequestType::RecipientType value)
{
	_b &= ~_recipient_type_mask;
	_b |= (uint8_t)value & _recipient_type_mask;
}
