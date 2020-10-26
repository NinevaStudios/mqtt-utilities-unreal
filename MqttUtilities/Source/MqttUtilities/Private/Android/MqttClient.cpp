// Copyright (c) 2019 Nineva Studios

#include "MqttClient.h"

#include "Async/Async.h"
#include "MqttConnectionData.h"
#include "MqttMessage.h"

void UMqttClient::PostInitProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("MQTTCLIENT => UMqttClient::PostInitProperties()"));

	Super::PostInitProperties();

	if (UMqttClient::StaticClass()->GetDefaultObject() != this)
	{
		// add to root to avoid GC issues
		AddToRoot();
	}
}

void UMqttClient::BeginDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("MQTTCLIENT => UMqttClient::BeginDestroy()"));

	if (UMqttClient::StaticClass()->GetDefaultObject() != this)
	{
		// remove from root so now it can be safely destroyed by GC
		RemoveFromRoot();
	}

	Super::BeginDestroy();
}

void UMqttClient::Init(FMqttClientConfig configData)
{
	auto url = FJavaClassObject::GetJString(configData.HostUrl);
	auto clientId = FJavaClassObject::GetJString(configData.ClientId);
	MqttHelperJavaObject = MakeShareable(new FJavaClassObject("com/ninevastudios/mqttdemo/MqttHelper",
		"(Landroid/app/Activity;Ljava/lang/String;ILjava/lang/String;J)V", FJavaWrapper::GameActivityThis,
		*url, configData.Port, *clientId, (jlong)this));

	ConnectMethod = MqttHelperJavaObject->GetClassMethod("connect", "(Ljava/lang/String;Ljava/lang/String;)V");
	DisconnectMethod = MqttHelperJavaObject->GetClassMethod("disconnect", "(I)V");
	SubscribeMethod = MqttHelperJavaObject->GetClassMethod("subscribeToTopic", "(Ljava/lang/String;I)V");
	UnsubscribeMethod = MqttHelperJavaObject->GetClassMethod("unsubscribeFromTopic", "(Ljava/lang/String;)V");
	PublishMethod = MqttHelperJavaObject->GetClassMethod("publishMessage", "(Ljava/lang/String;Ljava/lang/String;IZ)V");
}

void UMqttClient::Connect(FMqttConnectionData connectionData, const FOnConnectDelegate& onConnectCallback)
{
	OnConnectDelegate = onConnectCallback;

	auto login = FJavaClassObject::GetJString(connectionData.Login);
	auto password = FJavaClassObject::GetJString(connectionData.Password);
	MqttHelperJavaObject->CallMethod<void>(ConnectMethod, *login, *password);
}

void UMqttClient::Disconnect(const FOnDisconnectDelegate& onDisconnectCallback)
{
	OnDisconnectDelegate = onDisconnectCallback;

	MqttHelperJavaObject->CallMethod<void>(DisconnectMethod, 0);
}

void UMqttClient::Subscribe(FString topic, int qos)
{
	if (topic.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("MQTT => Topic is not set. Subscription cancelled."));
		return;
	}

	auto jtopic = FJavaClassObject::GetJString(topic);
	MqttHelperJavaObject->CallMethod<void>(SubscribeMethod, *jtopic, qos);
}

void UMqttClient::Unsubscribe(FString topic)
{
	if (topic.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("MQTT => Topic is not set. Unsubscription cancelled."));
		return;
	}

	auto jtopic = FJavaClassObject::GetJString(topic);
	MqttHelperJavaObject->CallMethod<void>(UnsubscribeMethod, *jtopic);
}

void UMqttClient::Publish(FMqttMessage message)
{
	auto jtopic = FJavaClassObject::GetJString(message.Topic);
	auto jmessage = FJavaClassObject::GetJString(message.Message);
	MqttHelperJavaObject->CallMethod<void>(PublishMethod, *jtopic, *jmessage, message.Qos, message.Retain);
}

void UMqttClient::OnConnect()
{
	OnConnectDelegate.ExecuteIfBound();
}

void UMqttClient::OnDisconnect()
{
	OnDisconnectDelegate.ExecuteIfBound();
}

void UMqttClient::OnSubscribe(int messageId, TArray<int> qos)
{
	OnSubscribeDelegate.ExecuteIfBound(messageId, qos);
}

void UMqttClient::OnUnsubscribe(int messageId)
{
	OnUnsubscribeDelegate.ExecuteIfBound(messageId);
}

