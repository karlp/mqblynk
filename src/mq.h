/* 
 * File:   mq.h
 * Author: karlp
 *
 * Created on February 4, 2016, 10:18 PM
 */

#pragma once
#include <memory>
#include <list>
#include <mosquittopp.h>
#include <BlynkApiLinux.h>
#include <BlynkSocket.h>

#include "TopicMaps.h"


class BlynkMQTT : public mosqpp::mosquittopp {
public:
	BlynkMQTT(BlynkSocket &blynk);
	~BlynkMQTT();

	virtual void on_message(const struct mosquitto_message* message);

	virtual void on_connect(int rc);

	virtual void on_log(int level, const char* str);

	virtual void read(const BlynkReq& request);
	virtual void write(const BlynkReq& request, const BlynkParam &param);

//	virtual void add_out_map(std::unique_ptr<OutputMap> map);
	virtual void add_out_map(OutputMap *map);
	virtual void add_in_map(InputMap map);
	void magic(void);
	bool should_run(void);
	void clean(void);
private:
	BlynkSocket &_blynk;
	std::list<OutputMap*> outputMaps;
	std::list<InputMap> inputMaps;
	bool _should_run;

};
