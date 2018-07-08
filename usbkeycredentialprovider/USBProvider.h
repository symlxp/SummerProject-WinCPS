#pragma once

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <map>
#include <iostream>
#include <ctime>
#include <fstream>
#include <Windows.h>

std::wstring StringToWstring(const std::string str);

std::string WstringToString(const std::wstring str);

void Init();

bool GetUDiskRoot(std::string &uDiskRoot);

int getRandNum();

bool USBKeyCertificate(wchar_t *usbKeyPassword);