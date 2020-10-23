// Copyright (c) 2020 Nineva Studios

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UMqttUtilitiesSettings;

class FMqttUtilitiesModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * Singleton-like access to this module's interface. This is just for convenience!
	 * Beware of calling this during the shutdown phase, though. Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance
	 */
	static inline FMqttUtilitiesModule& Get()
	{
		return FModuleManager::Get().GetModuleChecked<FMqttUtilitiesModule>("MqttUtilities");
	}

	/**
	 * Checks to see if this module is loaded and ready. It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("MqttUtilities");
	}

	UMqttUtilitiesSettings* GetSettings() const { return MqttUtilitiesSettings; }

private:
	UMqttUtilitiesSettings* MqttUtilitiesSettings;

	void* mDllHandleMosquitto;
	void* mDllHandleMosquittopp;
};
