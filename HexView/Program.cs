using FileStream fs = File.OpenRead("./ts1.ts");
using BinaryReader binaryReader = new(fs);
using FileStream outputTxt = File.Open("./hex.txt", FileMode.Create);
using StreamWriter streamWriter = new(outputTxt);

int startPos = 188;
int endPos = 188 * 2;

fs.Seek(startPos, SeekOrigin.Begin);

for (int i = 0; i < endPos - startPos; i++)
{
	try
	{
		byte[] bs = binaryReader.ReadBytes(20);
		foreach (byte b in bs)
		{
			streamWriter.Write($"{b:x}\t");
		}

		streamWriter.WriteLine();
	}
	catch
	{
		break;
	}
}

streamWriter.Flush();
Console.WriteLine("完成");
Console.ReadLine();