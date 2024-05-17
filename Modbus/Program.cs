﻿using libsdv3.Modbus;
using System.IO.Ports;

SerialPort serial = new("COM5")
{
	BaudRate = 115200,
	Parity = Parity.Even,
	StopBits = StopBits.One
};

serial.Open();
ModbusSdv3Device sdv3 = new(serial, 1, true);
sdv3.EI9 = !sdv3.EI9;
Console.WriteLine(sdv3.EI9);
Console.WriteLine(sdv3.EI1);
Console.WriteLine(sdv3.EI3);
Console.WriteLine(sdv3.EOUT1);
Console.WriteLine(sdv3.EOUT3);
