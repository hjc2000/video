using libsdv3.Modbus;
using System.IO.Ports;

namespace Sdv3ControlPanel.Data;

public class Database
{
	public static SerialPort? SerialPort { get; set; }
	public static ModbusSdv3Device? SDV3 { get; set; }
}
