using JCNET.流;
using libsdv3.Modbus;
using System.IO.Ports;

SerialPort serial = new("COM5")
{
	BaudRate = 115200,
	Parity = Parity.Even,
	StopBits = StopBits.One
};

serial.Open();
ModbusSdv3Device sdv3 = new(new SerialPortStream(serial), 1, true);
Console.WriteLine(sdv3.Pn(1, 6));
Console.WriteLine(sdv3.Pn(1, 7));
