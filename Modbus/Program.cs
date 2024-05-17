using libsdv3.Modbus;
using System.IO.Ports;

SerialPort serial = new("COM5")
{
	BaudRate = 115200,
	Parity = Parity.Even,
	StopBits = StopBits.One
};

serial.Open();
ModbusSdv3Device sdv3 = new(serial, 1)
{
	EI9 = true
};
Console.WriteLine(sdv3.EI9);
