#pragma once
#include<IDisposable.h>
#include<cstdint>
#include<filesystem>
#include<fstream>
#include<iostream>
#include<jccpp/CancellationTokenSource.h>
#include<jccpp/Exception.h>
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
	///		读取数据，并且要求至少要读出 minimum_bytes 个字节。除非到达流末尾了。
	///		如果到达流末尾，此时读取到的字节数将会小于 minimum_bytes。
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="offset"></param>
	/// <param name="minimum_bytes"></param>
	/// <returns>
	///		返回读取到的字节数。
	///		一般会大于等于 minimum_bytes，除非到达流结尾了。如果到达流结尾，返回值会小于 minimum_bytes，
	///		此时实际读取到的字节数就是小于 minimum_bytes。
	/// </returns>
	virtual int64_t ReadAtLeast(uint8_t *buffer, int64_t offset, int64_t minimum_bytes);

	/// <summary>
	///		从流中读取精确数量的字节数写入 buffer 中。
	///		如果遇到流末尾，无法满足要求，则读取到的字节数会小于 count。
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="offset"></param>
	/// <param name="count"></param>
	/// <returns>
	///		返回读取到的字节数。一般等于 count，除非到达流末尾，无法满足要求了。
	///		调用者应该检查返回值，判断是否满足要求。
	/// </returns>
	virtual int64_t ReadExactly(uint8_t *buffer, int64_t offset, int64_t count);

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
