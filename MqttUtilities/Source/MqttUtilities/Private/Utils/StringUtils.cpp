// Copyright (c) 2019 Nineva Studios

#include "StringUtils.h"

char* StringUtils::CopyString(FString str)
{
	auto strAnsi = StringCast<ANSICHAR>(*str);
	const char* strPtr = strAnsi.Get();

	char* strCopy;
	size_t strLen;

	if (!strPtr)
	{
		return nullptr;
	}

	strLen = strlen(strPtr) + 1;

	if (nullptr == (strCopy = (char*)malloc(strLen)))
	{
		return nullptr;
	}

	memcpy(strCopy, strPtr, strLen);

	return strCopy;
}
