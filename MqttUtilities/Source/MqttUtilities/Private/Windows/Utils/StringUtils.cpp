// Copyright (c) 2019 Nineva Studios

#include "StringUtils.h"

char* StringUtils::CopyString(FString str)
{
	auto originalFString = StringCast<ANSICHAR>(*str);
	const char* originalStr = originalFString.Get();
	
	char *copyStr;
	size_t str_len;

	if (!originalStr)
	{
		return nullptr;
	}
	
	str_len = strlen(originalStr) + 1;
	
	if (nullptr == (copyStr = (char*)malloc(str_len)))
	{
		return nullptr;
	}	
	
	memcpy(copyStr, originalStr, str_len);

	return copyStr;
}
