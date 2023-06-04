using FileStream fs = File.OpenRead("./ts0.ts");
using BinaryReader binaryReader = new(fs);
using FileStream outputTxt = File.Open("./hex.txt", FileMode.Create);
using StreamWriter streamWriter = new(outputTxt);

int startPos = 0;
int endPos = 188 * 10;

fs.Seek(startPos, SeekOrigin.Begin);

for (int i = 0; i < endPos - startPos; i++)
{
	try
	{
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

streamWriter.Flush();
Console.WriteLine("完成");
Console.ReadLine();