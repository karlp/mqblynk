
#include <iostream>
#include <stdio.h>

#include <json-c/json.h>
#include "jsonpath.h"
#include "mq.h"

/* NO! this doesn't post directly to blynk? this instead just processes and returns values to be processed elsewhere?*/
static void mq_blynk_js_cb(struct json_object *js, void *priv)
{
	printf("entered callback!\n");
	OutputMap *map = (OutputMap*)priv;
	switch (json_object_get_type(js)) {
	case json_type_double:
		printf("virtualwrite %d of double: %f\n", map->pin, json_object_get_double(js));
		map->blynk->virtualWrite(map->pin, json_object_get_double(js));
		break;
	case json_type_int:
		/* fuck, I need another object wrapper? or put the "blynk" object into the map? */
		printf("virtualwrite %d of int: %d\n", map->pin, json_object_get_int(js));
		map->blynk->virtualWrite(map->pin, json_object_get_int(js));
		break;
	default:
		break;
	}
}


BlynkMQTT::BlynkMQTT(BlynkSocket &blynk) : mosquittopp(), _blynk(blynk)
{
	this->_should_run = true;
	mosqpp::lib_init();
}

BlynkMQTT::~BlynkMQTT()
{
	mosqpp::lib_cleanup();
}

void BlynkMQTT::on_connect(int rc)
{
	printf("got connected!: %d\n", rc);
	rc = subscribe(NULL, "mqblynk/command/#", 0);
	printf("susbcribe returned: %d: %s\n", rc, mosqpp::strerror(rc));
	for (auto e : this->outputMaps) {
		subscribe(NULL, e->topic);
	}
}

void BlynkMQTT::on_message(const struct mosquitto_message* message)
{
	printf("Got message on topic: %s\n", message->topic);
	const char *payloads = (const char *)message->payload;
	bool matches;
	mosqpp::topic_matches_sub("mqblynk/command/#", message->topic, &matches);
	if (matches) {
		if (strstr(payloads, "quit")) {
			printf("exiting on quit request\n");
			this->_should_run = false;
		} else {
			printf("command unrecognised!: %s\n", payloads);
		}
		return;
	}
	
	enum json_tokener_error js_tok_error;
	struct json_object *js = json_tokener_parse_verbose((const char*)message->payload, &js_tok_error);

	if (js_tok_error != json_tokener_success) {
		printf("Invalid json, reason: %s, actual data: %s\n", json_tokener_error_desc(js_tok_error), (const char*)message->payload);
		return;
	}
	
	/* Check config for topic mapping->[pin:jsonpath]->filter */
	// for now, just some sorta gross hacks on raw messages.
	for (auto map : this->outputMaps) {
		std::cout << "Considering topic: " << map->topic << std::endl;
		bool matches;
		int rc = mosqpp::topic_matches_sub(map->topic, message->topic, &matches);
		if (rc != MOSQ_ERR_SUCCESS) {
			printf("invalid params is code error, out of memory we can't handle: %d\n", rc);
			break;
		}
		if (matches) {
			printf("Topic matches\n");
			jp_match(map->_jp->path, js, mq_blynk_js_cb, map);
			


//			if (strcmp(map.magic, "int") == 0) {
//				int x = atoi((const char*) message->payload);
//				printf("blynking int %d -> %d\n", map.pin, x);
//				this->_blynk.virtualWrite(map.pin, x);
//			} else if (strcmp(map.magic, "str") == 0) {
//				const char *x = (const char *) message->payload;
//				printf("blynking str %d -> %s\n", map.pin, x);
//				this->_blynk.virtualWrite(map.pin, x);
//			} else if (strcmp(map.magic, "lcd") == 0) {
//				const char *x = (const char *) message->payload;
//				printf("blynking lcd %d -> %s\n", map.pin, x);
//				char mem[64] = "";
//				BlynkParam cmd(mem, 0, sizeof(mem));
//				cmd.add("p");
//				cmd.add(0);
//				cmd.add(0);
//				cmd.add(x);
//				this->_blynk.virtualWrite(map.pin, cmd);
//			} else {
//				printf("unknown magic mapping: %s\n", map.magic);
//			}
		}
	}
	json_object_put(js);
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

//void BlynkMQTT::add_out_map(std::unique_ptr<OutputMap> map)
//{
//	this->outputMaps.push_back(map.get());
//	std::cout << "added map for topic: " << map.get()->topic << " map size now: " << this->outputMaps.size() << std::endl;
//}

void BlynkMQTT::add_out_map(OutputMap *map)
{
	map->blynk = &_blynk;
	this->outputMaps.push_back(map);
}

void BlynkMQTT::add_in_map(InputMap map)
{
	this->inputMaps.push_back(map);
}

void BlynkMQTT::magic()
{
	printf("whee\n");
	for (auto e : this->outputMaps) {
		printf("%s -> %d\n", e->topic, e->pin);
	}
}

bool BlynkMQTT::should_run()
{
	return _should_run;
}

void BlynkMQTT::clean()
{
	for (auto e : this->outputMaps) {
		delete e;
	}
}