void UMqttClient::OnPublish(int messageId)
{
	OnPublishDelegate.ExecuteIfBound(messageId);
}

void UMqttClient::OnMessage(FMqttMessage message)
{
	OnMessageDelegate.ExecuteIfBound(message);
}

void UMqttClient::OnError(int errCode, FString message)
{
	OnErrorDelegate.ExecuteIfBound(errCode, message);
}

JNI_METHOD void Java_com_ninevastudios_mqttdemo_MqttHelper_onConnected(JNIEnv* env, jclass clazz, jlong objAddr)
{
	UE_LOG(LogTemp, Warning, TEXT("MQTTCLIENT => Java_com_ninevastudios_mqttdemo_MqttHelper_onConnected"));

	UMqttClient* client = reinterpret_cast<UMqttClient*>(objAddr);
	if (client)
	{
		AsyncTask(ENamedThreads::GameThread, [=]() {
			client->OnConnect();
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_mqttdemo_MqttHelper_onDisconnected(JNIEnv* env, jclass clazz, jlong objAddr)
{
	UE_LOG(LogTemp, Warning, TEXT("MQTTCLIENT => Java_com_ninevastudios_mqttdemo_MqttHelper_onDisconnected"));

	UMqttClient* client = reinterpret_cast<UMqttClient*>(objAddr);
	if (client)
	{
		AsyncTask(ENamedThreads::GameThread, [=]() {
			client->OnDisconnect();
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_mqttdemo_MqttHelper_onSubscribed(JNIEnv* env, jclass clazz,
	jlong objAddr, int messageId, int qos)
{
	UE_LOG(LogTemp, Warning, TEXT("MQTTCLIENT => Java_com_ninevastudios_mqttdemo_MqttHelper_onSubscribed"));

	UMqttClient* client = reinterpret_cast<UMqttClient*>(objAddr);
	if (client)
	{
		TArray<int> qosArray = { qos };
		AsyncTask(ENamedThreads::GameThread, [=]() {
			client->OnSubscribe(messageId, qosArray);
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_mqttdemo_MqttHelper_onUnsubscribed(JNIEnv* env, jclass clazz,
	jlong objAddr, int messageId)
{
	UE_LOG(LogTemp, Warning, TEXT("MQTTCLIENT => Java_com_ninevastudios_mqttdemo_MqttHelper_onUnsubscribed"));

	UMqttClient* client = reinterpret_cast<UMqttClient*>(objAddr);
	if (client)
	{
		AsyncTask(ENamedThreads::GameThread, [=]() {
			client->OnUnsubscribe(messageId);
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_mqttdemo_MqttHelper_onPublished(JNIEnv* env, jclass clazz,
	jlong objAddr, int messageId)
{
	UE_LOG(LogTemp, Warning, TEXT("MQTTCLIENT => Java_com_ninevastudios_mqttdemo_MqttHelper_onPublished"));

	UMqttClient* client = reinterpret_cast<UMqttClient*>(objAddr);
	if (client)
	{
		AsyncTask(ENamedThreads::GameThread, [=]() {
			client->OnPublish(messageId);
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_mqttdemo_MqttHelper_onMessageArrived(JNIEnv* env, jclass clazz,
	jlong objAddr, jstring topic, jstring payload, int qos, bool retain)
{
	UE_LOG(LogTemp, Warning, TEXT("MQTTCLIENT => Java_com_ninevastudios_mqttdemo_MqttHelper_onMessageArrived"));

	UMqttClient* client = reinterpret_cast<UMqttClient*>(objAddr);
	if (client)
	{
		FMqttMessage message;
		message.Topic = FJavaHelper::FStringFromParam(env, topic);
		message.Message = FJavaHelper::FStringFromParam(env, payload);
		message.Qos = qos;
		message.Retain = retain;
		AsyncTask(ENamedThreads::GameThread, [=]() {
			client->OnMessage(message);
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_mqttdemo_MqttHelper_onError(JNIEnv* env, jclass clazz,
	jlong objAddr, int errCode, jstring errMessage)
{
	UE_LOG(LogTemp, Warning, TEXT("MQTTCLIENT => Java_com_ninevastudios_mqttdemo_MqttHelper_onError"));

	UMqttClient* client = reinterpret_cast<UMqttClient*>(objAddr);
	if (client)
	{
		FString ErrorMessage = FJavaHelper::FStringFromParam(env, errMessage);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			client->OnError(errCode, ErrorMessage);
		});
	}
}
