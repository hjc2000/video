using PInvokeLib;

using MemoryStream memoryStream = new();
byte[] buffer = [1, 2, 3, 4, 5];
memoryStream.Write(buffer);
memoryStream.Position = 0;
using DotNetStreamHelper dotNetStreamHelper = new(memoryStream);
DotNetStreamHelper.TestDotNetStream(dotNetStreamHelper.CppObjRawPtr);