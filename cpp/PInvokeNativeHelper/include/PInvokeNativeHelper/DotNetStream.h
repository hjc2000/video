#pragma once
#include<jccpp/Exception.h>
#include<jccpp/stream/Stream.h>

/// <summary>
///		本类对象应该由 C# 调用 C 接口函数 CreateDotNetStream 进行创建。
///		C# 侧需要保持住在堆上的本对象的指针，同时，本类对象的生命周期内，
///		绑定的 C# 的 Stream 对象应该始终有效，不能被回收。
/// </summary>
class DotNetStream :public Stream
{
private:
	#pragma region 用来接收 .NET 委托的函数指针
	uint8_t(*_can_read)();
	uint8_t(*_can_write)();
	uint8_t(*_can_seek)();
	int64_t(*_length)();
	void (*_set_length)(int64_t value);
	int32_t(*_read)(uint8_t *buffer, int32_t offset, int32_t count);
	void (*_write)(uint8_t *buffer, int32_t offset, int32_t count);
	void (*_flush)();
	void (*_close)();
	int64_t(*_position)();
	void (*_set_position)(int64_t value);
	uint8_t(*_error)();
	#pragma endregion

	void CheckError();

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
		uint8_t(*can_read)(),
		uint8_t(*can_write)(),
		uint8_t(*can_seek)(),
		int64_t(*length)(),
		void (*set_length)(int64_t value),
		int32_t(*read)(uint8_t *buffer, int32_t offset, int32_t count),
		void (*write)(uint8_t *buffer, int32_t offset, int32_t count),
		void (*flush)(),
		void (*close)(),
		int64_t(*position)(),
		void (*set_position)(int64_t value),
		uint8_t(*error)()
	);

	bool CanRead() override;
	bool CanWrite() override;
	bool CanSeek() override;
	int64_t Length() override;
	void SetLength(int64_t value) override;
	int32_t Read(uint8_t *buffer, int32_t offset, int32_t count) override;
	void Write(uint8_t *buffer, int32_t offset, int32_t count) override;
	void Flush() override;
	void Close() override;
	int64_t Position() override;
	void SetPosition(int64_t value) override;

	/// <summary>
	///		转换成共享指针。
	///		- 返回的共享指针的删除器是个什么都不做的 lambda，因为 DotNetStream
	///		  对象的生命周期是由 .NET 管理的。
	/// </summary>
	/// <returns></returns>
	shared_ptr<DotNetStream> ToSharePtr()
	{
		return shared_ptr<DotNetStream>{ this, [](DotNetStream *p) {} };
	}
};

extern "C"
{
	/// <summary>
	///		C# 调用本函数，将委托传进来。
	///		创建出来的对象由 C# 负责释放，C++ 不要对本函数返回的指针执行 delete。
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
	/// <returns></returns>
	DotNetStream *CreateDotNetStream(
		uint8_t(*can_read)(),
		uint8_t(*can_write)(),
		uint8_t(*can_seek)(),
		int64_t(*length)(),
		void (*set_length)(int64_t value),
		int32_t(*read)(uint8_t *buffer, int32_t offset, int32_t count),
		void (*write)(uint8_t *buffer, int32_t offset, int32_t count),
		void (*flush)(),
		void (*close)(),
		int64_t(*position)(),
		void (*set_position)(int64_t value),
		uint8_t(*error)()
	);

	/// <summary>
	///		CreateDotNetStream 函数创建出来的对象由 C# 负责调用本函数进行释放，
	///		C++ 不要对 CreateDotNetStream 函数返回的指针执行 delete。
	/// </summary>
	/// <param name="obj"></param>
	void FreeDotNetStream(DotNetStream *obj);
}
