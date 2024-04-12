using PInvokeLib;

using CppStream video_stream = CppStream.GetFileStream(@"C:\Users\huang\source\repos\video\ffmpeg\资源\zf.ts");
CancellationTokenSource cancel_read = new();
_ = Task.Run(async () =>
{
    byte[] buffer = new byte[1024];
    CancellationToken cancellationToken = cancel_read.Token;
    try
    {
        while (!cancellationToken.IsCancellationRequested)
        {
            await video_stream.ReadAsync(buffer, 0, buffer.Length, cancellationToken);
        }
    }
    catch (Exception ex)
    {
        Console.WriteLine(ex.ToString());
    }
});

Console.ReadLine();
cancel_read.Cancel();
video_stream.Close();
