#pragma once
#include<CancellationTokenSource.h>
#include<Exception.h>
#include<IDisposable.h>
#include<cstdint>
#include<filesystem>
#include<fstream>
#include<iostream>
#include<memory>
#include<string>

using std::string;
using std::fstream;
using std::shared_ptr;

class Stream
{
public:
	virtual ~Stream() {}

public:
	virtual bool CanRead() = 0;
	virtual bool CanWrite() = 0;
	virtual bool CanSeek() = 0;

	virtual int64_t Length() = 0;
	virtual void SetLength(int64_t value) = 0;

	/// <summary>
	///		从流中读取数据写入 buffer。
	/// </summary>
	/// <param name="buffer">要将读取到的数据写入的缓冲区。</param>
	/// <param name="offset">将读取到的数据写入 dst_buf 时的起始位置。</param>
	/// <param name="count">要读取的字节数。</param>
	/// <returns>实际读取的字节数。如果返回 0，说明此流结束。</returns>
	virtual int64_t Read(uint8_t *buffer, int64_t offset, int64_t count) = 0;

	/// <summary>
	///		将 buffer 中的数据写入流中。
	/// </summary>
	/// <param name="buffer">数据源缓冲区。</param>
	/// <param name="offset">从 buffer 中取数据的起始位置。</param>
	/// <param name="count">从 buffer 中取多少个字节。</param>
	virtual void Write(uint8_t *buffer, int64_t offset, int64_t count) = 0;

	/// <summary>
	///		* 对于写入的数据，作用是将其从内部缓冲区转移到底层。
	///		* 对于内部的可以读取但尚未读取的数据，一般不会有什么作用。Flush 没见过对可读数据生效的。
	/// </summary>
	virtual void Flush() = 0;

	/// <summary>
	///		关闭后对流的操作将会引发异常。
	/// </summary>
	virtual void Close() = 0;

	virtual int64_t Position() = 0;
	virtual void SetPosition(int64_t value) = 0;

	/// <summary>
	///		将本流复制到目标流中。
	/// </summary>
	/// <param name="dst_stream">目标流。</param>
	/// <param name="token">用来取消复制操作</param>
	/// <exception cref="TaskCanceledException 取消复制会抛出异常"></exception>
	virtual void CopyTo(Stream *dst_stream, std::shared_ptr<CancellationToken> cancellationToken = nullptr);
};
