#pragma once
#include<cstdint>

class USBRequestType
{
public:
	#pragma region 请求属性枚举类型定义
	/// <summary>
	///		请求类型
	/// </summary>
	enum class RequestType :uint8_t
	{
		/**
		* @brief 标准请求
		*/
		Standard = 0x00,

		/**
		* @brief 类请求
		*/
		Class = 0x01,

		/**
		* @brief 厂商请求
		*/
		Vendor = 0x02
	};

	/// <summary>
	///		数据传输方向的枚举
	/// </summary>
	enum class DataDirection :uint8_t
	{
		HostToDevice = 0,
		DeviceToHost = 1
	};

	/// <summary>
	///		接收者的类型
	/// </summary>
	enum class RecipientType :uint8_t
	{
		/**
		* @brief 接收者是一个设备
		*/
		Device = 0x00,

		/**
		* @brief 接收者是一个接口
		*/
		Interface = 0x01,

		/**
		* @brief 接收者是一个端点
		*/
		Endpoint = 0x02,
		Other = 0x03
	};
	#pragma endregion

public:
	/**
	 * @brief
	 * @param dir 数据传输方向
	 * @param type 请求类型
	 * @param recipient 接收者的类型
	*/
	USBRequestType(DataDirection dir, RequestType type, RecipientType recipient)
	{
		_request_type._direction = dir;
		_request_type._type = type;
		_request_type._recipient = recipient;
	}

public:
	uint8_t ToByte() const
	{
		return _request_type._byte;
	}

	operator uint8_t() const
	{
		return ToByte();
	}

private:
	union
	{
		struct
		{
			RecipientType _recipient : 5;	// 接收者位，占据最低的5位
			RequestType _type : 2;			// 类型位，接下来的2位
			DataDirection _direction : 1;	// 方向位，最高位
		};

		uint8_t _byte;	// 用于直接访问整个字节
	} _request_type;
};
