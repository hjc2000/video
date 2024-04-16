using PInvokeLib;
using System.Runtime.InteropServices;

using FileStream video_file = File.Open(
	"F:/repos/video/cpp/test/资源/idol.mp4",
	FileMode.Open,
	FileAccess.Read,
	FileShare.Read
);
using DotNetStreamHelper dotnet_stream_helper = new(video_file);
Test.ReencodeDotNetVideoStream(dotnet_stream_helper.CppObjRawPtr);

internal partial class Test
{
	/// <summary>
	///		需要改成能够重复打开文件。否则ffmpeg读取文件完了会Close
	/// </summary>
	/// <param name="dotnet_stream"></param>
	[LibraryImport("libffmpeg-wrapper", EntryPoint = "ReencodeDotNetVideoStream")]
	public static unsafe partial void ReencodeDotNetVideoStream(nuint dotnet_stream);
}
