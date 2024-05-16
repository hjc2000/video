using System.IO.Ports;

SerialPort port = new("COM5")
{
	BaudRate = 115200,
	Parity = Parity.Even,
	StopBits = StopBits.One
};

port.Open();
byte[] start_cmd = [0x01, 0x05, 0x02, 0x08, 0xFF, 0x00, 0x0C, 0x40, 0x00, 0x00];
byte[] stop_cmd = [0x01, 0x05, 0x02, 0x08, 0x00, 0x00, 0x0C, 0x40, 0x30, 0x14];
while (true)
{
	port.Write(start_cmd, 0, start_cmd.Length);
	await Task.Delay(5000);
	port.Write(stop_cmd, 0, stop_cmd.Length);
	await Task.Delay(5000);
}
