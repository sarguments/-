#include "stdafx.h"
#include "hoxy_Header.h"
#include "hoxy_CmdStart.h"

CCmdStart::CCmdStart()
{
	// 랜덤 시드
	srand((unsigned int)time(NULL));

	// 콘솔 창 띄우기
	AllocConsole();
	FILE *acStreamOut;
	freopen_s(&acStreamOut, "CONOUT$", "wt", stdout);

	CCmdStart::CmdDebugText(L"CMD START Init", true);
}

CCmdStart::~CCmdStart()
{
}

void CCmdStart::CmdDebugText(const WCHAR* text, bool bParam)
{
	if (bParam)
	{
		wcout << std::setw(30) << text << std::setw(50) << L"|OK|" << endl;
	}
	else
	{
		int errorNum = GetLastError();
		wcout << std::setw(30) << text << std::setw(40) << L"|ERROR| : " << std::setw(10) << errorNum << endl;

		// 파일 생성
		FILE* fp;
		errno_t err = _wfopen_s(&fp, L"debug.txt", L"a+t");
		if (err != 0)
		{
			CCmdStart::CmdDebugText(L"file open error", false);
			exit(1);
		}

		fwprintf(fp, L"ERROR : %d // %ws\n", errorNum, text);
		fclose(fp);
	}
}