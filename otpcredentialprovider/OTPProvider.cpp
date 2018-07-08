#include "OTPProvider.h"

//#define DEBUG

std::map<std::wstring, std::wstring>userSecrets;

std::wstring StringToWstring(const std::string str)
{
	unsigned len = str.size() * 2;// 预留字节数
	setlocale(LC_CTYPE, "");     //必须调用此函数
	wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
	mbstowcs(p, str.c_str(), len);// 转换
	std::wstring str1(p);
	delete[] p;// 释放申请的内存
	return str1;
}

std::string WstringToString(const std::wstring str)
{
	unsigned len = str.size() * 4;
	setlocale(LC_CTYPE, "");
	char *p = new char[len];
	wcstombs(p, str.c_str(), len);
	std::string str1(p);
	delete[] p;
	return str1;
}


void Init() {
	freopen("C:\\Windows\\System32\\usersecret.txt", "r", stdin);

	int userNum;
	std::cin >> userNum;

	std::string userName, userSecret;

	for (size_t i = 0; i < userNum; i++)
	{
		std::cin >> userName >> userSecret;
		userSecrets[StringToWstring(userName)] = StringToWstring(userSecret);
	}

}

bool GetPinCode(wchar_t * userName, int nonce, wchar_t * pinCode1, wchar_t * pinCode2, wchar_t * pinCode3)
{

	if (userSecrets.size()==0)
	{
		Init();
	}


	if (userSecrets.size() == 0)
	{
		return false;
	}

	std::wstring userSecret = userSecrets[std::wstring(userName)];
	if (userSecret.length() == 0) {
		return false;
	}

	time_t currentTime = time(nullptr);
	currentTime /= 30;

	char tempArr[200];
	sprintf(tempArr, "%s%d%d", WstringToString(userSecret).c_str(), currentTime, nonce);

#ifdef DEBUG
	std::ofstream outs;
	outs.open("C:\\cps-log.txt", std::ios::app);
#endif // DEBUG

	wchar_t sha1Result[100];
	sha1_hash(tempArr, sha1Result);

	sprintf(tempArr, "%s%s", WstringToString(userSecret).c_str(), WstringToString(std::wstring(sha1Result)).c_str());

#ifdef DEBUG
	outs << tempArr << std::endl;
#endif // DEBUG

	sha1_hash(tempArr, sha1Result);

#ifdef DEBUG
	outs << WstringToString(std::wstring(sha1Result))<<std::endl;
#endif // DEBUG

	//wchar_t pinCode[9];

	wcsncpy(pinCode1, sha1Result, 8);
	pinCode1[8] = '\0';

#ifdef DEBUG
	outs << "--------------------------------------" << std::endl;
	outs << WstringToString(std::wstring(pinCode1)) << std::endl;
#endif // DEBUG

	sprintf(tempArr, "%s%d%d", WstringToString(userSecret).c_str(), currentTime - 1, nonce);
	sha1_hash(tempArr, sha1Result);
	sprintf(tempArr, "%s%s", WstringToString(userSecret).c_str(), WstringToString(std::wstring(sha1Result)).c_str());
	sha1_hash(tempArr, sha1Result);
	wcsncpy(pinCode2, sha1Result, 8);
	pinCode2[8] = '\0';

	sprintf(tempArr, "%s%d%d", WstringToString(userSecret).c_str(), currentTime + 1, nonce);
	sha1_hash(tempArr, sha1Result);
	sprintf(tempArr, "%s%s", WstringToString(userSecret).c_str(), WstringToString(std::wstring(sha1Result)).c_str());
	sha1_hash(tempArr, sha1Result);
	wcsncpy(pinCode3, sha1Result, 8);
	pinCode3[8] = '\0';

#ifdef DEBUG
	outs << WstringToString(std::wstring(pinCode3)) << std::endl;
	outs << WstringToString(std::wstring(pinCode2)) << std::endl;
	outs << "--------------------------------------" << std::endl;
#endif // DEBUG


	return true;
}

int randNumForOTP = 0;

int getRandNum()
{

	if (!randNumForOTP) {
		srand(time(NULL));
	}

	randNumForOTP = 0;

	for (size_t i = 0; i < 8; i++)
	{
		randNumForOTP *= 10;
		randNumForOTP += rand() % 10;
	}

	return randNumForOTP;
}

// 以下为SHA-1实现

void SHA1Reset(SHA1Context *context) {// 初始化动作
	context->Length_Low = 0;
	context->Length_High = 0;
	context->Message_Block_Index = 0;

	context->Message_Digest[0] = 0x67452301;
	context->Message_Digest[1] = 0xEFCDAB89;
	context->Message_Digest[2] = 0x98BADCFE;
	context->Message_Digest[3] = 0x10325476;
	context->Message_Digest[4] = 0xC3D2E1F0;

	context->Computed = 0;
	context->Corrupted = 0;
}


int SHA1Result(SHA1Context *context) {// 成功返回1，失败返回0
	if (context->Corrupted) {
		return 0;
	}
	if (!context->Computed) {
		SHA1PadMessage(context);
		context->Computed = 1;
	}
	return 1;
}


