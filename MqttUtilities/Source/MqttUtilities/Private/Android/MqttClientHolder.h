// Copyright (c) 2019 Nineva Studios

#pragma once

#include "MqttClient.h"

class MqttClientHolder
{
	MqttClientHolder();

public:

	static MqttClientHolder* GetInstance();

	void AddClient(FGuid guid, UMqttClient* client);
	void RemoveClient(FGuid guid);

	UMqttClient* GetClient(FGuid guid) const;

private:

	TMap<FGuid, UMqttClient*> Clients;
};