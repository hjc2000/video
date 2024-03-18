#include "FileStream.h"
#include<fstream>

using namespace std;

shared_ptr<FileStream> FileStream::CreateNewAnyway(const char *url)
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
		throw jc::Exception(std::format("{} 创建 {} 失败", CODE_POS_STR, url));
	}

	return fs;
}

shared_ptr<FileStream> FileStream::Open(const char *url)
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
		throw jc::Exception(std::format("{} 打开 {} 失败", CODE_POS_STR, url));
	}

	return fs;
}
