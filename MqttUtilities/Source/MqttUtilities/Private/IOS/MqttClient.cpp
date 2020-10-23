// Copyright (c) 2019 Nineva Studios

#include "MqttClient.h"

#include "Async/Async.h"
#include "MqttDelegate.h"
#include "MqttUtilitiesSettings.h"
#include "MqttUtilitiesModule.h"
#include "Utils/ConversionUtils.h"

UMqttClient::~UMqttClient()
{
	if (mqttSession != nil)
	{
		(MQTTSession*)CFBridgingRelease(mqttSession);
		mqttSession = nil;
	}
}

void UMqttClient::Connect(FMqttConnectionData connectionData, const FOnConnectDelegate& onConnectCallback)
{
	OnConnectDelegate = onConnectCallback;

	if (connectionData.bUseTLS)
	{
		UMqttUtilitiesSettings* Settings = FMqttUtilitiesModule::Get().GetSettings();

		MQTTSSLSecurityPolicy* securityPolicy = [MQTTSSLSecurityPolicy policyWithPinningMode:MQTTSSLPinningModeNone];
		MQTTSSLSecurityPolicyTransport* transport = [[MQTTSSLSecurityPolicyTransport alloc] init];
		transport.host = config.HostUrl.GetNSString();
		transport.port = config.Port;
		transport.tls = YES;
		if (Settings->UseCertificatePinning)
		{
			securityPolicy.SSLPinningMode = MQTTSSLPinningModeCertificate;

			// Certificate must be DER encoded
			FString CertificatePath = FPaths::ProjectContentDir();
			CertificatePath /= "Certificates";
			CertificatePath /= Settings->IOSCertificateName;
			CertificatePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*CertificatePath);

			if (IFileManager::Get().FileExists(*CertificatePath))
			{
				UE_LOG(LogTemp, Verbose, TEXT("MQTT => Pinning certificate: %s"), *CertificatePath);

				securityPolicy.pinnedCertificates = @[[NSData dataWithContentsOfFile:CertificatePath.GetNSString()]];
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("MQTT => Could not find certificate bundle at: %s"), *CertificatePath);
			}
		}

		securityPolicy.allowInvalidCertificates = Settings->AllowInvalidCertificates;
		securityPolicy.validatesCertificateChain = Settings->ValidatesCertificateChain;
		securityPolicy.validatesDomainName = Settings->ValidatesDomainName;
		transport.securityPolicy = securityPolicy;
		mqttSession.transport = transport;
	}
	else
	{
		MQTTCFSocketTransport* transport = [[MQTTCFSocketTransport alloc] init];
		transport.host = config.HostUrl.GetNSString();
		transport.port = config.Port;
		mqttSession.transport = transport;
	}

	mqttSession.password = connectionData.Password.GetNSString();
	mqttSession.userName = connectionData.Login.GetNSString();

	[mqttSession connectWithConnectHandler:^(NSError* error) {
		if (error)
		{
			int errCode = error.code;
			FString errMsg = FString(error.localizedDescription);
			UE_LOG(LogTemp, Error, TEXT("MQTT => Connect Error %d: %s"), errCode, *errMsg);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnErrorDelegate.ExecuteIfBound(errCode, errMsg);
			});
		}
	}];
}

void UMqttClient::Disconnect(const FOnDisconnectDelegate& onDisconnectCallback)
{
	OnDisconnectDelegate = onDisconnectCallback;

	[mqttSession disconnect];
}

void UMqttClient::Subscribe(FString topic, int qos)
{
	[mqttSession subscribeToTopic:topic.GetNSString()
						  atLevel:ConversionUtils::ConvertIntToQosLevel(qos)
				 subscribeHandler:^(NSError* error, NSArray<NSNumber*>* gQoss) {
					 if (error)
					 {
						 int errCode = error.code;
						 FString errMsg = FString(error.localizedDescription);
						 UE_LOG(LogTemp, Error, TEXT("MQTT => Sub Error %d: %s"), errCode, *errMsg);
						 AsyncTask(ENamedThreads::GameThread, [=]() {
							 OnErrorDelegate.ExecuteIfBound(errCode, errMsg);
						 });
					 }
				 }];
}

void UMqttClient::Unsubscribe(FString topic)
{
	[mqttSession unsubscribeTopic:topic.GetNSString()
			   unsubscribeHandler:^(NSError* error) {
				   if (error)
				   {
					   int errCode = error.code;
					   FString errMsg = FString(error.localizedDescription);
					   UE_LOG(LogTemp, Error, TEXT("MQTT => Unsub Error %d: %s"), errCode, *errMsg);
					   AsyncTask(ENamedThreads::GameThread, [=]() {
						   OnErrorDelegate.ExecuteIfBound(errCode, errMsg);
					   });
				   }
			   }];
}

void UMqttClient::Publish(FMqttMessage message)
{
	[mqttSession publishData:[message.Message.GetNSString() dataUsingEncoding:NSUTF8StringEncoding]
					 onTopic:message.Topic.GetNSString()
					  retain:message.Retain
						 qos:ConversionUtils::ConvertIntToQosLevel(message.Qos)
			  publishHandler:^(NSError* error) {
				  if (error)
				  {
					  int errCode = error.code;
					  FString errMsg = FString(error.localizedDescription);
					  UE_LOG(LogTemp, Error, TEXT("MQTT => Publish Error %d: %s"), errCode, *errMsg);
					  AsyncTask(ENamedThreads::GameThread, [=]() {
						  OnErrorDelegate.ExecuteIfBound(errCode, errMsg);
					  });
				  }
			  }];
}

void UMqttClient::Init(FMqttClientConfig configData)
{
	config = configData;

	MQTTSession* session = [[MQTTSession alloc] init];
	session.clientId = configData.ClientId.GetNSString();

	MqttDelegate* mqttDelegate = [[MqttDelegate alloc] init];

	mqttDelegate.onConnected = ^() {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			OnConnectDelegate.ExecuteIfBound();
		});
	};

	mqttDelegate.onDisconnected = ^() {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			OnDisconnectDelegate.ExecuteIfBound();
		});
	};

	mqttDelegate.onPublished = ^(int mid) {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			OnPublishDelegate.ExecuteIfBound(mid);
		});
	};

	mqttDelegate.onMessageReceived = ^(FMqttMessage message) {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			OnMessageDelegate.ExecuteIfBound(message);
		});
	};

	mqttDelegate.onSubscribed = ^(int mid, TArray<int> qos) {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			OnSubscribeDelegate.ExecuteIfBound(mid, qos);
		});
	};

	mqttDelegate.onUnsubscribed = ^(int mid) {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			OnUnsubscribeDelegate.ExecuteIfBound(mid);
		});
	};

	session.delegate = mqttDelegate;

	mqttSession = (MQTTSession*)CFBridgingRetain(session);
}
