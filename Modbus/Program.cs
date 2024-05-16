using libsdv3.Modbus;
using System.IO.Ports;

SerialPort port = new("COM5")
{
	BaudRate = 115200,
	Parity = Parity.Even,
	StopBits = StopBits.One
};

port.Open();
ModbusSdv3Device sdv3 = new(port, 1);
Console.WriteLine(sdv3.EI9);
