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
			_output_file = File.Open(_output_file_path, FileMode.Create);
			_output_writer = new(_output_file);
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

		if (_output_writer is not null)
		{
			await _output_writer.DisposeAsync();
		}

		if (_output_file is not null)
		{
			await _output_file.DisposeAsync();
		}
	}

	private string? _output_file_path;
	private FileStream? _output_file = null;
	private StreamWriter? _output_writer = null;

	public void Write(string value)
	{
		if (_output_writer is not null)
		{
			_output_writer.Write(value);
		}
		else
		{
			Console.Write(value);
		}
	}

	public void WriteLine(string value)
	{
		if (_output_writer is not null)
		{
			_output_writer.WriteLine(value);
		}
		else
		{
			Console.WriteLine(value);
		}
	}

	public void WriteLine()
	{
		if (_output_writer is not null)
		{
			_output_writer.WriteLine();
		}
		else
		{
			Console.WriteLine();
		}
	}
}
