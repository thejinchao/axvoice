#include "stdafx.h"
#include "AV_Util.h"

#include "md5.h"

//------------------------------------------------------------------------------------------------------------------
std::string md5File(const char* szFileName)
{
	FILE* fp = 0;
	fopen_s(&fp, szFileName, "rb");
	if(fp==0) return "";

	MD5_CTX ctx;
	MD5Init(&ctx);

	do
	{
		const int READ_BUFFER = 1024;

		unsigned char szTemp[READ_BUFFER]={0};
		int readSize = (int)fread(szTemp, 1, READ_BUFFER, fp);
		if(readSize==0) break;

		MD5Update(&ctx, szTemp, readSize);

	}while(true);
	fclose(fp);

	unsigned char md5_code[16];
	MD5Final(&ctx, md5_code);

	char chBuffer[128]={0};
	for(register int nCount = 0; nCount < 16; nCount++)
	{
		StringCbPrintfA(chBuffer+nCount*2, 128-nCount*2, "%02x", md5_code[nCount]);
	}

	return std::string(chBuffer);
}
