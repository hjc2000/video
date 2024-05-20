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
Console.WriteLine($"0x{sdv3.CurrentAlarm:x2}");
for (int i = 1; i <= 20; i++)
{
	Console.WriteLine($"0x{sdv3.AlarmRecords(i):x2}");
}
