string oldFilePath = @"D:\my_files\workspace\temp\80149248_185_0.mp4";
string newFilePath = GetNewFilePath(oldFilePath);

// 打开旧文件
FileStream oldFile = File.OpenRead(oldFilePath);
// 创建新文件
FileStream newFile = File.Create(newFilePath);
using (oldFile)
using (newFile)
{
	// 移动流的位置，目的是去掉前3个字节
	oldFile.Position = 3;
	// 旧流复制到新流中
	await oldFile.CopyToAsync(newFile);
	await newFile.FlushAsync();

	// 读取新文件看看对不对
	newFile.Position = 0;
	BinaryReader reader = new(newFile);
	using (reader)
	{
		for (int i = 0; i < 100; i++)
		{
			byte aByte = reader.ReadByte();
			Console.Write(aByte + " ");
			if ((i + 1) % 20 == 0)
			{
				Console.WriteLine();
			}
		}
	}
}

static string GetNewFilePath(string oldFilePath)
{
	// 获取旧的信息
	string dir = Path.GetDirectoryName(oldFilePath) ?? string.Empty;
	string oldFileName = Path.GetFileNameWithoutExtension(oldFilePath);
	string extensionName = Path.GetExtension(oldFilePath);
	// 产生新的路径
	string newFullFileName = oldFileName + "_破解" + extensionName;
	string newPath = dir + "\\" + newFullFileName;
	return newPath;
}
