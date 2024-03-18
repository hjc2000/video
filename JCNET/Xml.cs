using System.Text;
using System.Xml.Serialization;

namespace JCNET;

/// <summary>
/// XML 反序列化错误
/// </summary>
public class XmlDeserializeException : Exception
{
	public XmlDeserializeException() { }
	public XmlDeserializeException(string? message, Exception? innerException) : base(message, innerException) { }
}

/// <summary>
/// XML 序列化错误
/// </summary>
public class XmlSerializeException : Exception
{
	public XmlSerializeException() { }
	public XmlSerializeException(string? message, Exception? innerException) : base(message, innerException) { }
}

public static class Xml
{
	#region 序列化
	/// <summary>
	///		将对象序列化为 XML 字符串
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="o"></param>
	/// <returns></returns>
	/// <exception cref="XmlSerializeException"></exception>
	public static string ObjToXml<T>(object? o)
	{
		try
		{
			XmlSerializer xmlSerializer = new(typeof(T));
			using MemoryStream memoryStream = new();
			xmlSerializer.Serialize(memoryStream, o);
			return Encoding.UTF8.GetString(memoryStream.ToArray());
		}
		catch (Exception e)
		{
			throw new XmlSerializeException("ObjToXml error", e);
		}
	}

	/// <summary>
	///		序列化对象并将字符串写入流中。传进来的流必须支持同步写入，否则会抛出异常。
	///		如果你的流不支持同步写入，可以先传进来 MemoryStream，然后再把 MemoryStream
	///		用异步的方式复制到你的流中
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="o"></param>
	/// <param name="stream"></param>
	/// <exception cref="XmlSerializeException"></exception>
	/// <returns></returns>
	public static async Task ObjToXmlStreamAsync<T>(object? o, Stream stream)
	{
		try
		{
			MemoryStream mstream = new();
			XmlSerializer xmlSerializer = new(typeof(T));
			xmlSerializer.Serialize(mstream, o);
			mstream.Position = 0;
			await mstream.CopyToAsync(stream);
		}
		catch (Exception e)
		{
			throw new XmlSerializeException("ObjToXmlStream error", e);
		}
	}
	#endregion

	#region 反序列化
	/// <summary>
	///		将 XML 字符串反序列化为对象
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="xml"></param>
	/// <returns></returns>
	/// <exception cref="XmlDeserializeException"></exception>
	public static T? XmlToObj<T>(string xml)
	{
		try
		{
			XmlSerializer xmlSerializer = new(typeof(T));
			byte[] buff = Encoding.UTF8.GetBytes(xml);
			using MemoryStream memoryStream = new(buff);
			return (T?)xmlSerializer.Deserialize(memoryStream);
		}
		catch (Exception e)
		{
			throw new XmlDeserializeException("XmlToObj error", e);
		}
	}

	/// <summary>
	///		将含有 xml 的流反序列化为对象。传进来的流必须支持同步读取，否则会抛出异常。
	///		如果你的流不支持同步读取，可以先用异步的方式读取到 MemoryStream 中，然后
	///		将 MemoryStream 传进来。
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="xmlStream"></param>
	/// <returns></returns>
	/// <exception cref="XmlDeserializeException"></exception>
	public static async Task<T?> XmlStreamToObjAsync<T>(Stream xmlStream)
	{
		try
		{
			MemoryStream mstream = new();
			await xmlStream.CopyToAsync(mstream);
			mstream.Position = 0;
			XmlSerializer xmlSerializer = new(typeof(T));
			return (T?)xmlSerializer.Deserialize(mstream);
		}
		catch (Exception ex)
		{
			throw new XmlDeserializeException("XmlStreamToObj error", ex);
		}
	}
	#endregion
}
