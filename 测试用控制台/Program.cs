using StreamLib;

string url = @"./ts0.ts";
using FileStream fileStream = File.Open(url, FileMode.Open, FileAccess.Read, FileShare.Read);
using FileStream outputFile = File.Open("./ts_parse.json", FileMode.Create);
StreamWriter outputWriter = new(outputFile);
for (int i = 0; i < 100; i++)
{
	byte[] buff = new byte[188];
	int haveRead = await fileStream.ReadAsync(buff);
	if (haveRead == 188)
	{
		await outputWriter.WriteLineAsync($"第{i + 1}个包");
		TSPacket tsPacket = new(buff);
		await outputWriter.WriteLineAsync(tsPacket.ToString());
	}
	else
	{
		Console.WriteLine("读取的不足188字节");
	}
}

await outputWriter.FlushAsync();
Console.WriteLine("完成");
Console.ReadLine();
