// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraClone.h"
#include "Modules/ModuleManager.h"
#include "LCLog.h"

class FLCGameModule : public FDefaultGameModuleImpl
{
public:

	// IModuleInterface내 함수들 상속
	virtual void StartupModule() override; 
	virtual void ShutdownModule() override;

};


void FLCGameModule::StartupModule()
{
	UE_LOG(LogLC, Warning, TEXT("Startup Moudle"));
}

void FLCGameModule::ShutdownModule()
{
	UE_LOG(LogLC, Warning, TEXT("Shutdown Moudle"));
}



IMPLEMENT_PRIMARY_GAME_MODULE(FLCGameModule, LyraClone, "LyraClone" );