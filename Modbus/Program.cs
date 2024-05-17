using libsdv3.Modbus;
using System.IO.Ports;

SerialPort serial = new("COM5")
{
	BaudRate = 115200,
	Parity = Parity.Even,
	StopBits = StopBits.One
};

serial.Open();
ModbusSdv3Device sdv3 = new(serial, 1);
Console.WriteLine(sdv3.EI9);
sdv3.EI9 = false;
Console.WriteLine(sdv3.EI9);
