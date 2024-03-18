namespace JCRazor.文件浏览器;

/// <summary>
///		文件管理器接口
/// </summary>
public interface IFileManager
{
	/// <summary>
	///		切换目录。
	/// </summary>
	/// <param name="full_path">获取 full_path 下的文件和文件夹。full_path 必须是个文件夹，不能是文件。</param>
	/// <returns></returns>
	Task<ChangeDirectoryResult> ChangeDirecoryAsync(string full_path);

	/// <summary>
	///		检查文件或文件夹是否存在。
	/// </summary>
	/// <param name="full_path"></param>
	/// <returns></returns>
	Task<bool> ExistsAsync(string full_path);

	/// <summary>
	///		创建文件夹
	/// </summary>
	/// <param name="full_path"></param>
	/// <returns></returns>
	Task CreateDirectoryAsync(string full_path);

	/// <summary>
	///		下载文件
	/// </summary>
	/// <param name="full_path"></param>
	/// <returns></returns>
	Task DownLoadFileAsync(string full_path);

	/// <summary>
	///		上传文件
	/// </summary>
	/// <param name="full_path"></param>
	/// <param name="file_stream"></param>
	/// <param name="overwrite">
	///		如果目标路径下已存在同名文件，是否覆盖。
	///		* 这个参数传递给服务器，如果不覆盖，又存在同名文件，服务器应该返回错误。
	///	</param>
	/// <returns></returns>
	Task UploadFileAsync(string full_path, Stream file_stream, bool overwrite, CancellationToken cancellationToken);

	/// <summary>
	///		追加文件的内容。即在文件末尾继续补充字节。
	/// </summary>
	/// <param name="full_path">要被追加的文件的路径。如果文件不存在，则会创建一个。</param>
	/// <param name="file_stream">此流的内容将被追加到文件末尾。</param>
	/// <returns>成功返回 0，失败返回负数的错误代码。</returns>
	Task<int> AppendFileAsync(string full_path, Stream file_stream);

	/// <summary>
	///		删除文件或文件夹。
	/// </summary>
	/// <param name="full_path"></param>
	Task DeleteAsync(string full_path);

	/// <summary>
	///		移动文件或文件夹。原地移动，换个名称就可以实现重命名。
	/// </summary>
	/// <param name="old_full_path">原始的路径（包括要被移动的文件本身）</param>
	/// <param name="new_full_path">目标路径（包括要被移动的文件本身）</param>
	/// <param name="overwrite">
	///		如果目标路径下已存在同名文件，是否覆盖。
	///		* 这个参数传递给服务器，如果不覆盖，又存在同名文件，服务器应该返回错误。
	///	</param>
	/// <returns></returns>
	Task MoveAsync(string old_full_path, string new_full_path, bool overwrite);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="old_full_path"></param>
	/// <param name="new_full_path"></param>
	/// <param name="overwrite">
	///		如果目标路径下已存在同名文件，是否覆盖。
	///		* 这个参数传递给服务器，如果不覆盖，又存在同名文件，服务器应该返回错误。
	///	</param>
	/// <returns></returns>
	Task CopyAsync(string old_full_path, string new_full_path, bool overwrite);

	/// <summary>
	///		更改权限
	/// </summary>
	/// <param name="full_path">文件或文件夹的路径。</param>
	/// <param name="mode_string">表示权限的字符串。例如在 linux 中有：777。</param>
	/// <returns></returns>
	Task SetAccessModeAsync(string full_path, string mode_string);
}

public class ChangeDirectoryResult
{
	public List<IDirectoryInfos> DirectoryList { get; } = [];
	public List<IFileInfos> FileList { get; } = [];
}

/// <summary>
///		为什么是 interface，而不是 class 或 struct？因为这可能需要序列化、反序列化。
///		用户可以实现接口，然后自己定义序列化、反序列化的行为。
/// </summary>
public interface IFileInfos
{
	/// <summary>
	///		基路径。要求以 / 结尾。
	/// </summary>
	string BasePath { get; }

	/// <summary>
	///		完整的路径
	/// </summary>
	public string FullPath
	{
		get
		{
			return BasePath + Name;
		}
	}

	/// <summary>
	///		文件名
	/// </summary>
	string Name { get; }

	long Size { get; }

	/// <summary>
	///		表示最后修改时间的字符串
	/// </summary>
	string LastModifyTime { get; }
}

/// <summary>
///		为什么是 interface，而不是 class 或 struct？因为这可能需要序列化、反序列化。
///		用户可以实现接口，然后自己定义序列化、反序列化的行为。
/// </summary>
public interface IDirectoryInfos
{
	/// <summary>
	///		基路径。要求以 / 结尾。
	/// </summary>
	string BasePath { get; }

	/// <summary>
	///		完整的路径
	/// </summary>
	public string FullPath
	{
		get
		{
			return BasePath + Name + "/";
		}
	}

	/// <summary>
	///		目录名
	/// </summary>
	string Name { get; }

	/// <summary>
	///		表示最后修改时间的字符串
	/// </summary>
	string LastModifyTime { get; }
}
