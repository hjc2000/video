using JCNET;
using JCNET.Modbus;
using JCNET.流;
using libsdv3.Modbus.Frame;
using System.IO.Ports;

namespace libsdv3.Modbus;

public class SerialPortModbusSdv3Device : ModbusSdv3Device
{
	public SerialPortModbusSdv3Device(SerialPort serialPort, byte device_addr, bool big_endian)
	{
		_serial_port = serialPort;
		_device_addr = device_addr;
		_big_endian = big_endian;
		_sdv3 = new InnerSerialPortModbusSdv3Device(
			new SerialPortStream(serialPort),
			device_addr,
			big_endian
		);
	}

	private bool _disposed = false;
	public override async ValueTask DisposeAsync()
	{
		await ((IAsyncDisposable)_sdv3).DisposeAsync();
		if (_disposed)
		{
			return;
		}

		_disposed = true;
		GC.SuppressFinalize(this);

		await _sdv3.DisposeAsync();
		_serial_port.Dispose();
	}

	private SerialPort _serial_port;
	private byte _device_addr;
	private bool _big_endian;
	private InnerSerialPortModbusSdv3Device _sdv3;
	private SemaphoreSlim _async_lock = new(1);

	public int RetryTimes { get; set; } = 5;

	/// <summary>
	///		重新构造一个 SerialPort 对象，复制旧的 SerialPort 对象的所有属性，
	///		然后打开，然后赋值给 _serial_port。
	/// </summary>
	private async Task ReopenPortAsync()
	{
		ObjectDisposedException.ThrowIf(_disposed, typeof(SerialPortModbusSdv3Device));
		Console.WriteLine("重新打开串口");
		try
		{
			SerialPort new_port = new()
			{
				PortName = _serial_port.PortName,
				BaudRate = _serial_port.BaudRate,
				Parity = _serial_port.Parity,
				StopBits = _serial_port.StopBits,
				ReadTimeout = _serial_port.ReadTimeout,
				WriteTimeout = _serial_port.WriteTimeout,
			};
			await _sdv3.DisposeAsync();
			_serial_port.Dispose();

			_serial_port = new_port;
			_sdv3 = new InnerSerialPortModbusSdv3Device(
				new SerialPortStream(_serial_port),
				_device_addr,
				_big_endian
			);
		}
		catch (Exception ex)
		{
			Console.WriteLine(ex.ToString());
		}
	}

	public override async Task WriteSingleBitAsync(ushort data_addr, bool value)
	{
		using LockGuard l = new(_async_lock);
		await l.WaitAsync();

		int retry_times = 0;
		while (true)
		{
			try
			{
				await _sdv3.WriteSingleBitAsync(data_addr, value);
				return;
			}
			catch (ModbusFrameException)
			{
				_serial_port.DiscardInBuffer();
				_serial_port.DiscardOutBuffer();
				if (retry_times < RetryTimes)
				{
					retry_times++;
					continue;
				}

				throw;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex);
				await Task.Delay(1000);
				await ReopenPortAsync();
			}
		}
	}

	public override async Task<byte[]> ReadBitsAsync(ushort data_addr, ushort bit_count)
	{
		using LockGuard l = new(_async_lock);
		await l.WaitAsync();

		int retry_times = 0;
		while (true)
		{
			try
			{
				return await _sdv3.ReadBitsAsync(data_addr, bit_count);
			}
			catch (ModbusFrameException)
			{
				_serial_port.DiscardInBuffer();
				_serial_port.DiscardOutBuffer();
				if (retry_times < RetryTimes)
				{
					retry_times++;
					continue;
				}

				throw;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex);
				await Task.Delay(1000);
				await ReopenPortAsync();
			}
		}
	}

	public override async Task<uint[]> ReadDatasAsync(ushort data_addr, ushort record_count)
	{
		using LockGuard l = new(_async_lock);
		await l.WaitAsync();

		int retry_times = 0;
		while (true)
		{
			try
			{
				return await _sdv3.ReadDatasAsync(data_addr, record_count);
			}
			catch (ModbusFrameException)
			{
				_serial_port.DiscardInBuffer();
				_serial_port.DiscardOutBuffer();
				if (retry_times < RetryTimes)
				{
					retry_times++;
					continue;
				}

				throw;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex);
				await Task.Delay(1000);
				await ReopenPortAsync();
			}
		}
	}

	public override async Task WriteDatasAsync(ushort data_addr, uint[] datas)
	{
		using LockGuard l = new(_async_lock);
		await l.WaitAsync();

		int retry_times = 0;
		while (true)
		{
			try
			{
				await _sdv3.WriteDatasAsync(data_addr, datas);
				return;
			}
			catch (ModbusFrameException)
			{
				_serial_port.DiscardInBuffer();
				_serial_port.DiscardOutBuffer();
				if (retry_times < RetryTimes)
				{
					retry_times++;
					continue;
				}

				throw;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex);
				await Task.Delay(1000);
				await ReopenPortAsync();
			}
		}
	}
}

