#pragma once
#include<A3Device.h>
#include<A3Excepion.h>
#include<jccpp/EndianConverter.h>
#include<libusb-wrapper/UsbDeviceListWrapper.h>
#include<libusb-wrapper/UsbDeviceWrapper.h>
#include<mutex>
#include<vatek_base.h>

using std::mutex;
using std::unique_lock;
using std::lock_guard;
using namespace jc;

static const usbdevice_id usb_device_ids[] = {
	{ usb_type_broadcast, 0x2C42, 0x2011, },
	{ usb_type_broadcast, 0x2C42, 0x2021, },
	{ usb_type_broadcast, 0x2C42, 0x1011, },
	{ usb_type_broadcast, 0x2C42, 0x1031, },// A3 找到的是这个
	{ usb_type_broadcast, 0x2C42, 0x2031, },
	{ usb_type_rescure, 0x2C42, 0x1010, },
	{ usb_type_rescure, 0x2C42, 0x1030, },
	{ usb_type_rescure, 0x2C42, 0x2010, },
	{ usb_type_rescure, 0x2C42, 0x2030, },
	{ usb_type_unknown, 0, 0, },
};

class UsbA3Device :public A3Device
{
public:
	/// <summary>
	///		查找 USB 设备
	/// </summary>
	/// <param name="type"></param>
	/// <param name="hlist"></param>
	/// <returns>找到的设备的数量</returns>
	static vector<shared_ptr<UsbA3Device>> FindDevices(usbdevice_type type, libusb::UsbDeviceListWrapper &usb_list);

private:
	UsbA3Device() {}

private:
	/// <summary>
	///		读写 A3 的内存时需要先告诉 A3 要读写的地址。
	/// </summary>
	/// <param name="address"></param>
	void SetAddress(uint32_t address)
	{
		ControlTransferOutput_WithoutPayloadBuffer(0x4A, address, true);
	}

	std::recursive_mutex _lock;

public:
	char _name[32]{};
	libusb::UsbDeviceWrapper _usb_device{};
	int32_t _ref = 0;
	int32_t _is_dma = 0;
	int32_t _epsize = 0;

public:// 方法
	#pragma region 继承自 A3Device
	uint32_t ReadRegister(uint32_t addr) override
	{
		lock_guard l(_lock);
		SetAddress(addr);

		uint32_t buf[2];
		ControlTransferInput(0x4C, 0, (uint8_t *)&buf[0], true);
		buf[0] = vatek_buffer_2_uint32((uint8_t *)&buf[0]);
		if (buf[0] != addr)
		{
			throw A3::ReadMemoryException();
		}

		return vatek_buffer_2_uint32((uint8_t *)&buf[1]);
	}

	/// <summary>
	///		写寄存器。失败会抛出异常。
	/// </summary>
	/// <param name="addr"></param>
	/// <param name="val"></param>
	void WriteRegister(int32_t addr, uint32_t val) override
	{
		lock_guard l(_lock);
		try
		{
			SetAddress(addr);
			ControlTransferOutput_WithoutPayloadBuffer(0x4D, val, true);
		}
		catch (...)
		{
			throw A3::WriteMemoryException();
		}
	}

	/// <summary>
	///		读取内存。失败会抛出异常。
	/// </summary>
	/// <param name="addr"></param>
	/// <param name="val"></param>
	/// <returns></returns>
	uint32_t ReadMemory(int32_t addr) override
	{
		uint32_t buf[2];
		lock_guard l(_lock);

		SetAddress(addr);
		ControlTransferInput(0x4E, 0, (uint8_t *)&buf[0], true);

		buf[0] = vatek_buffer_2_uint32((uint8_t *)&buf[0]);
		if (buf[0] != addr)
		{
			throw A3::ReadMemoryException();
		}

		return vatek_buffer_2_uint32((uint8_t *)&buf[1]);
	}

	/// <summary>
	///		失败会抛出异常。
	/// </summary>
	/// <param name="addr"></param>
	/// <param name="val"></param>
	/// <returns></returns>
	void WriteMemory(int32_t addr, uint32_t val) override
	{
		lock_guard l(_lock);
		try
		{
			SetAddress(addr);
			ControlTransferOutput_WithoutPayloadBuffer(0x4F, val, true);
		}
		catch (...)
		{
			throw A3::WriteMemoryException();
		}
	}

	const char *DeviceName() override
	{
		return _name;
	}