void SHA1Input(SHA1Context *context, const char *message_array, unsigned length) {
	if (!length) return;

	if (context->Computed || context->Corrupted) {
		context->Corrupted = 1;
		return;
	}

	while (length-- && !context->Corrupted) {
		context->Message_Block[context->Message_Block_Index++] = (*message_array & 0xFF);

		context->Length_Low += 8;

		context->Length_Low &= 0xFFFFFFFF;
		if (context->Length_Low == 0) {
			context->Length_High++;
			context->Length_High &= 0xFFFFFFFF;
			if (context->Length_High == 0) context->Corrupted = 1;
		}

		if (context->Message_Block_Index == 64) {
			SHA1ProcessMessageBlock(context);
		}
		message_array++;
	}
}

void SHA1ProcessMessageBlock(SHA1Context *context) {
	const unsigned K[] = { 0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6 };
	int         t;
	unsigned    temp;
	unsigned    W[80];
	unsigned    A, B, C, D, E;

	for (t = 0; t < 16; t++) {
		W[t] = ((unsigned)context->Message_Block[t * 4]) << 24;
		W[t] |= ((unsigned)context->Message_Block[t * 4 + 1]) << 16;
		W[t] |= ((unsigned)context->Message_Block[t * 4 + 2]) << 8;
		W[t] |= ((unsigned)context->Message_Block[t * 4 + 3]);
	}

	for (t = 16; t < 80; t++)  W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);

	A = context->Message_Digest[0];
	B = context->Message_Digest[1];
	C = context->Message_Digest[2];
	D = context->Message_Digest[3];
	E = context->Message_Digest[4];

	for (t = 0; t < 20; t++) {
		temp = SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
		temp &= 0xFFFFFFFF;
		E = D;
		D = C;
		C = SHA1CircularShift(30, B);
		B = A;
		A = temp;
	}
	for (t = 20; t < 40; t++) {
		temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
		temp &= 0xFFFFFFFF;
		E = D;
		D = C;
		C = SHA1CircularShift(30, B);
		B = A;
		A = temp;
	}
	for (t = 40; t < 60; t++) {
		temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
		temp &= 0xFFFFFFFF;
		E = D;
		D = C;
		C = SHA1CircularShift(30, B);
		B = A;
		A = temp;
	}
	for (t = 60; t < 80; t++) {
		temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
		temp &= 0xFFFFFFFF;
		E = D;
		D = C;
		C = SHA1CircularShift(30, B);
		B = A;
		A = temp;
	}
	context->Message_Digest[0] = (context->Message_Digest[0] + A) & 0xFFFFFFFF;
	context->Message_Digest[1] = (context->Message_Digest[1] + B) & 0xFFFFFFFF;
	context->Message_Digest[2] = (context->Message_Digest[2] + C) & 0xFFFFFFFF;
	context->Message_Digest[3] = (context->Message_Digest[3] + D) & 0xFFFFFFFF;
	context->Message_Digest[4] = (context->Message_Digest[4] + E) & 0xFFFFFFFF;
	context->Message_Block_Index = 0;
}

void SHA1PadMessage(SHA1Context *context) {
	if (context->Message_Block_Index > 55) {
		context->Message_Block[context->Message_Block_Index++] = 0x80;
		while (context->Message_Block_Index < 64)  context->Message_Block[context->Message_Block_Index++] = 0;
		SHA1ProcessMessageBlock(context);
		while (context->Message_Block_Index < 56) context->Message_Block[context->Message_Block_Index++] = 0;
	}
	else {
		context->Message_Block[context->Message_Block_Index++] = 0x80;
		while (context->Message_Block_Index < 56) context->Message_Block[context->Message_Block_Index++] = 0;
	}
	context->Message_Block[56] = (context->Length_High >> 24) & 0xFF;
	context->Message_Block[57] = (context->Length_High >> 16) & 0xFF;
	context->Message_Block[58] = (context->Length_High >> 8) & 0xFF;
	context->Message_Block[59] = (context->Length_High) & 0xFF;
	context->Message_Block[60] = (context->Length_Low >> 24) & 0xFF;
	context->Message_Block[61] = (context->Length_Low >> 16) & 0xFF;
	context->Message_Block[62] = (context->Length_Low >> 8) & 0xFF;
	context->Message_Block[63] = (context->Length_Low) & 0xFF;

	SHA1ProcessMessageBlock(context);
}

int sha1_hash(const char *source, wchar_t *lrvar) 
{
	SHA1Context sha;
	wchar_t buf[128];

	SHA1Reset(&sha);
	SHA1Input(&sha, source, strlen(source));

	if (!SHA1Result(&sha)) {
		return -1;
	}
	else {
		memset(buf, 0, sizeof(buf));
		swprintf(buf, L"%08X%08X%08X%08X%08X", sha.Message_Digest[0], sha.Message_Digest[1],
			sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
		wcscpy(lrvar, buf);
		return wcslen(buf);
	}
}
