// Copyright (c) 2019 Nineva Studios

#pragma once

#import <MQTTClient/MQTTClient.h>

class ConversionUtils
{
public:

	static MQTTQosLevel ConvertIntToQosLevel(int qos);
	static int GonvertQosLevelToInt(MQTTQosLevel qosLevel);
};