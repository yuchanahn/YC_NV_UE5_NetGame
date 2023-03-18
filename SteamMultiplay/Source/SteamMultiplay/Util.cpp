// Fill out your copyright notice in the Description page of Project Settings.


#include "Util.h"

Util::Util()
{
}

Util::~Util()
{
}

void Util::Log(const std::string& Message) {
	UE_LOG(LogTemp, Warning, TEXT("%hs"), Message.c_str());
}

void Util::LogDisplay(const std::string& Message, const int LogTime) {
	GEngine->AddOnScreenDebugMessage(-1, LogTime, FColor::Red, Message.c_str());
}

void Util::LogDisplay(const std::wstring& Message, const int LogTime) {
	GEngine->AddOnScreenDebugMessage(-1, LogTime, FColor::Red, Message.c_str());
}
