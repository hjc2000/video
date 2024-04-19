#include<CLI/CLI.hpp>
#include<ffmpeg-wrapper/mux/AVMixer.h>
#include<ffmpeg-wrapper/output-format/FileOutputFormat.h>

using namespace video;
using namespace jc;

int main(int argc, char **argv)
{
	try
	{
		std::string input_url;

		CLI::App app{ "检查输入格式是否有音频流和视频流。可以选择检查视频流或检查音频流。但不能同时选择检查。" };
		CLI::Option *option_video = app.add_flag(
			"--video",
			"检查是否有视频流。"
		);

		CLI::Option *option_audio = app.add_flag(
			"--audio",
			"检查是否有音频流。"
		);

		CLI::Option *option_input_url = app.add_option(
			"-i",
			input_url,
			"输入的媒体格式的 url。"
		)
			->required();

		app.callback([&]()
		{
			if (option_video->count() + option_audio->count() != 1)
			{
				throw CLI::ParseError("必须且只能指定一个 --video 或 --audio 选项。", CLI::ExitCodes::ExtrasError);
			}
		});

		// 解析命令行参数
		CLI11_PARSE(app, argc, argv);

		shared_ptr<InputFormat> input_format{ new InputFormat{input_url} };
		if (option_video->count())
		{
			AVStreamWrapper stream = input_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_VIDEO);
			if (stream)
			{
				cout << "true" << endl;
			}
			else
			{
				cout << "false" << endl;
			}

			return 0;
		}

		if (option_audio->count())
		{
			AVStreamWrapper stream = input_format->FindBestStream(AVMediaType::AVMEDIA_TYPE_AUDIO);
			if (stream)
			{
				cout << "true" << endl;
			}
			else
			{
				cout << "false" << endl;
			}

			return 0;
		}

		return -1;
	}
	catch (jc::Exception &e)
	{
		cout << e.what() << endl;
		throw;
	}
}
