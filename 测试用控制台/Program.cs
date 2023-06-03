using StreamLib;

string url = @"D:\my_files\workspace\wwwroot\wwwroot\ts0.ts";
using FileStream fileStream = File.Open(url, FileMode.Open);
byte[] buff = new byte[188];
int haveRead = await fileStream.ReadAsync(buff);
if (haveRead == 188)
{
	TSPacket tsPacket = new(buff);
	Console.WriteLine(tsPacket);
}
