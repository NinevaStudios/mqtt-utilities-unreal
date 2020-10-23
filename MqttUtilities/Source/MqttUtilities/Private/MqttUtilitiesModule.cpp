// Copyright (c) 2019 Nineva Studios

#include "MqttUtilitiesModule.h"

#include "Engine/Engine.h"
#include "HAL/PlatformProcess.h"
#include "ISettingsModule.h"
#include "Interfaces/IPluginManager.h"
#include "MqttUtilitiesSettings.h"

#define LOCTEXT_NAMESPACE "MqttUtilities"

IMPLEMENT_MODULE(FMqttUtilitiesModule, MqttUtilities)

void FMqttUtilitiesModule::StartupModule()
{
	MqttUtilitiesSettings = NewObject<UMqttUtilitiesSettings>(GetTransientPackage(), "MqttUtilitiesSettings", RF_Standalone);
	MqttUtilitiesSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "MqttUtilities",
			LOCTEXT("RuntimeSettingsName", "MQTT Utilities"),
			LOCTEXT("RuntimeSettingsDescription", "Configure MQTT Utilities"),
			MqttUtilitiesSettings);
	}

#if PLATFORM_WINDOWS

	const FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("MqttUtilities"))->GetBaseDir();
	const FString DLLPath = PluginDir / TEXT("Binaries/Win64/");

	FPlatformProcess::PushDllDirectory(*DLLPath);

	mDllHandleMosquitto = FPlatformProcess::GetDllHandle(*(DLLPath + "mosquitto.dll"));
	mDllHandleMosquittopp = FPlatformProcess::GetDllHandle(*(DLLPath + "mosquittopp.dll"));

	FPlatformProcess::PopDllDirectory(*DLLPath);

#endif

#if PLATFORM_MAC

	const FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("MqttUtilities"))->GetBaseDir();
	const FString DLLPath = PluginDir / TEXT("Binaries/Mac/");

	FPlatformProcess::PushDllDirectory(*DLLPath);

	mDllHandleMosquitto = FPlatformProcess::GetDllHandle(*(DLLPath + "mosquitto.dylib"));
	mDllHandleMosquittopp = FPlatformProcess::GetDllHandle(*(DLLPath + "mosquittopp.dylib"));

	FPlatformProcess::PopDllDirectory(*DLLPath);

#endif
}

void FMqttUtilitiesModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "MqttUtilities");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		MqttUtilitiesSettings->RemoveFromRoot();
	}
	else
	{
		MqttUtilitiesSettings = nullptr;
	}

#if PLATFORM_WINDOWS || PLATFORM_MAC

	if (mDllHandleMosquitto)
	{
		FPlatformProcess::FreeDllHandle(mDllHandleMosquitto);
		mDllHandleMosquitto = nullptr;
	}

	if (mDllHandleMosquittopp)
	{
		FPlatformProcess::FreeDllHandle(mDllHandleMosquittopp);
		mDllHandleMosquittopp = nullptr;
	}

#endif
}

#undef LOCTEXT_NAMESPACE
