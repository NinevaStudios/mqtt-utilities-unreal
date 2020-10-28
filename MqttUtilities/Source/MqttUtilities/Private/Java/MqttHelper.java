// Copyright (c) 2019 Nineva Studios

package com.ninevastudios.mqttdemo;

import android.app.Activity;
import android.util.Log;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class MqttHelper {
	private final String TAG = "MQTT => ";

	public static native void onConnected(long callbackAddr);
	public static native void onDisconnected(long callbackAddr);
	public static native void onSubscribed(long callbackAddr, int messageId, int qos);
	public static native void onUnsubscribed(long callbackAddr, int messageId);
	public static native void onPublished(long callbackAddr, int messageId);
	public static native void onMessageArrived(long callbackAddr, String topic, String message, int qos, boolean retain);
	public static native void onError(long callbackAddr, int errCode, String errMessage);

	private MqttAndroidClient mqttAndroidClient;
	private long mCallbackAddr;
	private String mServerAddr;
	private int mPort;

	public MqttHelper(Activity activity, String serverAddr, int port, String clientId, long callbackAddr) {
		mCallbackAddr = callbackAddr;
		mServerAddr = serverAddr;
		mPort = port;

		String serverUri = "tcp://" + mServerAddr + ":" + mPort;
		mqttAndroidClient = new MqttAndroidClient(activity, serverUri, clientId);
	}

	public void connect(String userName, String password, boolean useTls) {
		try {
			MqttConnectOptions options = new MqttConnectOptions();;
			if (!userName.isEmpty()) options.setUserName(userName);
			if (!password.isEmpty()) options.setPassword(password.toCharArray());

			// this overrides client server URI
			String[] serverUris = new String[1];
			String protocol = useTls ? "ssl://" : "tcp://";
			serverUris[0] = protocol + mServerAddr + ":" + mPort;
			options.setServerURIs(serverUris);

			mqttAndroidClient.connect(options, null, new IMqttActionListener() {
				@Override
				public void onSuccess(IMqttToken asyncActionToken) {
					onConnected(mCallbackAddr);
				}

				@Override
				public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
					MqttException e = asyncActionToken.getException();
					onError(mCallbackAddr, e.getReasonCode(), e.getMessage());
				}
			});
		} catch (MqttException e) {
			onError(mCallbackAddr, e.getReasonCode(), e.getMessage());
		}
	}

	public void disconnect(int quiesceTimeout) {
		try {
			if (!mqttAndroidClient.isConnected()) {
				Log.d(TAG, "MQTT => There is no active connection");
				return;
			}

			mqttAndroidClient.disconnect(quiesceTimeout, null, new IMqttActionListener() {
				@Override
				public void onSuccess(IMqttToken asyncActionToken) {
					onDisconnected(mCallbackAddr);
				}

				@Override
				public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
					MqttException e = asyncActionToken.getException();
					onError(mCallbackAddr, e.getReasonCode(), e.getMessage());
				}
			});
		} catch (MqttException e) {
			onError(mCallbackAddr, e.getReasonCode(), e.getMessage());
		}
	}

	public void subscribeToTopic(final String topic, final int qos) {
		try {
			if (!mqttAndroidClient.isConnected()) {
				Log.d(TAG, "MQTT => There is no active connection");
				return;
			}

			mqttAndroidClient.subscribe(topic, qos, this, new IMqttActionListener() {
				@Override
				public void onSuccess(IMqttToken asyncActionToken) {
					onSubscribed(mCallbackAddr, asyncActionToken.getMessageId(), qos);
				}

				@Override
				public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
					MqttException e = asyncActionToken.getException();
					onError(mCallbackAddr, e.getReasonCode(), e.getMessage());
				}
			});

			mqttAndroidClient.subscribe(topic, qos, new IMqttMessageListener() {
				@Override
				public void messageArrived(String topic, MqttMessage message) {
					String messageStr = new String(message.getPayload());
					onMessageArrived(mCallbackAddr, topic, messageStr, message.getQos(), message.isRetained());
				}
			});
		} catch (MqttException e) {
			onError(mCallbackAddr, e.getReasonCode(), e.getMessage());
		}
	}

	public void unsubscribeFromTopic(final String topic) {
		try {
			if (!mqttAndroidClient.isConnected()) {
				Log.d(TAG, "MQTT => There is no active connection");
				return;
			}

			mqttAndroidClient.unsubscribe(topic, null, new IMqttActionListener() {
				@Override
				public void onSuccess(IMqttToken asyncActionToken) {
					onUnsubscribed(mCallbackAddr, asyncActionToken.getMessageId());
				}

				@Override
				public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
					MqttException e = asyncActionToken.getException();
					onError(mCallbackAddr, e.getReasonCode(), e.getMessage());
				}
			});
		} catch (MqttException e) {
			onError(mCallbackAddr, e.getReasonCode(), e.getMessage());
		}
	}

	public void publishMessage(String topic, String payload, int qos, boolean retain) {
		try {
			if (!mqttAndroidClient.isConnected()) {
				Log.d(TAG, "MQTT => There is no active connection");
				return;
			}

			MqttMessage message = new MqttMessage();
			message.setPayload(payload.getBytes());
			message.setQos(qos);
			message.setRetained(retain);
			mqttAndroidClient.publish(topic, message, null, new IMqttActionListener() {
				@Override
				public void onSuccess(IMqttToken asyncActionToken) {
					onPublished(mCallbackAddr, asyncActionToken.getMessageId());
				}

				@Override
				public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
					MqttException e = asyncActionToken.getException();
					onError(mCallbackAddr, e.getReasonCode(), e.getMessage());
				}
			});
		} catch (MqttException e) {
			onError(mCallbackAddr, e.getReasonCode(), e.getMessage());
		}
	}
}
