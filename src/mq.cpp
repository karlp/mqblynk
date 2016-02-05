
#include <stdio.h>

#include "mq.h"

BlynkMQTT::BlynkMQTT(BlynkSocket &blynk) : mosquittopp(), _blynk(blynk)
{
	mosqpp::lib_init();
}


BlynkMQTT::~BlynkMQTT()
{
	mosqpp::lib_cleanup();
}

void BlynkMQTT::on_connect(int rc)
{
	printf("got connected!: %d\n", rc);
	rc = subscribe(NULL, "blynk/+/json/#", 0);
	printf("susbcribe returned: %d: %s\n", rc, mosqpp::strerror(rc));
}

void BlynkMQTT::on_message(const struct mosquitto_message* message)
{
	printf("Got message on topic: %s\n", message->topic);
	int x = atoi((const char*)message->payload);
	printf("blynking: %d\n", x);
	this->_blynk.virtualWrite(2, x);
}

void BlynkMQTT::on_log(int level, const char* str)
{
	printf("mqlog: level: %d: %s\n", level, str);
}
