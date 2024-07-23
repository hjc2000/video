#region 测试ffmpeg
//using FileStream video_file = File.Open(
//	"F:/repos/video/cpp/test/资源/fallen-down.ts",
//	FileMode.Open,
//	FileAccess.Read,
//	FileShare.Read
//);
//using DotNetStreamHelper dotnet_stream_helper = new(video_file);
//Test.ReencodeDotNetVideoStream(dotnet_stream_helper.CppObjRawPtr);

//internal partial class Test
//{
//	[LibraryImport("libffmpeg-wrapper", EntryPoint = "ReencodeDotNetVideoStream")]
//	public static unsafe partial void ReencodeDotNetVideoStream(nuint dotnet_stream);
//}
#endregion

using JCNET.数学;

string[] num_strs = ["-5.12", "5", "5/-1", "5/2", "/5", "0", 0.13.ToString()];
foreach (string str in num_strs)
{
	Fraction fraction = new(str);
	Console.WriteLine(fraction.Simplify());
}
