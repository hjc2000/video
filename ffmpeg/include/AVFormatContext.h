#pragma once
#include<Wraper.h>
#include<include_ffmpeg.h>

namespace FFmpeg
{
	class AVPacket;
	class AVStream;
	class AVDictionary;

	class AVFormatContext : public Wraper<::AVFormatContext>
	{
		#pragma region 生命周期
	public:
		AVFormatContext() {}
		AVFormatContext(::AVFormatContext *p) :Wraper(p) {}
		AVFormatContext(::AVFormatContext &ref) :Wraper(ref) {}
		~AVFormatContext();
		#pragma endregion

		#pragma region 私有字段
	private:
		/// <summary>
		/// 用来标识 AVFormatContext 当前是哪一种模式的枚举类型
		/// </summary>
		enum Mode
		{
			/// <summary>
			/// 表示 AVFormatContext 对象还没被初始化，即它以后可能是输入格式，也可能是输出格式
			/// </summary>
			unset,

			/// <summary>
			/// 表示 AVFormatContext 对象已经被初始化为输入格式了
			/// </summary>
			input,

			/// <summary>
			/// 表示 AVFormatContext 对象已经被初始化为输出格式了
			/// </summary>
			output,
		};

		/// <summary>
		/// 本对象当前是哪一种模式
		/// </summary>
		Mode _mode = Mode::unset;
		#pragma endregion

		#pragma region 初始化函数
	public:
		/// <summary>
		/// 打开指定 url 作为输入源。会将此 AVFormatContext 对象初始化为输入格式。
		/// </summary>
		/// <param name="url"></param>
		/// <param name="fmt"></param>
		/// <param name="options"></param>
		static shared_ptr< FFmpeg::AVFormatContext> open_input(const char *url, const ::AVInputFormat *fmt = nullptr, ::AVDictionary **options = nullptr);

		/// <summary>
		/// 创建输出格式上下文。会将此 AVFormatContext 对象变成输出格式
		/// </summary>
		/// <param name="filename"></param>
		static shared_ptr<FFmpeg::AVFormatContext> alloc_output_context2(const char *filename);
		#pragma endregion

		#pragma region 包装方法
	public:
		/// <summary>
		/// 以官方格式打印流信息
		/// </summary>
		/// 
		/// <param name="index">想要打印信息的流的索引号</param>
		/// 
		/// <param name="url">
		/// 自定义的 url，比如这是一个输出文件，那么 url 就是你定义的输出文件路径。
		/// 你随便输都没关系，这里的 url 的用途只有被显示
		/// </param>
		/// 
		/// <param name="is_output">
		/// 是否是输出文件。你也可以随便定，这个参数的用途只是用来被显示
		/// </param>
		void dump_format(int index, const char *url, int is_output);

		/// <summary>
		/// 以官方格式打印流信息
		/// </summary>
		/// 
		/// <param name="stream">
		/// 想要打印信息的流。本质上是读取流里面的 index 字段，所以即使传入的流不是本格式中的
		/// 流，也有可能打印出信息，只不过这肯定不对。所以不要传入别的格式的流
		/// </param>
		/// 
		/// <param name="url">
		/// 自定义的 url，比如这是一个输出文件，那么 url 就是你定义的输出文件路径。
		/// 你随便输都没关系，这里的 url 的用途只有被显示
		/// </param>
		/// 
		/// <param name="is_output">
		/// 是否是输出文件。你也可以随便定，这个参数的用途只是用来被显示
		/// </param>
		void dump_format(FFmpeg::AVStream stream, const char *url, int is_output);

		/// <summary>
		/// 通过读几个包来检测流信息。此操作不会导致读取进度向前推移
		/// </summary>
		/// <param name="options"></param>
		void find_stream_info(::AVDictionary **options = nullptr);

		/// <summary>
		/// 找出最好的流。如果找不到指定的流，会抛出异常
		/// </summary>
		/// <param name="type"></param>
		/// <returns>返回找到的流</returns>
		shared_ptr<FFmpeg::AVStream> find_best_stream(AVMediaType type);

		/// <summary>
		/// 成功返回 0，失败返回错误代码
		/// </summary>
		/// <param name="packet"></param>
		/// <returns></returns>
		int read_packet(shared_ptr<FFmpeg::AVPacket> packet);

		/// <summary>
		///		只有输出格式才能使用这个函数
		/// </summary>
		/// <param name="pCodec"></param>
		/// <returns></returns>
		FFmpeg::AVStream create_new_stream(const ::AVCodec *pCodec = nullptr);

		/// <summary>
		///	向格式写头部
		/// </summary>
		/// <param name="dic"></param>
		void write_header(FFmpeg::AVDictionary *dic = nullptr);

		/// <summary>
		///	以交织的方式向格式写入一个包。只有输出格式才能调用本方法。
		/// </summary>
		/// <param name="packet"></param>
		void interleaved_write_packet(FFmpeg::AVPacket packet);

		/// <summary>
		///	向格式写尾部
		/// </summary>
		void write_trailer();
		#pragma endregion

		#pragma region 扩展包装方法
	public:
		/// <summary>
		///	获取视频时长。要先调用 find_stream_info 方法分析流信息后才能调用此方法，否则得到
		///	的结果是错误的
		/// </summary>
		/// <returns>返回结果是一个字符串，里面储存着格式化过的时间</returns>
		std::string get_duration_as_formatted_time_string();

		/// <summary>
		///	获取指定索引的流。流的索引号超出范围会抛出异常
		/// </summary>
		/// <param name="stream_index"></param>
		/// <returns></returns>
		FFmpeg::AVStream get_stream(int stream_index);
	};
	#pragma endregion
}