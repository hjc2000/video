#pragma once
#include<jccpp/Exception.h>
#include<jccpp/stream/Stream.h>

/// <summary>
///		本类对象应该由 C# 调用 C 接口函数 CreateDotnetStream 进行创建。
///		C# 侧需要保持住在堆上的本对象的指针，同时，本类对象的生命周期内，
///		绑定的 C# 的 Stream 对象应该始终有效，不能被回收。
/// </summary>
class DotNetStream :public Stream
{
public:
	/// <summary>
	///		.NET 需要提供这些委托供 C++ 调用。此外，在使用本类对象期间，
	///		必须保证 .NET 侧的 Stream 对象不被回收。
	/// </summary>
	/// <param name="can_read"></param>
	/// <param name="can_write"></param>
	/// <param name="can_seek"></param>
	/// <param name="length"></param>
	/// <param name="set_length"></param>
	/// <param name="read"></param>
	/// <param name="write"></param>
	/// <param name="flush"></param>
	/// <param name="close"></param>
	/// <param name="position"></param>
	/// <param name="set_position"></param>
	/// <param name="error"></param>
	/// <param name="error_message_size"></param>
	/// <param name="error_message_buffer"></param>
	DotNetStream(
		bool (*can_read)(),
		bool (*can_write)(),
		bool (*can_seek)(),
		int64_t(*length)(),
		void (*set_length)(int64_t value),
		int64_t(*read)(uint8_t *buffer, int64_t offset, int64_t count),
		void (*write)(uint8_t *buffer, int64_t offset, int64_t count),
		void (*flush)(),
		void (*close)(),
		int64_t(*position)(),
		void (*set_position)(int64_t value),
		bool (*error)(),
		int64_t(*error_message_size)(),
		uint8_t *(*error_message_buffer)()
	);

private:
	bool (*_can_read)();
	bool (*_can_write)();
	bool (*_can_seek)();
	int64_t(*_length)();
	void (*_set_length)(int64_t value);
	int64_t(*_read)(uint8_t *buffer, int64_t offset, int64_t count);
	void (*_write)(uint8_t *buffer, int64_t offset, int64_t count);
	void (*_flush)();
	void (*_close)();
	int64_t(*_position)();
	void (*_set_position)(int64_t value);
	bool (*_error)();
	int64_t(*_error_message_size)();
	uint8_t *(*_error_message_buffer)();

	void CheckError();

public:
	bool CanRead() override;
	bool CanWrite() override;
	bool CanSeek() override;
	int64_t Length() override;
	void SetLength(int64_t value) override;
	int64_t Read(uint8_t *buffer, int64_t offset, int64_t count) override;
	void Write(uint8_t *buffer, int64_t offset, int64_t count) override;
	void Flush() override;
	void Close() override;
	int64_t Position() override;
	void SetPosition(int64_t value) override;
};

extern "C"
{
	DotNetStream *CreateDotnetStream(
		bool (*can_read)(),
		bool (*can_write)(),
		bool (*can_seek)(),
		int64_t(*length)(),
		void (*set_length)(int64_t value),
		int64_t(*read)(uint8_t *buffer, int64_t offset, int64_t count),
		void (*write)(uint8_t *buffer, int64_t offset, int64_t count),
		void (*flush)(),
		void (*close)(),
		int64_t(*position)(),
		void (*set_position)(int64_t value),
		bool (*error)(),
		int64_t(*error_message_size)(),
		uint8_t *(*error_message_buffer)()
	);

	void FreeDotnetStream(DotNetStream *obj);
}