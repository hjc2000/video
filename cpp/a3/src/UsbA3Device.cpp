#include"UsbA3Device.h"
#include<functional>
#include<memory>
#include<stdio.h>
#include<vector>

using namespace std;
using namespace libusb;
using std::shared_ptr;
using std::unique_ptr;

usbdevice_id *usb_ll_list_get_id(uint16_t vid, uint16_t pid)
{
	int32_t pos = 0;

	while (usb_device_ids[pos].type != usb_type_unknown)
	{
		if (usb_device_ids[pos].vid == vid && usb_device_ids[pos].pid == pid)
			return (usbdevice_id *)&usb_device_ids[pos];
		pos++;
	}

	return NULL;
}

vector<shared_ptr<UsbA3Device>> UsbA3Device::FindDevices(usbdevice_type type, UsbDeviceListWrapper &usb_list)
{
	vector<shared_ptr<UsbA3Device>> device_vector;
	int32_t i = 0;

	for (UsbDeviceWrapper device : usb_list)
	{
		try
		{
			device.Open();
		}
		catch (...)
		{
			continue;
		}

		device.ClaimInterface();
		auto desc = device.GetDescriptor();
		usbdevice_type devtype = usb_type_unknown;
		auto check_func = [&](usbdevice_type checkparam)->int32_t
		{
			usbdevice_id *puid = usb_ll_list_get_id(desc.idVendor, desc.idProduct);
			if (puid && (puid->type == checkparam || checkparam == usb_type_all))
			{
				devtype = puid->type;
				return 1;
			}

			return 0;
		};

		if (check_func(type))
		{
			shared_ptr<UsbA3Device> newdevice{ new UsbA3Device{} };
			newdevice->_usb_device = device;
			//WINUSB_PIPE_INFORMATION  Pipe{};
			//WinUsb_QueryPipe(
			//	vk_device_data->_winusb_handle,
			//	0,
			//	USBDEV_BULK_WRITE_EP,
			//	&Pipe
			//);

			//newdevice->_husb = vk_device_data;

			newdevice->_is_dma = 0;
			//newdevice->_bulksize = Pipe.MaximumPacketSize;
			device_vector.push_back(newdevice);
		}
	}

	return device_vector;
}
