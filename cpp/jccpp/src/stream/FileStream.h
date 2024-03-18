#pragma once
#include<Stream.h>

class FileStream :public Stream
{
private:
	FileStream(string url)
	{
		_url = url;
	}

public:
	~FileStream()
	{
		Close();
	}

public:
	#pragma region 工厂函数
	/// <summary>
	///		用创建模式打开文件流，无论如何都会创建一个新的空白文件。会覆盖旧的。
	/// </summary>
	/// <remark>
	///		创建模式就是：
	///		* 如果文件不存在，创建一个新的。
	///		* 如果文件存在，会打开，然后将长度截断为 0，相当于创建了一个新文件然后覆盖。
	/// </remark>
	/// <param name="url">文件路径</param>
	/// <returns>创建文件成功则返回 FileStream 对象</returns>
	static shared_ptr<FileStream> CreateNewAnyway(const char *url);

	/// <summary>
	///		用打开模式打开一个文件。如果文件不存在，此函数不会创建文件。
	/// </summary>
	/// <param name="url">文件路径</param>
	/// <returns>如果文件存在，且成功打开，则返回 FileStream 对象。如果文件不存在，返回空指针。</returns>
	/// <exception cref="Exception 如果传进来的 url 不是文件，而是目录，会抛出异常"></exception>
	static shared_ptr<FileStream> Open(const char *url);
	#pragma endregion

public:
	bool CanRead() override
	{
		return true;
	}

	bool CanWrite() override
	{
		return true;
	}

	bool CanSeek() override
	{
		return true;
	}

	int64_t Length() override
	{
		// 记录当前位置
		int64_t current_pos = _fs->tellg();

		// seek 到文件末尾
		_fs->seekg(0, _fs->end);

		// 记录文件末尾的位置（最后一个字节之后一个字节，所以 end_pos 等于文件长度）
		int64_t end_pos = _fs->tellg();

		// seek 回原来的位置
		_fs->seekg(current_pos);
		return end_pos;
	}

	void SetLength(int64_t value) override
	{
		// 防止 Position 属性超出边界
		int64_t current_pos = Position();
		int64_t new_pos = std::min(value, current_pos);
		SetPosition(new_pos);

		// 重设大小
		std::filesystem::resize_file(_url.c_str(), value);
		std::cout << "更改大小后文件大小=" << Length() << std::endl;
	}

	int64_t Read(uint8_t *dst_buf, int64_t offset, int64_t count) override
	{
		_fs->read((char *)(dst_buf + offset), count);
		int64_t have_read = _fs->gcount();
		SetPosition(_fs->tellg());
		return have_read;
	}

	void Write(uint8_t *src_buf, int64_t offset, int64_t count) override
	{
		_fs->write((char *)(src_buf + offset), count);
		SetPosition(_fs->tellp());
	}

	int64_t Position() override
	{
		return _fs->tellg();
	}

	void SetPosition(int64_t value) override
	{
		/* 必须先清除标志。因为如果不清除，上次读写如果触发了 eof 了，即使在这里 seek 到非尾部
		* 下次读写流时仍会因为 eof 已经被设置了而无法读写。
		*/
		_fs->clear();
		_fs->seekg(value);
		_fs->seekp(value);
	}

	void Flush() override
	{
		_fs->flush();
	}

	void Close() override
	{
		_fs->close();
	}

private:
	string _url;
	shared_ptr<fstream> _fs;
};