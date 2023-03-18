// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
