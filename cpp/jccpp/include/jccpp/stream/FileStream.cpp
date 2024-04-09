#include"jccpp/stream/FileStream.h"
#include<fstream>

using namespace std;

bool FileStream::CanRead()
{
	return _can_read;
}

bool FileStream::CanWrite()
{
	return _can_write;
}

bool FileStream::CanSeek()
{
	return _can_seek;
}

shared_ptr<FileStream> FileStream::CreateNewAnyway(std::string url)
{
	shared_ptr<FileStream> fs{ new FileStream{url} };

	/* 加上 ios_base::trunc，这样打开文件流后，如果原本存在此文件，就会将其截断，
	* 让其初始长度变成 0，相当于一个新文件。
	*/
	fs->_fs = shared_ptr<fstream>{
		new fstream{
			url,
			ios_base::out | ios_base::in | ios_base::trunc | ios_base::binary
		},
	};

	if (fs->_fs->fail())
	{
		throw jc::Exception(std::format("{} 创建 {} 失败。检查文件是不是只读的。", CODE_POS_STR, url));
	}

	fs->_can_read = true;
	fs->_can_write = true;
	fs->_can_seek = true;
	return fs;
}

shared_ptr<FileStream> FileStream::Open(std::string url)
{
	if (!filesystem::exists(url))
	{
		// 文件不存在
		throw jc::Exception(std::format("{} 文件 {} 不存在。", CODE_POS_STR, url));
	}

	if (filesystem::is_directory(url))
	{
		throw jc::Exception(std::format("{} {} 不是一个文件，而是一个目录", CODE_POS_STR, url));
	}

	shared_ptr<FileStream> fs{ new FileStream{url} };
	fs->_fs = shared_ptr<fstream>{
		new fstream{
			url,
			ios_base::in | ios_base::out | ios_base::binary
		},
	};

	if (fs->_fs->fail())
	{
		throw jc::Exception(std::format("{} 打开 {} 失败。检查文件是不是只读的。", CODE_POS_STR, url));
	}

	fs->_can_read = true;
	fs->_can_write = true;
	fs->_can_seek = true;
	return fs;
}

int64_t FileStream::Length()
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

void FileStream::SetLength(int64_t value)
{
	// 防止 Position 属性超出边界
	int64_t current_pos = Position();
	SetPosition(std::min(value, current_pos));

	// 重设大小
	std::filesystem::resize_file(_url.c_str(), value);
	std::cout << "更改大小后文件大小=" << Length() << std::endl;
}

int64_t FileStream::Position()
{
	return _fs->tellg();
}

void FileStream::SetPosition(int64_t value)
{
	/* 必须先清除标志。因为如果不清除，上次读写如果触发了 eof 了，即使在这里 seek 到非尾部
	* 下次读写流时仍会因为 eof 已经被设置了而无法读写。
	*/
	_fs->clear();
	_fs->seekg(value);
	_fs->seekp(value);
}

int64_t FileStream::Read(uint8_t *buffer, int64_t offset, int64_t count)
{
	_fs->read((char *)(buffer + offset), count);
	int64_t have_read = _fs->gcount();
	SetPosition(_fs->tellg());
	return have_read;
}

void FileStream::Write(uint8_t *buffer, int64_t offset, int64_t count)
{
	_fs->write((char *)(buffer + offset), count);
	SetPosition(_fs->tellp());
}

void FileStream::Flush()
{
	_fs->flush();
}

void FileStream::Close()
{
	_fs->close();
}