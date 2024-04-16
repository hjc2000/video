using PInvokeLib;
using System.Runtime.InteropServices;

FileStream video_file = File.Open(
	"D:/repos/video/cpp/test/资源/idol.mp4",
	FileMode.Open,
	FileAccess.Read,
	FileShare.Read
);
DotNetStreamHelper dotnet_stream_helper = new(video_file);
Test.ReencodeDotNetVideoStream(dotnet_stream_helper.CppObjRawPtr);

internal partial class Test
{
	[LibraryImport("libffmpeg-wrapper", EntryPoint = "ReencodeDotNetVideoStream")]
	public static unsafe partial void ReencodeDotNetVideoStream(nuint dotnet_stream);
}
