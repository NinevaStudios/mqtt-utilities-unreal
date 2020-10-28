// Copyright (c) 2020 Nineva Studios

#pragma once

#include "Engine/EngineTypes.h"

#include "MqttUtilitiesSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class MQTTUTILITIES_API UMqttUtilitiesSettings : public UObject
{
	GENERATED_BODY()

public:
	/** 
	* Name of the certificate (with extension and base64 encoded) to use for connection validation.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Security | Desktop", Meta = (DisplayName = "Certificate Name"))
	FString DesktopCertificateName = "broker.crt";

	/**
	* Use custom security config. This will copy a certificate located in the Resources/AndroidCertificates folder of the plugin.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Security | Android")
	bool UseCustomSecurityConfig;

	/** 
	* Name of the certificate (no extension, only lower case letters, numbers, or underscores) to use as a raw resource for Android.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Security | Android", Meta = (DisplayName = "Certificate Name"))
	FString AndroidCertificateName = "broker_crt";

	/**
	* Enable iOS14 app tracking transparency framework.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Security | iOS")
	bool UseCertificatePinning;

	/**
	* Name of the certificate (with extension and der encoded) to use for connection validation.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Security | iOS", Meta = (DisplayName = "Certificate Name"))
	FString IOSCertificateName = "broker.der";

	/**
	* Whether or not to trust servers with an invalid or expired SSL certificates.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Security | iOS")
	bool AllowInvalidCertificates = true;

	/**
	* Whether to evaluate an entire SSL certificate chain, or just the leaf certificate.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Security | iOS")
	bool ValidatesCertificateChain = false;

	/**
	* Whether or not to validate the domain name in the certificate's CN field.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Security | iOS")
	bool ValidatesDomainName = false;
};
