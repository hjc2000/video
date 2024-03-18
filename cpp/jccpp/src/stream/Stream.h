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
	///		读取指定的字节数到目标缓冲区中
	/// </summary>
	/// <param name="dst_buf">要将读取到的数据写入的缓冲区。</param>
	/// <param name="offset">将读取到的数据写入 dst_buf 时的起始位置。</param>
	/// <param name="count">要读取的字节数。</param>
	/// <returns>实际读取的字节数。如果返回 0，说明此流结束。</returns>
	virtual int64_t Read(uint8_t *dst_buf, int64_t offset, int64_t count) = 0;

	/// <summary>
	///		将 src_buf 中的数据从 offset 处开始往后，一共 count 个字节写入流中。
	/// </summary>
	/// <param name="src_buf">src_buf 数据源缓冲区。</param>
	/// <param name="offset">从 src_buf 中取数据的起始位置。</param>
	/// <param name="count">从 src_buf 中取多少个数据。</param>
	virtual void Write(uint8_t *src_buf, int64_t offset, int64_t count) = 0;

	/// <summary>
	///		* 对于写入的数据，作用是将其从内部缓冲区转移到底层。
	///		* 对于内部的可以读取但尚未读取的数据，一般不会有什么作用。Flush 没见过对可读数据生效的。
	/// </summary>
	virtual void Flush() = 0;

	/// <summary>
	///		关闭或结束流。
	/// 
	///		* 不一定要在关闭后禁止读写操作，可以只禁止写入，不禁止读取。因为关闭后可能还需要读出残留数据。
	///		  应该具体情况具体分析，派生类应该详细注释。
	/// 
	///		* 不管是什么原因，原则上应该避免关闭后还能写入，最多只能允许关闭后继续读取残留数据。
	/// 
	///		* 关闭也可以是为了取消信号量的阻塞，或者停止内部线程之类的。
	/// 
	///		* 对于网络流之类的，Close 方法一般设计为关闭套接字，可以考虑关闭后，Read 方法永远返回 0，表示流结束了。
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
