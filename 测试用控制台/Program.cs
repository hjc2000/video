using StreamLib;

SplicedStream splicedStream = new();
int i = 0;
splicedStream.OnStreamQueueEmpty += async (tcs) =>
{
	if (i <= 4)
	{
		FileStream fileStream = File.Open(@"D:\my_files\workspace\wwwroot\wwwroot\" + $"ts{i++}.ts", FileMode.Open);
		splicedStream.PushBack(fileStream);
	}

	Console.WriteLine("即将等待1s");
	await Task.Delay(1000);
	Console.WriteLine("结束等待");
	Console.WriteLine();
	tcs.SetResult();
};
FileStream outputFile = File.Open(@"D:\my_files\workspace\wwwroot\wwwroot\out.ts", FileMode.Create);
await splicedStream.CopyToAsync(outputFile);
await outputFile.FlushAsync();
Console.WriteLine("结束");
Console.ReadLine();