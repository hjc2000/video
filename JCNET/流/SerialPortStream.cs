﻿using System.IO.Ports;

namespace JCNET.流;

/// <summary>
///		为 SerialPort 实现流接口。
/// </summary>
public class SerialPortStream : Stream
{
	public SerialPortStream(SerialPort serial_port)
	{
		_serial_port = serial_port;
	}

	private SerialPort _serial_port;

	public override void Flush()
	{

	}

	public override int Read(byte[] buffer, int offset, int count)
	{
		return _serial_port.Read(buffer, offset, count);
	}

	public override long Seek(long offset, SeekOrigin origin)
	{
		throw new NotSupportedException();
	}

	public override void SetLength(long value)
	{
		throw new NotSupportedException();
	}

	public override void Write(byte[] buffer, int offset, int count)
	{
		_serial_port.Write(buffer, offset, count);
	}

	public override bool CanRead
	{
		get
		{
			return true;
		}
	}
	public override bool CanSeek
	{
		get
		{
			return false;
		}
	}
	public override bool CanWrite
	{
		get
		{
			return true;
		}
	}
	public override long Length
	{
		get
		{
			throw new NotSupportedException();
		}
	}
	public override long Position
	{
		get
		{
			throw new NotSupportedException();
		}
		set
		{
			throw new NotSupportedException();
		}
	}
}
