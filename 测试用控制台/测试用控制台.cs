using PInvokeLib;
using System.Runtime.InteropServices;

using FileStream video_file = File.Open(
	"F:/repos/video/cpp/test/资源/fallen-down.ts",
	FileMode.Open,
	FileAccess.Read,
	FileShare.Read
);
using DotNetStreamHelper dotnet_stream_helper = new(video_file);
Test.ReencodeDotNetVideoStream(dotnet_stream_helper.CppObjRawPtr);

internal partial class Test
{
	[LibraryImport("libffmpeg-wrapper", EntryPoint = "ReencodeDotNetVideoStream")]
	public static unsafe partial void ReencodeDotNetVideoStream(nuint dotnet_stream);
}

//using System.Management.Automation;

//PowerShell pwsh = PowerShell.Create();
//PSDataCollection<PSObject> results = await pwsh.AddScript("Get-Location")
//	.InvokeAsync();
//if (pwsh.Streams.Error.Count > 0)
//{
//	Console.WriteLine("出错了");
//	foreach (ErrorRecord error in pwsh.Streams.Error)
//	{
//		Console.WriteLine(error.ToString());
//	}
//}
//else
//{
//	foreach (PSObject result in results)
//	{
//		Console.WriteLine(result.ToString());
//	}
//}
