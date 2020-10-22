// Copyright (c) 2019 Nineva Studios

#pragma once

#include "MqttClientBase.h"

#if PLATFORM_IOS
#import <MQTTClient/MQTTClient.h>
#endif

#include "MqttClient.generated.h"

UCLASS()
class UMqttClient : public UMqttClientBase
{
	GENERATED_BODY()

public:

	virtual ~UMqttClient();

	void Connect(FMqttConnectionData connectionData, const FOnConnectDelegate& onConnectCallback) override;

	void Disconnect(const FOnDisconnectDelegate& onDisconnectCallback) override;

	void Subscribe(FString topic, int qos) override;

	void Unsubscribe(FString topic) override;

	void Publish(FMqttMessage message) override;

public:

	void Init(FMqttClientConfig configData) override;

private:
	
	FMqttClientConfig config;
	MQTTSession* mqttSession;
};
