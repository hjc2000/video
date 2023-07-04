#include<FFmpeg.h>

/// <summary>
/// 尝试解封装功能
/// </summary>
void remux_main()
{
	FFmpeg::AVFormatContext inputFormatCtx;
	FFmpeg::AVFormatContext outputFormatCtx;
	inputFormatCtx.open_input("./re.ts");
	outputFormatCtx.alloc_output_context2("output.ts");
	// 查找输入文件的流信息
	inputFormatCtx.find_stream_info();
	// 输入文件的流数量
	int intput_stream_count = inputFormatCtx()->nb_streams;
	// 创建流映射数组
	int *stream_map = new int[intput_stream_count];
	// 流要复制到的目标索引
	int dst_stream_index = 0;
	// 遍历每一个输入流，对输入流进行过滤，为输出格式创建流，将通过过滤的输入流的信息复制到输出流
	for (int i = 0; i < intput_stream_count; i++)
	{
		FFmpeg::AVStream current_input_stream = inputFormatCtx.get_stream(i);
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
			FFmpeg::AVStream new_output_stream = outputFormatCtx.create_new_stream();
			current_input_stream.copy_parameters_to(new_output_stream);
			new_output_stream()->codecpar->codec_tag = 0;
		}
	}

	// 打印输出格式信息
	outputFormatCtx.dump_format(0, "file", 1);
	cout << "--------------------------" << endl;
	outputFormatCtx.write_header();
	FFmpeg::AVPacket packet;
	while (!inputFormatCtx.read_packet(packet))
	{
		FFmpeg::AVStream input_stream = inputFormatCtx.get_stream(packet()->stream_index);
		// 如果映射表中的目标索引号是负数，表示此流不被复制到输出格式中
		if (stream_map[packet()->stream_index] >= 0)
		{
			// 将此包的流索引号改为目标流索引号，这样这个包就会被写入输出格式的中的目标索引号
			// 的流中
			packet()->stream_index = stream_map[packet()->stream_index];
			FFmpeg::AVStream output_stream = outputFormatCtx.get_stream(packet()->stream_index);
			packet.av_packet_rescale_ts(input_stream()->time_base, output_stream()->time_base);
			packet()->pos = -1;
			outputFormatCtx.interleaved_write_packet(packet);
		}
		packet.unref();
	}

	delete[] stream_map;
}