	void WriteBlock(uint8_t *pbuf, int32_t len) override
	{
		if (!pbuf || len == 0)
		{
			return;
		}

		if (len > CHIP_STREAM_SLICE_LEN || (len % 64) != 0)
		{
			throw jc::invalid_argument{ "非法参数" };
		}

		vk_result nres = vatek_badparam;
		if (!_is_dma)
		{
			EndianConverter::ToBigEndian(pbuf, len, 4);
		}

		int rlen = _usb_device.BulkTransfer(USBDEV_BULK_WRITE_EP, pbuf, len, 0);
		if (rlen != len)
		{
			throw A3::UsbException();
		}
	}

	vk_result ReadBlock(uint8_t *pbuf, int32_t len) override
	{
		vk_result nres = vatek_success;
		if (len > CHIP_STREAM_SLICE_LEN || (len % 64) != 0)
		{
			throw jc::invalid_argument{ "非法参数" };
		}

		return (vk_result)_usb_device.BulkTransfer(USBDEV_BULK_READ_EP, pbuf, len, 0);
	}

	/// <summary>
	///		开启或关闭 DMA。
	/// </summary>
	/// <param name="enable">true 开启 DMA，false 关闭 DMA</param>
	/// <returns></returns>
	void EnableDMA(bool enable) override
	{
		if (enable)
			ControlTransferOutput_WithoutPayloadBuffer(0x41, 0x0001, true);
		else
			ControlTransferOutput_WithoutPayloadBuffer(0x41, 0x0000, true);

		_is_dma = enable;
	}
	#pragma endregion

	/// <summary>
	///		用控制传输读取一个小包。包的长度是 8 字节。
	///		如果读取到的数据不等于 8 字节会抛出异常。
	/// </summary>
	/// <param name="cmd"></param>
	/// <param name="param0"></param>
	/// <param name="rxbuf">读取的 8 个字节会写入这里。</param>
	/// <returns></returns>
	void ControlTransferInput(uint8_t cmd, uint32_t param0, uint8_t *rxbuf, bool change_param_to_big_endian)
	{
		uint16_t wval;
		uint16_t widx;
		if (change_param_to_big_endian)
		{
			// 按大端序放置
			wval = ((param0 >> 16) << 8) | ((param0 >> 24) & 0xFF);
			widx = ((param0 & 0xFF) << 8) | ((param0 >> 8) & 0xFF);
		}
		else
		{
			// 按小端序放置
			uint8_t *pbuf = (uint8_t *)&param0;
			wval = (pbuf[1] << 8) | pbuf[0];
			widx = (pbuf[3] << 8) | pbuf[2];
		}

		uint32_t length_read = _usb_device.ControlTransfer(
			libusb::USBRequestOptions{
				libusb::USBRequestOptions::DataDirection::DeviceToHost,
				libusb::USBRequestOptions::RequestType::Vendor,
				libusb::USBRequestOptions::RecipientType::Device,
			},
			cmd,
			wval,
			widx,
			rxbuf,
			8,
			0
			);

		if (length_read != 8)
		{
			throw jc::Exception();
		}
	}

	void ControlTransferOutput_WithoutPayloadBuffer(uint8_t cmd, uint32_t param0, bool change_param_to_big_endian)
	{
		uint16_t wval;
		uint16_t widx;
		if (change_param_to_big_endian)
		{
			// 按大端序放置
			wval = ((param0 >> 16) << 8) | ((param0 >> 24) & 0xFF);
			widx = ((param0 & 0xFF) << 8) | ((param0 >> 8) & 0xFF);
		}
		else
		{
			// 按小端序放置
			uint8_t *pbuf = (uint8_t *)&param0;
			wval = (pbuf[1] << 8) | pbuf[0];
			widx = (pbuf[3] << 8) | pbuf[2];
		}

		/* 台湾方面使用了厂商请求，即自定义请求 USBRequestOptions::RequestType::Vendor。然后请求命令使用了自定义的。
		 * 请求命令就是形参 cmd。cmd 将会被传入 0x4c, 0x41 之类的。这些是非标准的命令代码，是厂商自定义的。
		 *
		 * 使用自定义命令必须将请求类型设置为 USBRequestOptions::RequestType::Vendor。
		 */
		uint32_t length_read = _usb_device.ControlTransfer(
			libusb::USBRequestOptions{
				libusb::USBRequestOptions::DataDirection::HostToDevice,
				libusb::USBRequestOptions::RequestType::Vendor,
				libusb::USBRequestOptions::RecipientType::Device,
			},
			cmd,
			wval,
			widx,
			nullptr,
			0,
			0
			);
	}
};








