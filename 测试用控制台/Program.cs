using StreamLib;

string url = @"./ts0.ts";
using FileStream fileStream = File.Open(url, FileMode.Open, FileAccess.Read, FileShare.Read);
using FileStream outputFile = File.Open("./ts_parse.json", FileMode.Create);
StreamWriter outputWriter = new(outputFile);
using FileStream outputPAT = File.Open("./pat.json", FileMode.Create);
StreamWriter patWriter = new(outputPAT);
for (int i = 0; i < 100; i++)
{
	while (true)
	{
		int syncByte = fileStream.ReadByte();
		if (syncByte == 0x47)
		{
			break;
		}
		else if (syncByte == -1)
		{
			Console.WriteLine("文件结束");
			return;
		}
	}

	byte[] buff = new byte[187];
	int haveRead = await fileStream.ReadAsync(buff);
	if (haveRead == 187)
	{
		await outputWriter.WriteLineAsync($"第{i + 1}个包");
		TSPacket tsPacket = new(buff);
		if (tsPacket.PID == 0 && tsPacket.Payload != null)
		{
			await patWriter.WriteLineAsync(new PAT(tsPacket.Payload.ActualPayload).ToString());
		}

		await outputWriter.WriteLineAsync(tsPacket.ToString());
	}
	else
	{
		Console.WriteLine("读取的不足187字节");
	}
}

await outputWriter.FlushAsync();
await patWriter.FlushAsync();
Console.WriteLine("完成");
Console.ReadLine();
