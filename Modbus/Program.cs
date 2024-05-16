using System.IO.Ports;

SerialPort port = new("COM5")
{
	BaudRate = 115200,
	Parity = Parity.Even,
	StopBits = StopBits.One
};

port.Open();
byte[] data = [0x01, 0x05, 0x02, 0x08, 0x00, 0x00, 0x0C, 0x40, 0x30, 0x14];
port.Write(data, 0, data.Length);
