#include<CLI/CLI.hpp>
#include<jccpp/Exception.h>
#include<jccpp/stream/FileStream.h>
#include<TSDumper.h>
#include<tsduck/io/TSPacketStreamReader.h>
#include<iostream>

using namespace video;
using namespace std;

int main(int argc, char **argv)
{
	try
	{
		string input_file_path, output_file_path;
		CLI::App app{ "输出 ts 文件的包、表格的信息到指定的文本文件中。" };

		// 定义输入文件选项
		app.add_option(
			"-i,--input",
			input_file_path,
			"输入的 ts 文件"
		)
			->required()
			->check(CLI::ExistingFile); // 确保输入文件存在

		// 定义输出文件选项
		app.add_option(
			"-o,--output",
			output_file_path,
			"输出的文本文件"
		)
			->required();

		// 解析命令行参数
		CLI11_PARSE(app, argc, argv);

		// 执行到这里就是解析命令行成功，并且命令行参数不是 -h,--help
		cout << "正在解析 ts" << endl;

		shared_ptr<FileStream> input_file_stream = FileStream::Open(input_file_path);
		TSPacketStreamReader reader{ input_file_stream };
		shared_ptr<TSDumper> ts_dumper{ new TSDumper{output_file_path} };
		CancellationTokenSource cancel_pump_source;
		reader.PumpTo(ts_dumper, cancel_pump_source.Token());
		ts_dumper->DisplayStatisticalResults();
	}
	catch (std::exception &e)
	{
		cerr << e.what() << endl;
	}

	return 0;
}
