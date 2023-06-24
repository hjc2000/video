#include<FFmpeg.h>

/// <summary>
/// 尝试解封装功能
/// </summary>
void try_remux()
{
	FFmpeg::AVFormatContext iformat_context;
	FFmpeg::AVFormatContext oformat_context;
	iformat_context.open_input("./re.ts");
	oformat_context.alloc_output_context2("output.ts");
	// 查找输入文件的流信息
	iformat_context.find_stream_info();
	// 输入文件的流数量
	int intput_stream_count = iformat_context()->nb_streams;
	// 创建流映射数组
	int* stream_map = new int[intput_stream_count];
	int dst_stream_index = 0;// 流要复制到的目标索引
	// 遍历每一个输入流，对输入流进行过滤，为输出格式创建流，将通过过滤的输入流的信息复制到输出流
	for (int i = 0; i < intput_stream_count; i++)
	{
		FFmpeg::AVStream current_input_stream = iformat_context.get_stream(i);
		// 过滤流。只保留视频流、音频流、字幕流
		if (current_input_stream()->codecpar->codec_type != FFmpeg::AVMediaType::AVMEDIA_TYPE_AUDIO &&
			current_input_stream()->codecpar->codec_type != FFmpeg::AVMediaType::AVMEDIA_TYPE_VIDEO &&
			current_input_stream()->codecpar->codec_type != FFmpeg::AVMediaType::AVMEDIA_TYPE_SUBTITLE)
		{
			// 不是视频、音频、字幕流
			stream_map[i] = -1;
		}
		else
		{
			// 符合过滤条件
			stream_map[i] = dst_stream_index++;
			FFmpeg::AVStream current_output_stream = oformat_context.create_new_stream();
			current_input_stream.copy_parameters_to(current_output_stream);
			current_output_stream()->codecpar->codec_tag = 0;
		}
	}
	// 打印输出格式信息
	oformat_context.dump_format(0, "file", 1);
	cout << "--------------------------" << endl;
	oformat_context.write_header();
	try
	{
		FFmpeg::AVPacket packet;
		while (1)
		{
			iformat_context.read_frame(packet);
			FFmpeg::AVStream input_stream = iformat_context.get_stream(packet()->stream_index);
			// 如果映射表中的目标索引号是负数，表示此流不被复制到输出格式中
			if (stream_map[packet()->stream_index] >= 0)
			{
				// 将此包的流索引号改为目标流索引号，这样这个包就会被写入输出格式的中的目标索引号
				// 的流中
				packet()->stream_index = stream_map[packet()->stream_index];
				FFmpeg::AVStream output_stream = oformat_context.get_stream(packet()->stream_index);
				packet.av_packet_rescale_ts(input_stream()->time_base, output_stream()->time_base);
				packet()->pos = -1;
				oformat_context.interleaved_write_frame(packet);
			}
			packet.unref();
		}
	}
	catch (int err_code)
	{
		cout << "错误码：" << err_code << endl;
		std::cerr << FFmpeg::error_code_to_str(err_code) << endl;
		cout << "写尾部" << endl;
		oformat_context.write_trailer();
	}

	delete[] stream_map;
}
