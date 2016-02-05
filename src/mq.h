/* 
 * File:   mq.h
 * Author: karlp
 *
 * Created on February 4, 2016, 10:18 PM
 */

#pragma once

#include <mosquittopp.h>
#include <BlynkApiLinux.h>
#include <BlynkSocket.h>

class BlynkMQTT : public mosqpp::mosquittopp {
public:
	BlynkMQTT(BlynkSocket &blynk);
	~BlynkMQTT();

	virtual void on_message(const struct mosquitto_message* message);

	virtual void on_connect(int rc);

	virtual void on_log(int level, const char* str);
private:
	BlynkSocket &_blynk;
};
