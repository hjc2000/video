#pragma once
#include<Exception.h>
#include<List.h>
#include<USBRequestType.h>
#include<UsbConfigDescriptorWrapper.h>
#include<Wrapper.h>
#include<define.h>
#include<iostream>
#include<libusb-1.0/libusb.h>
#include<memory>
#include<vector>

using std::shared_ptr;

class UsbDeviceWrapper :public Wrapper<libusb_device>
{
public:
	/// <summary>
	///		默认构造函数。此时并没有实际指向任何一个 USB 设备，所以调用任何方法都将引发异常。
	/// </summary>
	UsbDeviceWrapper() {}
	UsbDeviceWrapper(libusb_device *device);
	UsbDeviceWrapper(UsbDeviceWrapper const &other);
	~UsbDeviceWrapper();

	UsbDeviceWrapper &operator=(UsbDeviceWrapper const &other);

private:
	void Ref(libusb_device *device);
	void Unref();

	shared_ptr<libusb_device_handle> _device_handle = nullptr;

public:
	/// <summary>
	///		获取设备的描述信息。不需要打开设备就可获取。
	/// </summary>
	/// <returns></returns>
	libusb_device_descriptor GetDescriptor();

	/// <summary>
	///		打开设备。
	/// </summary>
	void Open();

	/// <summary>
	///		控制传输。需要打开设备。
	///		一次传输称为一个数据包。数据包和 http 报文类似，有头部和主体。
	/// </summary>
	/// <param name="request_type">请求类型</param>
	/// <param name="request_cmd">请求执行的命令</param>
	/// <param name="value">数据包的值。类似于 http 报文的头部。</param>
	/// <param name="index">数据包的索引。应该类似于 http 的请求路径之类的。</param>
	/// <param name="data">数据包缓冲区。根据传输方向，可以为发送缓冲区和接收缓冲区。类似于 http 报文的主体。</param>
	/// <param name="length">数据包的长度。</param>
	/// <param name="timeout">超时时间。单位：毫秒。传入 0 无限超时。</param>
	/// <returns>
	///		成功则返回实际传输的缓冲区字节数。失败返回负数的错误代码。
	///		如果发送时就没有缓冲区，则成功时返回值等于 0.
	///		如果你只关心是否失败，判断返回值是否小于 0 就行了。小于 0 表示失败，否则就是成功。
	/// </returns>
	int ControlTransfer(
		USBRequestType request_type,
		uint8_t request_cmd,
		uint16_t value,
		uint16_t index,
		uint8_t *data,
		uint16_t length,
		uint32_t timeout
	);

	/// <summary>
	///		获取状态代码。需要打开设备。
	/// </summary>
	/// <returns>状态代码。（将状态信息的 2 个字节按照小端序拼接成 uint16_t）</returns>
	uint16_t GetStatus();

	/// <summary>
	///		块传输
	/// </summary>
	/// <param name="endpoint">
	///		传输端点和方向。
	///		* 最高位表示传输方向。最高位为 1，表示输入（设备到主机），最高位为 0，表示输出（主机到设备）。
	///		* 低 7 位表示端点地址。
	/// </param>
	/// <param name="data">数据缓冲区。根据数据传输方向，可为输入缓冲区和输出缓冲区。</param>
	/// <param name="length">最大传输字节数。实际传输的字节数会小于等于此值。</param>
	/// <param name="timeout">超时时间。单位：毫秒。传入 0 表示无限超时。</param>
	/// <returns>返回已传输的字节数。失败会抛出异常。</returns>
	int BulkTransfer(uint8_t endpoint, uint8_t *data, int length, uint32_t timeout);

	/// <summary>
	///		获取配置文件描述符列表。
	/// </summary>
	/// <returns></returns>
	std::vector<shared_ptr<UsbConfigDescriptorWrapper>> GetConfigDescriptorList();

	void ClaimInterface();
	void ClaimInterface(UsbConfigDescriptorWrapper &config);
	void ClaimInterface(libusb_interface const &interface);
	void ClaimInterface(int interface_number);
};
