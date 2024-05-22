namespace JCNET;

/// <summary>
///		日志输出端口。可以输出到文件或控制台。指定文件路径则输出到文件，文件路径为空则输出到控制台。
/// </summary>
public class LogOutputPort : IAsyncDisposable
{
	/// <summary>
	///		传入 null 则使用控制台作为输出，否则使用文件作为输出。
	/// </summary>
	/// <param name="output_file_path"></param>
	public LogOutputPort(string? output_file_path)
	{
		_output_file_path = output_file_path;
		if (_output_file_path is not null)
		{
			_log_file = File.Open(_output_file_path, FileMode.Create, FileAccess.ReadWrite, FileShare.ReadWrite);
			_log_writer = new(_log_file)
			{
				AutoFlush = true
			};
		}
	}

	private bool _disposed = false;
	public async ValueTask DisposeAsync()
	{
		if (_disposed)
		{
			return;
		}

		_disposed = true;
		GC.SuppressFinalize(this);

		if (_log_writer is not null)
		{
			await _log_writer.DisposeAsync();
		}

		if (_log_file is not null)
		{
			await _log_file.DisposeAsync();
		}
	}

	private string? _output_file_path;
	private FileStream? _log_file = null;
	private StreamWriter? _log_writer = null;

	public void Write<T>(T value)
	{
		lock (this)
		{
			if (_log_writer is not null)
			{
				_log_writer.Write(value);
			}
			else
			{
				Console.Write(value);
			}
		}
	}

	public void WriteLine<T>(T value)
	{
		lock (this)
		{
			if (_log_writer is not null)
			{
				_log_writer.WriteLine(value);
			}
			else
			{
				Console.WriteLine(value);
			}
		}
	}

	public void WriteLine()
	{
		lock (this)
		{
			if (_log_writer is not null)
			{
				_log_writer.WriteLine();
			}
			else
			{
				Console.WriteLine();
			}
		}
	}
}
