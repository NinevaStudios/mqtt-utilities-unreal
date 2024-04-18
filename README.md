## If this repo helped you, please consider buying us a cup of coffee:

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=8JXUYRFCZMR7G)

# MQTT Utilities plugin for Unreal Engine

MQTT Utilities is a plugin for Unreal Engine intended to expose MQTT client functionality to blueprints.

Check out the [Documentation](https://github.com/NinevaStudios/MqttUtilities-UnrealEngine/wiki) to learn more about the plugin.

Learn more about [MQTT](http://mqtt.org/) and its basic [concepts](https://mosquitto.org/man/mqtt-7.html).

[Download](https://github.com/NinevaStudios/MqttUtilities-UnrealEngine/releases) demo applications for Windows/Mac/Android and try it out. Also, you can download complete demo project made with UE 4.23 and take a look how to use certain plugin features!

This plugin is based on the [Eclipse Mosquitto](https://github.com/eclipse/mosquitto) client Libraries for it's base.

## Features
* Establish connection with MQTT brokers
* Publish messages
* Receive messages with a certain topic
* Unified API for Windows/Mac/Android/iOS/Linux

## Supported platforms

* Windows x64
  + (Eclipse Mosquitto 1.4.8)
* Android
  + (Eclipse Mosquitto 1.4.8)
* iOS
  + (Eclipse Mosquitto 1.4.8)
* Mac OS
  + (Eclipse Mosquitto 1.4.8)
* Linux
  + (Eclipse Mosquitto 2.0.13)
  + (Tested with Ubuntu 18.04)

## List of compatible MQTT brokers

* [CloudMQTT](https://www.cloudmqtt.com/)
* [HiveMQ](https://www.hivemq.com/)
* [flespi](https://flespi.com/mqtt-broker)
* [ActiveMQ](https://activemq.apache.org/index.html)
* [mosca](https://github.com/mcollina/mosca)
* [Eclipse Mosquitto](https://mosquitto.org/)
* [RabbitMQ](https://www.rabbitmq.com/)

This list can be extended in the future. Also, even if some MQTT brokers are not listed here it doesn't necessarily mean that they are incompatible with MQTT Utilities plugin for Unreal Engine unless this is mentioned explicitly.

## Changelog

### v1.2.0 Update release

+ FIXED https://github.com/NinevaStudios/mqtt-utilities-unreal/pull/23
+ FIXED https://github.com/NinevaStudios/mqtt-utilities-unreal/pull/29
+ FIXED https://github.com/NinevaStudios/mqtt-utilities-unreal/pull/32
+ FIXED https://github.com/NinevaStudios/mqtt-utilities-unreal/pull/38

### v1.1.0 Update release

+ ADDED MQTT client implementation for Linux
+ Few README cleanup/updates

### v1.0.0 Initial release

+ ADDED MQTT client implementation for Windows/Mac/Android/iOS
