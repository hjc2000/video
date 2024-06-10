using JCNET.Math;

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

Fraction f = new(1, 4);
Fraction f1 = new(2, 3);
Console.WriteLine(f + f1);
Console.WriteLine(f - f1);
Console.WriteLine(f * f1);
Console.WriteLine(f / f1);
Console.WriteLine(f.Reciprocal);
Console.WriteLine(f1.Div);
Console.WriteLine(f1.Mod);