/// <summary>
///		内部使用
/// </summary>
internal class InnerSerialPortModbusSdv3Device : ModbusSdv3Device
{
	public InnerSerialPortModbusSdv3Device(Stream serial_stream, byte device_addr, bool big_endian)
	{
		_serial_stream = serial_stream;
		_device_addr = device_addr;
		_big_endian = big_endian;
		_auto_bit_converter = new AutoBitConverter(big_endian);
	}

	private bool _disposed = false;
	public override async ValueTask DisposeAsync()
	{
		if (_disposed)
		{
			return;
		}

		_disposed = true;
		GC.SuppressFinalize(this);

		await _serial_stream.DisposeAsync();
	}

	private byte _device_addr;
	private Stream _serial_stream;
	private bool _big_endian;
	private AutoBitConverter _auto_bit_converter;

	private static void PrintFrame(byte[] frame, bool is_send)
	{
		if (is_send)
		{
			Console.Write("发送：");
		}
		else
		{
			Console.Write("接收：");
		}

		foreach (byte b in frame)
		{
			Console.Write($"{b:x2} ");
		}

		Console.WriteLine();
	}

	#region modbus 基本操作函数
	/// <summary>
	///		检查 ADU。
	///		* 不包括 PDU 部分，只检查作为头部的地址和作为尾部的 CRC16。
	/// </summary>
	/// <param name="response_frame"></param>
	/// <exception cref="ModbusFrameException"></exception>
	private void CheckADU(Memory<byte> response_frame)
	{
		ModbusCrc16 crc16 = new();
		crc16.Add(response_frame[..^2]);

		Span<byte> span = response_frame.Span;
		if (span[^2] != crc16.RegisterLowByte)
		{
			throw new ModbusFrameException("CRC 校验错误");
		}

		if (span[^1] != crc16.RegisterHighByte)
		{
			throw new ModbusFrameException("CRC 校验错误");
		}

		if (span[0] != _device_addr)
		{
			throw new ModbusFrameException("接收到非预期地址的设备的响应");
		}
	}

	/// <summary>
	///		写单个位
	/// </summary>
	/// <param name="data_addr"></param>
	/// <param name="value"></param>
	/// <exception cref="ModbusFrameException"></exception>
	public override async Task WriteSingleBitAsync(ushort data_addr, bool value)
	{
		WriteSingleBitRequestFrame request_frame = new()
		{
			SlaveAddress = _device_addr,
			DataAddress = data_addr,
			BitValue = value,
		};
		byte[] frame = request_frame.ToBytes(_big_endian);
		PrintFrame(frame, true);
		CancellationTokenSource cancel_write = new();
		cancel_write.CancelAfter(2000);
		await _serial_stream.WriteAsync(frame, cancel_write.Token);

		// 接收
		byte[] read_buffer = new byte[8];
		CancellationTokenSource cancel_read = new();
		cancel_read.CancelAfter(2000);
		await _serial_stream.ReadExactlyAsync(read_buffer, cancel_read.Token);
		PrintFrame(read_buffer, false);
		CheckADU(read_buffer);
		if (read_buffer[1] != (byte)FunctionCode.WriteSingleBit)
		{
			throw new ModbusFrameException("设备回复的帧中的功能码错误");
		}

		ushort received_data_addr = _auto_bit_converter.ToUInt16(read_buffer, 2);
		if (received_data_addr != data_addr)
		{
			throw new ModbusFrameException("设备回复帧中的数据地址不对");
		}

		ushort received_data = _auto_bit_converter.ToUInt16(read_buffer, 4);
		if (received_data != 0 != value)
		{
			throw new ModbusFrameException("设备回复帧中的数据不对");
		}
	}

