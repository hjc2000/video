string oldFilePath = @"74669624_nb2-1-30280.m4s";
string newFilePath = GetNewFilePath(oldFilePath);
Console.WriteLine(newFilePath);

// 打开旧文件
using FileStream oldFile = File.OpenRead(oldFilePath);

// 创建新文件
using FileStream newFile = File.Create(newFilePath);

// 移动流的位置，目的是去掉前 9 个字节
oldFile.Position = 9;

// 旧流复制到新流中
await oldFile.CopyToAsync(newFile);
await newFile.FlushAsync();

// 读取新文件看看对不对
newFile.Position = 0;
using BinaryReader reader = new(newFile);
for (int i = 0; i < 100; i++)
{
	byte aByte = reader.ReadByte();
	Console.Write(aByte + " ");
	if ((i + 1) % 20 == 0)
	{
		Console.WriteLine();
	}
}

static string GetNewFilePath(string oldFilePath)
{
	// 获取旧的信息
	string oldFileName = Path.GetFileNameWithoutExtension(oldFilePath);
	string extensionName = Path.GetExtension(oldFilePath);
	// 产生新的路径
	string newFullFileName = oldFileName + "_破解" + extensionName;
	return newFullFileName;
}
