
#include <iostream>
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
	/* Check config for topic mapping->[pin:jsonpath]->filter */
	// for now, just some sorta gross hacks on raw messages.
	for (auto map : this->outputMaps) {
		std::cout << "Considering topic: " << map.topic << std::endl;
		bool matches;
		int rc = mosqpp::topic_matches_sub(map.topic, message->topic, &matches);
		if (rc != MOSQ_ERR_SUCCESS) {
			printf("invalid params is code error, out of memory we can't handle: %d\n", rc);
			break;
		}
		if (matches) {
			if (strcmp(map.magic, "int") == 0) {
				int x = atoi((const char*) message->payload);
				printf("blynking int %d -> %d\n", map.pin, x);
				this->_blynk.virtualWrite(map.pin, x);
			} else if (strcmp(map.magic, "str") == 0) {
				const char *x = (const char *) message->payload;
				printf("blynking str %d -> %s\n", map.pin, x);
				this->_blynk.virtualWrite(map.pin, x);
			} else if (strcmp(map.magic, "lcd") == 0) {
				const char *x = (const char *) message->payload;
				printf("blynking lcd %d -> %s\n", map.pin, x);
				char mem[64] = "";
				BlynkParam cmd(mem, 0, sizeof(mem));
				cmd.add("p");
				cmd.add(0);
				cmd.add(0);
				cmd.add(x);
				this->_blynk.virtualWrite(map.pin, cmd);
			} else {
				printf("unknown magic mapping: %s\n", map.magic);
			}
		}
	}
}

void BlynkMQTT::on_log(int level, const char* str)
{
	printf("mqlog: level: %d: %s\n", level, str);
}

void BlynkMQTT::read(const BlynkReq& request)
{
	printf("<<read req for pin %d\n", request.pin);
}

void BlynkMQTT::write(const BlynkReq& request, const BlynkParam& param)
{
	printf(">> write on pin %d, with: %s\n", request.pin, param.asStr());
	/* Check map of vars to topics and write back */
	for (auto map : this->inputMaps) {
		if (map.pin == request.pin) {
			printf("pin %d matched, publishing to %s\n", request.pin, map.topic);
			publish(NULL, map.topic, strlen(param.asStr()), param.asStr(), 0, false);
			break;
		}
	}
}

void BlynkMQTT::add_out_map(OutputMap map)
{
	this->outputMaps.push_back(map);
	std::cout << "added map for topic: " << map.topic << " map size now: " << this->outputMaps.size() << std::endl;
}

void BlynkMQTT::add_in_map(InputMap map)
{
	this->inputMaps.push_back(map);
}
