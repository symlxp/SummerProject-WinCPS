#include "USBProvider.h"
#define DEBUG

std::map<std::string, std::string>userKeyPath;

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
	freopen("C:\\WinCPS\\Keys\\userKeyList.txt", "r", stdin);

	int userNum;
	std::cin >> userNum;

	std::string userName, keyPath;

	for (size_t i = 0; i < userNum; i++)
	{
		std::cin >> userName >> keyPath;
		userKeyPath[userName] = keyPath;
	}

}

bool GetUDiskRoot(std::string &uDiskRoot)
{
	UINT diskType;
	size_t szAllDriveStr = GetLogicalDriveStrings(0, NULL);
	wchar_t *pDriveStr = new wchar_t[szAllDriveStr + 1];
	wchar_t *pForDelete = pDriveStr;
	GetLogicalDriveStrings(szAllDriveStr, pDriveStr);
	size_t szDriveStr = wcslen(pDriveStr);

	while (szDriveStr > 0)
	{
		diskType = GetDriveType(pDriveStr);
		if (diskType == DRIVE_REMOVABLE) {
			uDiskRoot = WstringToString(std::wstring(pDriveStr));
			std::ifstream fileIn(uDiskRoot + "certificate.exe");
			if (fileIn) {
				delete pForDelete;
				return true;
			}
		}

		pDriveStr += szDriveStr + 1;
		szDriveStr = wcslen(pDriveStr);
	}

	delete pForDelete;
	return false;
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

bool USBKeyCertificate(wchar_t *usbKeyPassword) {
	
	if (userKeyPath.size() == 0)
	{
		Init();
	}

	if (userKeyPath.size() == 0)
	{
		return false;
	}

#ifdef DEBUG
	std::ofstream fOut("C:\\cps-log.txt");
	fOut << "get userKeyPath" << std::endl;
#endif // DEBUG

	std::string usbKeyRoot;
	if (!GetUDiskRoot(usbKeyRoot)) {
		return false;
	}

#ifdef DEBUG
	fOut << "get usbkeyroot:" << usbKeyRoot << std::endl;
#endif // DEBUG

	std::string userName;
	//std::string userPassword;

	std::ifstream fileIn;
	fileIn.open(usbKeyRoot + "username.txt");
	fileIn >> userName;
	fileIn.close();

#ifdef DEBUG
	fOut << "get username:" << userName << std::endl;
#endif // DEBUG

	std::ofstream fileOut("C:\\WinCPS\\Temp\\usb-nonce.txt");
	int nonce = getRandNum();
	fileOut << nonce;
	fileOut.close();

	system(("C:\\WinCPS\\OpenSSL\\bin\\openssl.exe rsautl -encrypt -in C:\\WinCPS\\Temp\\usb-nonce.txt -inkey " + userKeyPath[userName] + " -pubin -out C:\\WinCPS\\Temp\\usb-nonce.en").c_str());
#ifdef DEBUG
	fOut << "creat nonce.en with:" << "openssl rsautl -encrypt -in C:\\WinCPS\\Temp\\usb-nonce.txt -inkey " + userKeyPath[userName] + " -pubin -out C:\\WinCPS\\Temp\\usb-nonce.en" << std::endl;
#endif // DEBUG

	system((usbKeyRoot + "certificate.exe "+ WstringToString(std::wstring(usbKeyPassword))+" "+usbKeyRoot).c_str());
#ifdef DEBUG
	fOut << "usb certificate with:" << usbKeyRoot + "certificate.exe " + WstringToString(std::wstring(usbKeyPassword)) + " " + usbKeyRoot << std::endl;
#endif // DEBUG

	system(("C:\\WinCPS\\OpenSSL\\bin\\openssl.exe pkeyutl -verify -in C:\\WinCPS\\Temp\\usb-response.txt -sigfile C:\\WinCPS\\Temp\\usb-response.sig -pubin -inkey "+ userKeyPath[userName]+" > C:\\WinCPS\\Temp\\usb-sig-result.txt").c_str());
#ifdef DEBUG
	fOut << "pc certificate with:" << "openssl pkeyutl -verify -in C:\\WinCPS\\Temp\\usb-response.txt -sigfile C:\\WinCPS\\Temp\\usb-response.sig -pubin -inkey " + userKeyPath[userName] + " > C:\\WinCPS\\Temp\\usb-sig-result.txt" << std::endl;
#endif // DEBUG

	fileIn.open("C:\\WinCPS\\Temp\\usb-sig-result.txt");
	std::string sigResult;
	fileIn >> sigResult;
	fileIn >> sigResult;
	fileIn >> sigResult;
	fileIn.close();
	
	if (sigResult == "Successfully") {

		system("del C:\\WinCPS\\Temp\\usb-sig-result.txt");
		system("del C:\\WinCPS\\Temp\\usb-nonce.txt");
		system("del C:\\WinCPS\\Temp\\usb-nonce.en");
		system("del C:\\WinCPS\\Temp\\usb-response.txt");
		system("del C:\\WinCPS\\Temp\\usb-response.sig");
		system("del C:\\WinCPS\\Temp\\usb-sig-result.txt");
		system(("del "+usbKeyRoot+"nonce.txt").c_str());

		return true;
	}

#ifdef DEBUG
	fOut.close();
#endif // DEBUG


	return false;
}
