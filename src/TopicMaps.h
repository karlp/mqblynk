/* 
 * File:   TopicMaps.h
 * Author: karlp
 *
 * Created on February 17, 2016, 9:31 PM
 */

#pragma once

#include "jsonpath.h"

#include <BlynkApiLinux.h>
#include <BlynkSocket.h>

class OutputMap {
public:
	OutputMap(struct jp_state *jp, const char * topic, int pin, BlynkSocket *blynk = NULL) : 
	_jp(jp), topic(topic), pin(pin), blynk(blynk) {};
	~OutputMap();
	void dump(void);
	struct jp_state *_jp;
	const char *topic;
	int pin;
	BlynkSocket *blynk;
};

class InputMap {
public:

	InputMap(int pin, const char *topic) :
            pin(pin), topic(topic) {};
	~InputMap();
	void dump(void);
	int pin;
	const char *topic;
};

