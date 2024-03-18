#pragma once
#include<UsbContextWrapper.h>
#include<UsbDeviceWrapper.h>
#include<functional>
#include<stdint.h>
#include<vector>

using std::vector;
using std::function;

/**
 * @brief 设备列表。支持使用 for-in 循环，在循环中用 UsbDeviceWrapper 接收。
*/
class UsbDeviceListWrapper :public Wrapper<libusb_device *>
{
public:
	~UsbDeviceListWrapper()
	{
		FreeDeviceList();
	}

private:
	int64_t _count = 0;

	void FreeDeviceList()
	{
		libusb_free_device_list(_wrapped_obj, true);
		_wrapped_obj = nullptr;
	}

public:
	/**
	 * @brief 查找设备，将找到的设备储存到本对象中。可反复调用，多次查找。
	 * @param usb_ctx	查找设备依赖 LibusbContextWrapper。
	 * @return	找到的设备的数量。
	*/
	int64_t FindDevices(UsbContextWrapper &usb_ctx)
	{
		FreeDeviceList();
		_count = libusb_get_device_list(usb_ctx, &_wrapped_obj);
		return _count;
	}

	/**
	 * @brief 最后一次调用 FindDevices 所找到的设备的数量。
	 * @return
	*/
	int64_t Count() const
	{
		return _count;
	}

	/**
	 * @brief 获取列表中的指定设备
	 * @param index
	 * @return
	*/
	UsbDeviceWrapper operator[](int index)
	{
		return UsbDeviceWrapper{ _wrapped_obj[index] };
	}

	/**
	 * @brief 支持迭代。
	 * 可以在 for-in 循环中用 UsbDeviceWrapper 来接收，因为 UsbDeviceWrapper 有接受
	 * libusb_device *device 的构造函数，这会发生隐式的构造。
	 *
	 * @return
	*/
	libusb_device **begin()
	{
		return _wrapped_obj;
	}

	libusb_device **end()
	{
		return _wrapped_obj + _count;
	}

	/// <summary>
	///		过滤设备
	/// </summary>
	/// <param name="device_filter_func">
	///		过滤器。
	///		* 是一个回调函数。本方法遍历设备列表，获取一个设备的设备描述后，就会调用 device_filter_func，并传入设备描述。
	///		  device_filter_func 自己决定是否想要该设备。返回 true 表示想要，返回 false 表示不想要。
	/// </param>
	/// <returns></returns>
	vector<UsbDeviceWrapper> FilterDevices(function<bool(libusb_device_descriptor)> device_filter_func)
	{
		vector<UsbDeviceWrapper> device_vector;
		for (UsbDeviceWrapper device : *this)
		{
			auto desc = device.GetDescriptor();
			if (device_filter_func(desc))
			{
				// 通过过滤的设备才被放到向量中。
				device_vector.push_back(device);
			}
		}

		return device_vector;
	}
};