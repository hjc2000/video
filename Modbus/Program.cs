using System.IO.Ports;

string[] port_names = SerialPort.GetPortNames();
foreach (string port_name in port_names)
{
	Console.WriteLine(port_name);
}
