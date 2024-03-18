using JCNET.Parse;
using System.Text;

if (args.Length == 0)
{
	throw new ArgumentNullException(nameof(args), "请输入要修复的文件的绝对路径");
}

string pc_file_full_path = args[0];
using FileStream fs = File.Open(pc_file_full_path, FileMode.Open, FileAccess.ReadWrite, FileShare.Read);
using StreamReader stream_reader = new(fs);
StringBuilder out_string_builder = new();
string[] matched_prefixs = ["prefix", "exec_prefix", "libdir", "includedir", "sharedlibdir"];
while (true)
{
	string? line = await stream_reader.ReadLineAsync();
	if (line is null)
	{
		break;
	}

	line = line.Trim();
	foreach (string matched_prefix in matched_prefixs)
	{
		if (line.StartsWith(matched_prefix))
		{
			line = FixLine(line);
			break;
		}
	}

	// 防止有多余的斜杠
	line = line.Replace("//", "/");
	out_string_builder.AppendLine(line);
}

// 修复完成，开始输出
Console.WriteLine(out_string_builder.ToString());
fs.SetLength(0);
using StreamWriter stream_writer = new(fs);
await stream_writer.WriteAsync(out_string_builder.ToString());
return 0;

string Cygpath(string path)
{
	path = path.Replace('\\', '/');
	int index = path.IndexOf(':');
	if (index == -1)
	{
		// 找不到冒号，说明可能已经是 msys 风格的路径了。
		return path;
	}

	// 从开头到冒号的部分，不包括冒号
	string left = path[..index];

	// 从冒号后面的一个字符到结尾。
	string right = path[(index + 1)..];

	left = left.ToLower();
	return $"/{left}/{right}";
}

string FixLine(string line)
{
	(bool success, string key, string value) = line.ParseKeyValueEx();
	if (success)
	{
		line = $"{key}={Cygpath(value)}";
	}

	return line;
}
