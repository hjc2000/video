#pragma once
#include<cstdint>

class USBRequestType
{
	#pragma region pos, mask
private:
	/// <summary>
	///		从第 0 位开始
	/// </summary>
	static uint8_t const _recipient_type_pos = 0;
	/// <summary>
	///		占据 5 位
	/// </summary>
	static uint8_t const _recipient_type_mask = 0x1f << _recipient_type_pos;

	/// <summary>
	///		从第 5 位开始
	/// </summary>
	static uint8_t const _request_type_pos = 5;
	/// <summary>
	///		占据 2 位
	/// </summary>
	static uint8_t const _request_type_mask = 0x3 << _request_type_pos;

	/// <summary>
	///		从第 7 位开始
	/// </summary>
	static uint8_t const _data_direction_pos = 7;
	/// <summary>
	///		占据 1 位
	/// </summary>
	static uint8_t const _data_direction_mask = 0x1 << _data_direction_pos;
	#pragma endregion

	#pragma region 请求属性枚举类型定义
public:
	/// <summary>
	///		接收者的类型
	/// </summary>
	enum class RecipientType :uint8_t
	{
		/// <summary>
		///		接收者是一个设备
		/// </summary>
		Device = 0x00 << _recipient_type_pos,

		/// <summary>
		///		接收者是一个接口
		/// </summary>
		Interface = 0x01 << _recipient_type_pos,

		/// <summary>
		///		接收者是一个端点
		/// </summary>
		Endpoint = 0x02 << _recipient_type_pos,

		/// <summary>
		///		其它类型的接收者
		/// </summary>
		Other = 0x03 << _recipient_type_pos
	};

	/// <summary>
	///		请求类型
	/// </summary>
	enum class RequestType :uint8_t
	{
		/// <summary>
		///		标准请求
		/// </summary>
		Standard = 0x00 << _request_type_pos,

		/// <summary>
		///		类请求
		/// </summary>
		Class = 0x01 << _request_type_pos,

		/// <summary>
		///		厂商请求
		/// </summary>
		Vendor = 0x02 << _request_type_pos
	};

	/// <summary>
	///		数据传输方向
	/// </summary>
	enum class DataDirection :uint8_t
	{
		HostToDevice = 0 << _data_direction_pos,
		DeviceToHost = 1 << _data_direction_pos
	};
	#pragma endregion

public:
	/// <summary>
	///		默认构造函数。
	///		默认构造出的请求属性不会符合你的期望。记得调用 Set 系列方法设置请求属性。
	/// </summary>
	USBRequestType() {}

	/// <summary>
	///		
	/// </summary>
	/// <param name="dir">数据传输方向</param>
	/// <param name="type">请求类型</param>
	/// <param name="recipient">接收者的类型</param>
	USBRequestType(DataDirection dir, RequestType type, RecipientType recipient);

private:
	uint8_t _b = 0;

public:
	operator uint8_t() const;

	USBRequestType::DataDirection GetDataDirection();
	void SetDataDirection(USBRequestType::DataDirection value);

	USBRequestType::RequestType GetRequestType();
	void SetRequestType(USBRequestType::RequestType value);

	USBRequestType::RecipientType GetRecipientType();
	void SetRecipientType(USBRequestType::RecipientType value);
};
