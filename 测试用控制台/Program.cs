using StreamLib;

string url = @"D:\my_files\workspace\wwwroot\wwwroot\ts0.ts";
using FileStream fileStream = File.Open(url, FileMode.Open);
using FileStream outputFile = File.Open("./ts_parse.txt", FileMode.Create);
StreamWriter outputWriter = new(outputFile);
for (int i = 0; i < 10; i++)
{
	byte[] buff = new byte[188];
	int haveRead = await fileStream.ReadAsync(buff);
	if (haveRead == 188)
	{
		TSPacket tsPacket = new(buff);
		await outputWriter.WriteLineAsync(tsPacket.ToString());
	}
}

await outputWriter.FlushAsync();
Console.WriteLine("完成");
Console.ReadLine();