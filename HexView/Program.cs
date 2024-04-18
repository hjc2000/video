using JCNET;
using System.CommandLine;

Option<string> input_file_path_option = new("-i", "选择输入文件");
Option<string?> output_file_path_option = new("-o",
	"要将结果输出到哪个文件。一般是指定一个 txt 文件。不指定则输出到控制台。");
Option<string?> start_position_option = new("-start", "从文件的哪里开始查看");
Option<string?> end_position_option = new("-end", "查看到文件的哪里");

RootCommand root_command = new("将文件的字节用 16 进制数显示");
root_command.AddOption(input_file_path_option);
root_command.AddOption(output_file_path_option);
root_command.AddOption(start_position_option);
root_command.AddOption(end_position_option);
root_command.SetHandler(
	HandleAsync,
	input_file_path_option,
	output_file_path_option,
	start_position_option,
	end_position_option
);
await root_command.InvokeAsync(args);

static async Task HandleAsync(string input_file_path, string? output_file_path,
	string? start, string? end)
{
	long start_pos = long.Parse(start ?? "0");
	long end_pos = long.Parse(end ?? "188");
	using FileStream input_file = File.Open(input_file_path,
		FileMode.Open, FileAccess.Read, FileShare.Read);
	using BinaryReader binary_reader = new(input_file);

	await using LogOutputPort port = new(output_file_path);
	input_file.Seek(start_pos, SeekOrigin.Begin);
	for (long i = 0; i < end_pos - start_pos; i++)
	{
		try
		{
			byte b = binary_reader.ReadByte();
			port.Write($"{b:x2}\t");
			if ((i + 1) % 10 == 0)
			{
				port.WriteLine();
			}
		}
		catch
		{
			break;
		}
	}
}
