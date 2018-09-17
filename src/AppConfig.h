/* 
 * Karl Palsson <karlp@tweak.net.au>, Feb 2016
 */

#pragma once

#include <list>
#include "TopicMaps.h"

// name space pollution, need to learn how to do that properly.
enum AppConfigCode {
		AC_INVALID,
		AC_OK,
		AC_BAD_FILE,
		AC_REQUIRED_FIELD_MISSING,
		__AC_CODE_MAX
};

class AppConfig {
public:
	/**
	 * Parse a json config file as best we can.
         * @param jsonfile
         * @return true if parse succeeded, no idea whether valid total config or not
         */
	bool parse(const char *jsonfile);
	
	
	/**
	 * TODO - take a "stream" or something to write to?
         */
	void dump(void);
	
	/**
	 * check whether all requied fields are valid, and optional ones are sane
         * @return 
         */
	bool is_valid(void);
	
	/**
	 * Return a string to help describe failures, for both parse and validity
         * @return 
         */
	const char *failure();
	
	
	std::list<OutputMap*> outputs; // may need pointers?
	const char *blynk_token;
	const char *blynk_server;
	uint16_t blynk_port;
	const char *mqtt_server;
	int mqtt_port;
private:
	AppConfigCode code;
	bool _valid;
};
