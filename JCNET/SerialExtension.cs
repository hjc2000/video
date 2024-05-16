using System.IO.Ports;

namespace JCNET;

public static class SerialExtension
{
	/// <summary>
	///		从串口中精确地读出 count 个字节后才会返回。除非发生异常。
	/// </summary>
	/// <param name="serial"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	public static byte[] ReadExactly(this SerialPort serial, int count)
	{
		byte[] read_buffer = new byte[count];
		int total_read = 0;
		while (total_read < count)
		{
			int have_read = serial.Read(read_buffer, total_read, count - total_read);
			total_read += have_read;
		}

		return read_buffer;
	}

	public static void Write(this SerialPort serial, byte[] buffer)
	{
		serial.Write(buffer, 0, buffer.Length);
	}
}
