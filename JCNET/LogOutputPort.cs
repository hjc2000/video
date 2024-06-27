namespace JCNET;

/// <summary>
///		日志输出端口
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
		_console_origin_writer = Console.Out;
		if (_output_file_path is not null)
		{
			_log_file = File.Open(_output_file_path, FileMode.Create,
				FileAccess.ReadWrite, FileShare.ReadWrite);
			_log_writer = new(_log_file)
			{
				AutoFlush = true
			};

			Console.SetOut(_log_writer);
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

		Console.SetOut(_console_origin_writer);
	}

	private string? _output_file_path;
	private FileStream? _log_file = null;
	private StreamWriter? _log_writer = null;
	private TextWriter _console_origin_writer;
}
