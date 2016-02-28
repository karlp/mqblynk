/*
 * Karl Palsson <karlp@tweak.net.au> Feb 2016
 */

#include <iostream>
#include <stdio.h>
#include <json-c/json_object.h>
#include <list>

#include "AppConfig.h"

// TODO this class is screaming for tests!
// FIXME -strdup must be freed!

static void handle_blynk(json_object *js, AppConfig *conf) {
	json_object_object_foreach(js, key, val) {
		const char *x = json_object_get_string(val);
		if (strcmp(key, "token") == 0) {
			conf->blynk_token = strdup(x);
		}
		if (strcmp(key, "server") == 0) {
			conf->blynk_server = strdup(x);
		}
		// TODO -handle being int or string!
		if (strcmp(key, "port") == 0) {
			conf->blynk_port = strdup(x);
		}
	}	
}

static void handle_mqtt(json_object *js, AppConfig *conf) {
	json_object_object_foreach(js, key, val) {
		const char *x = json_object_get_string(val);
		if (strcmp(key, "server") == 0) {
			conf->mqtt_server = strdup(x);
		}
		// TODO -handle being int or string!
//		if (strcmp(key, "port") == 0) {
//			conf->mqtt_port = strdup(x);
//		}
	}
}

static void handle_output_e(json_object *js, AppConfig *conf) {
	struct jp_state *jps;
	const char *topic;
	int pin;
	int found = 0;
	json_object_object_foreach(js, key, val) {
		if (strcmp(key, "jsonpath") == 0) {
			const char *s = json_object_get_string(val);
			jps = jp_parse(s);
			if (!jps) {
				printf("Failed to parse jsonpath (ignoring entry): %s : %s\n",
					s, jp_error_to_string(jps->error_code));
				return;
			}
			found++;
		}
		if (strcmp(key, "topic") == 0) {
			topic = json_object_get_string(val);
			found++;
		}
		if (strcmp(key, "pin") == 0) {
			pin = json_object_get_int(val);
			found++;
		}
	}
	if (found == 3) {
		auto om = new OutputMap(jps, topic, pin);
		conf->outputs.push_back(om);
	}
//		struct jp_state *jps = jp_parse(e.jsonpath);
//		if (jps) {
//			auto om = new OutputMap(jps, e.topic, e.pin);
//			//blynkMQTT->add_out_map(std::unique_ptr<OutputMap>{om});
//			blynkMQTT->add_out_map(om);
//		} else {
//			printf("failed to parse jsonpath: %s ignoring: %s\n", e.jsonpath, jp_error_to_string(jps->error_code));
//		}
	
}

static void handle_output(json_object *js, AppConfig *conf) {
	if (json_object_get_type(js) != json_type_array) {
		return;
	}
	for (int i = 0; i < json_object_array_length(js); i++) {
		json_object *elem = json_object_array_get_idx(js, i);
		handle_output_e(elem, conf);
	}
}

/* boo, how to pass extra info? don't care now! */
// TODO - make this a map of the "strong" enum to the strings? 
// right side of the map can be a struct/class with extra info?
static const char *reasons[__AC_CODE_MAX] = {
	[AC_INVALID] = "Invalid error code",
	[AC_OK] = "No error",
	[AC_BAD_FILE] = "File not found or invalid json",
	[AC_REQUIRED_FIELD_MISSING] = "A required field was missing",
};


bool AppConfig::parse(const char* jsonfile) {
	this->_valid = false;
	json_object *js = json_object_from_file(jsonfile);
	if (!js) {
		this->code = AC_BAD_FILE;
		return false;
	}
	
	//json_object *blynk_js = json_object_
	json_object_object_foreach(js, key, val) {
		printf("considering top level key: %s\n", key);
		if (strcmp(key, "blynk") == 0) {
			handle_blynk(val, this);
		}
		if (strcmp(key, "mqtt") == 0) {
			handle_mqtt(val, this);
		}
		if (strcmp(key, "output") == 0) {
			handle_output(val, this);
		}
	}
	
	this->_valid = true;
	return true;
}

const char* AppConfig::failure()
{
	return reasons[this->code];
}


bool AppConfig::is_valid() {
	if (!this->_valid) {
		return false;
	}
	if (this->blynk_token && strlen(this->blynk_token) > 0) {
		return true;
	}
	//this->code = REQUIRED_FIELD_MISSING; // TODO no_token; // enum types in c++?
	return false;
}

void AppConfig::dump()
{
	std::cout << "Config dump: " << std::endl
		<< "\tBlynk settings:" << std::endl
		<< "Token: " << this->blynk_token << std::endl
		<< "Server: " << this->blynk_server << "Port:" << this->blynk_port << std::endl
		<< "\tMQTT settings:" << std::endl
		<< "Server: " << this->mqtt_server << "Port: " << this->mqtt_port << std::endl
		<< "\tOutput Maps: " << this->outputs.size() << " entries" << std::endl;
	// TODO Now, delegate to the maps themselves...
	for (auto om : this->outputs) {
		om->dump();
	}
}
