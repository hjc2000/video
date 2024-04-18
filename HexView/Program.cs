using System.CommandLine;

Option<string> file_info_option = new("-i", "选择输入文件");
Option<string> start_position_option = new("-start", "从文件的哪里开始查看");
Option<string> end_position_option = new("-end", "查看到文件的哪里");

RootCommand root_command = new("将文件的字节用 16 进制数显示");
root_command.AddOption(file_info_option);
root_command.AddOption(start_position_option);
root_command.AddOption(end_position_option);
root_command.SetHandler(
	HandleAsync,
	file_info_option,
	start_position_option,
	end_position_option
);
await root_command.InvokeAsync(args);

static async Task HandleAsync(string file_path, string start, string end)
{
	long start_pos = long.Parse(start);
	long end_pos = long.Parse(end);
	using FileStream fs = File.Open(file_path, FileMode.Open);
	using BinaryReader binaryReader = new(fs);
	using FileStream outputTxt = File.Open("hex.txt", FileMode.Create);
	using StreamWriter streamWriter = new(outputTxt);

	fs.Seek(start_pos, SeekOrigin.Begin);
	for (long i = 0; i < end_pos - start_pos; i++)
	{
		try
		{
			if (i % 188 == 0)
			{
				streamWriter.WriteLine($"第{(i / 188) + 1}个包");
			}

			byte b = binaryReader.ReadByte();
			streamWriter.Write($"{b:x2}\t");
			if ((i + 1) % 4 == 0)
			{
				streamWriter.WriteLine();
			}

			if ((i + 1) % 188 == 0)
			{
				streamWriter.WriteLine();
			}
		}
		catch
		{
			break;
		}
	}

	await streamWriter.FlushAsync();
	Console.WriteLine("完成");
}
