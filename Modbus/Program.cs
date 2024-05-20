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

Console.WriteLine("pn 参数");
Console.WriteLine(sdv3.Pn(1, 6));
Console.WriteLine(sdv3.Pn(1, 7));
Console.WriteLine();

Console.WriteLine("报警记录：");
Console.WriteLine($"0x{sdv3.CurrentAlarm:x2}");
for (int i = 1; i <= sdv3.AlarmRecordCount; i++)
{
	Console.WriteLine($"0x{sdv3.AlarmRecords(i):x2}");
}

Console.WriteLine();

Console.WriteLine("反馈当前位置：");
Console.WriteLine(sdv3.FeedbackCurrentPosition);
Console.WriteLine();