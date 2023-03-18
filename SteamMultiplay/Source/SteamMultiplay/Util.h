// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <format>

namespace std
{
	template<>
	struct formatter<FName> : formatter<string_view>
	{
		template<typename FormatContext>
		auto format(const FName& s, FormatContext& ctx)
		{
			std::string str = TCHAR_TO_UTF8(*s.ToString());
			return format_to(ctx.out(), "{}", str);
		}
	};

	template<>
	struct formatter<FString> : formatter<string_view>
	{
		template<typename FormatContext>
		auto format(const FString& s, FormatContext& ctx)
		{
			std::string str = TCHAR_TO_UTF8(*s);
			return format_to(ctx.out(), "{}", str);
		}
	};
}

/**
 * 
 */
class STEAMMULTIPLAY_API Util
{
public:
	Util();
	~Util();

	static void Log(const std::string& Message);
	static void LogDisplay(const std::string& Message, const int LogTime = 15);
	static void LogDisplay(const std::wstring& Message, const int LogTime = 15);
};
