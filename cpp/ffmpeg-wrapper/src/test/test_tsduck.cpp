#include "test_tsduck.h"
#include<FileStream.h>
#include<PatParser.h>
#include<filesystem>
#include<tsCerrReport.h>

using namespace std;
using namespace video;

void test_tsduck()
{
	std::filesystem::current_path(Predefine_ResourceDir);

	// 输入文件
	ts::TSFile in_tsfile{};
	bool ret = in_tsfile.openRead(u"不老梦.ts", 0, ts::CerrReport::Instance());
	if (!ret)
	{
		cout << "打开输入文件失败" << endl;
		return;
	}

	cout << in_tsfile.getFileName() << endl;
	PatParser pat_handler;
	ts::TSPacket packet;
	while (1)
	{
		size_t have_read = in_tsfile.readPackets(&packet, nullptr, 1, ts::CerrReport::Instance());
		if (have_read == 0)
		{
			break;
		}

		pat_handler.SendPacket(&packet);
	}
}
