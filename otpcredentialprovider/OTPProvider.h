#pragma once

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <map>
#include <iostream>
#include <ctime>
#include <fstream>
//#include <WinBase.h>

bool GetPinCode(wchar_t * userName, int nonce, wchar_t * pinCode1, wchar_t * pinCode2, wchar_t * pinCode3);
int getRandNum();

// 以下为SHA-1实现

#ifndef _SHA1_H_
#define _SHA1_H_
typedef struct SHA1Context {
	unsigned Message_Digest[5];
	unsigned Length_Low;
	unsigned Length_High;
	unsigned char Message_Block[64];
	int Message_Block_Index;
	int Computed;
	int Corrupted;
} SHA1Context;

void SHA1Reset(SHA1Context *);
int SHA1Result(SHA1Context *);
void SHA1Input(SHA1Context *, const char *, unsigned);
#endif


#define SHA1CircularShift(bits,word) ((((word) << (bits)) & 0xFFFFFFFF) | ((word) >> (32-(bits))))

void SHA1ProcessMessageBlock(SHA1Context *);
void SHA1PadMessage(SHA1Context *);

int sha1_hash(const char *source, wchar_t *lrvar);
