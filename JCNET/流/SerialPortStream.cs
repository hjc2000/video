using System.IO.Ports;

namespace JCNET.流;

/// <summary>
///		为 SerialPort 实现流接口。
///		* 这里的流操作方法发生异常时会尝试重新打开串口。
/// </summary>
#pragma warning disable CA1844 // 在子类化 “Stream” 时提供异步方法的基于内存的重写
public class SerialPortStream : Stream
#pragma warning restore CA1844 // 在子类化 “Stream” 时提供异步方法的基于内存的重写
{
	public SerialPortStream(SerialPort serial_port)
	{
		_serial_port = serial_port;
	}

	protected override void Dispose(bool disposing)
	{
		base.Dispose(disposing);
		if (disposing)
		{
			_serial_port.Dispose();
		}
	}

	/// <summary>
	///		重新构造一个 SerialPort 对象，复制旧的 SerialPort 对象的所有属性，
	///		然后打开，然后赋值给 _serial_port。
	/// </summary>
	private void ReopenPort()
	{
		try
		{
			_serial_port.Dispose();
			SerialPort new_port = new()
			{
				BaudRate = _serial_port.BaudRate,
				Parity = _serial_port.Parity,
				StopBits = _serial_port.StopBits,
				PortName = _serial_port.PortName,
				ReadTimeout = _serial_port.ReadTimeout,
				WriteTimeout = _serial_port.WriteTimeout,
				BreakState = _serial_port.BreakState,
				DataBits = _serial_port.DataBits,
				DiscardNull = _serial_port.DiscardNull,
				DtrEnable = _serial_port.DtrEnable,
				Encoding = _serial_port.Encoding,
				Handshake = _serial_port.Handshake,
				NewLine = _serial_port.NewLine,
				ParityReplace = _serial_port.ParityReplace,
				ReadBufferSize = _serial_port.ReadBufferSize,
				WriteBufferSize = _serial_port.WriteBufferSize,
				ReceivedBytesThreshold = _serial_port.ReceivedBytesThreshold,
				RtsEnable = _serial_port.RtsEnable,
				Site = _serial_port.Site,
			};
			new_port.Open();
			_serial_port = new_port;
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	/// <summary>
	///		清空输入输出缓冲区中的所有内容
	/// </summary>
	/// <param name="cancellationToken"></param>
	/// <returns></returns>
	public async Task CleanInOutBufferAsync(CancellationToken cancellationToken)
	{
		while (!cancellationToken.IsCancellationRequested)
		{
			try
			{
				_serial_port.DiscardInBuffer();
				_serial_port.DiscardOutBuffer();
				return;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.ToString());
				await Task.Delay(1000, cancellationToken);
				ReopenPort();
			}
		}

		throw new OperationCanceledException();
	}

	private SerialPort _serial_port;

	public override void Flush()
	{

	}

	public override int Read(byte[] buffer, int offset, int count)
	{
		throw new NotSupportedException("不支持同步读写");
	}

	public override async Task<int> ReadAsync(byte[] buffer, int offset, int count, CancellationToken cancellationToken)
	{
		while (!cancellationToken.IsCancellationRequested)
		{
			try
			{
				int have_read = _serial_port.Read(buffer, offset, count);
				return have_read;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.ToString());
				await Task.Delay(1000, cancellationToken);
				ReopenPort();
			}
		}

		throw new OperationCanceledException();
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
		throw new NotSupportedException("不支持同步读写");
	}

	public override async Task WriteAsync(byte[] buffer, int offset, int count, CancellationToken cancellationToken)
	{
		while (!cancellationToken.IsCancellationRequested)
		{
			try
			{
				_serial_port.Write(buffer, offset, count);
				return;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.ToString());
				await Task.Delay(1000, cancellationToken);
				ReopenPort();
			}
		}

		throw new OperationCanceledException();
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