	/// <summary>
	///		读取多个位数据。
	/// </summary>
	/// <param name="data_addr">数据地址</param>
	/// <param name="bit_count">要读取多少个位</param>
	/// <returns></returns>
	public override async Task<byte[]> ReadBitsAsync(ushort data_addr, ushort bit_count)
	{
		if (bit_count == 0)
		{
			throw new ArgumentException("不允许读取 0 个位");
		}

		ReadBitsRequestFrame request_frame = new()
		{
			SlaveAddress = _device_addr,
			DataAddress = data_addr,
			BitCount = bit_count,
		};
		byte[] frame = request_frame.ToBytes(_big_endian);
		PrintFrame(frame, true);
		CancellationTokenSource cancel_write = new();
		cancel_write.CancelAfter(2000);
		await _serial_stream.WriteAsync(frame, cancel_write.Token);

		byte[] read_buffer = new byte[5 + (bit_count / 8) + 1];
		CancellationTokenSource cancel_read = new();
		cancel_read.CancelAfter(2000);
		await _serial_stream.ReadExactlyAsync(read_buffer, cancel_read.Token);
		PrintFrame(read_buffer, false);
		CheckADU(read_buffer);
		if (read_buffer[1] != (byte)FunctionCode.ReadBits)
		{
			throw new ModbusFrameException("设备回复的帧中的功能码错误");
		}

		return read_buffer[3..^2];
	}

	/// <summary>
	///		读取各种数据
	/// </summary>
	/// <param name="data_addr">数据地址</param>
	/// <param name="record_count">记录数。一个记录是 16 位。读取 1 个 32 位的数据需要 2 个记录。</param>
	/// <returns></returns>
	public override async Task<uint[]> ReadDatasAsync(ushort data_addr, ushort record_count)
	{
		if (record_count == 0)
		{
			throw new ArgumentException($"{nameof(record_count)} 不能为 0");
		}

		ReadDatasRequestFrame request_frame = new()
		{
			SlaveAddress = _device_addr,
			DataAddress = data_addr,
			RecordCount = record_count,
		};
		byte[] frame = request_frame.ToBytes(_big_endian);
		PrintFrame(frame, true);
		CancellationTokenSource cancel_write = new();
		cancel_write.CancelAfter(2000);
		await _serial_stream.WriteAsync(frame, cancel_write.Token);

		// 接收响应
		byte[] read_buffer = new byte[5 + (record_count * 2)];
		CancellationTokenSource cancel_read = new();
		cancel_read.CancelAfter(2000);
		await _serial_stream.ReadExactlyAsync(read_buffer, cancel_read.Token);
		PrintFrame(read_buffer, false);
		CheckADU(read_buffer);
		if (read_buffer[1] != (byte)FunctionCode.ReadDatas)
		{
			throw new ModbusFrameException("设备回复的帧中的功能码错误");
		}

		if (read_buffer[2] != record_count * 2)
		{
			throw new ModbusFrameException("返回的数据字节数不对");
		}

		int response_uint32_data_count = read_buffer[2] / 4;
		uint[] ret = new uint[response_uint32_data_count];
		for (int i = 0; i < response_uint32_data_count; i++)
		{
			int start_pos = 3 + (i * 4);
			ret[i] = _auto_bit_converter.ToUInt32(read_buffer, start_pos);
		}

		return ret;
	}

	public override async Task WriteDatasAsync(ushort data_addr, uint[] datas)
	{
		if (datas.Length == 0)
		{
			throw new ArgumentException($"{nameof(datas)} 的长度不能为 0");
		}

		WriteDatasRequestFrame request_frame = new()
		{
			SlaveAddress = _device_addr,
			DataAddress = data_addr,
			Datas = datas,
		};
		byte[] frame = request_frame.ToBytes(_big_endian);
		PrintFrame(frame, true);
		CancellationTokenSource cancel_write = new();
		cancel_write.CancelAfter(2000);
		await _serial_stream.WriteAsync(frame, cancel_write.Token);

		// 接收响应
		byte[] read_buffer = new byte[8];
		CancellationTokenSource cancel_read = new();
		cancel_read.CancelAfter(2000);
		await _serial_stream.ReadExactlyAsync(read_buffer, cancel_read.Token);
		PrintFrame(read_buffer, false);
		CheckADU(read_buffer);
		if (read_buffer[1] != (byte)FunctionCode.WriteDatas)
		{
			throw new ModbusFrameException("设备回复的帧中的功能码错误");
		}

		ushort response_data_addr = _auto_bit_converter.ToUInt16(read_buffer, 2);
		if (response_data_addr != data_addr)
		{
			throw new ModbusFrameException("设备回复帧中的数据地址不对");
		}

		ushort response_record_count = _auto_bit_converter.ToUInt16(read_buffer, 4);
		if (response_record_count != datas.Length * 2)
		{
			throw new ModbusFrameException("设备回复帧中的记录数不对");
		}
	}
	#endregion
}
