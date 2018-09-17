/*
 * Karl Palsson <karlp@etactica.com> Feb 2016
 * MIT, ISC, Apache2 or BSD 2 clause, at your choice
 */

//#define BLYNK_DEBUG
#define BLYNK_PRINT stdout
#include <getopt.h>
#include <json.h>
#include <BlynkApiLinux.h>
#include <BlynkSocket.h>
#include "mq.h"
#include "AppConfig.h"


static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);
#include <BlynkWidgets.h>

#define MQTT_DEFAULT_DOMAIN "127.0.0.1"

struct blynk_data {
	const char *token;
	const char *server;
	const char *port;
};

struct mqtt_data {
	const char *server;
};

struct state_data {
	struct blynk_data blynk;
	struct mqtt_data mqtt;
};

static BlynkMQTT *blynkMQTT;

BLYNK_WRITE_DEFAULT()
{
	blynkMQTT->write(request, param);
}

BLYNK_READ_DEFAULT()
{
	blynkMQTT->read(request);
}

static
void parse_options(int argc, char* argv[], AppConfig &conf)
{
	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"config", required_argument, 0, 'c'},
		{0, 0, 0, 0}
	};
	
	// Set default values TODO - make a member function?
	conf.blynk_server = BLYNK_DEFAULT_DOMAIN;
	conf.blynk_port = BLYNK_DEFAULT_PORT;
	conf.mqtt_port = 1883;
	conf.mqtt_server = MQTT_DEFAULT_DOMAIN;
	
	const char* usage =
		"Usage: %s [options]\n"
		"\n"
		"Options:\n"
		"  -h, --help  Print this help\n"
		"  -c config.json, --config=configfile.json JSON Configuration file.\n"
		"\n"
		"Defaults:\n"
		"  Unless specified in the config file, the following defaults are used:\n"
		"  blynk.server: " BLYNK_DEFAULT_DOMAIN "\n"
		"  blynk.port: " BLYNK_TOSTRING(BLYNK_DEFAULT_PORT) "\n"
		"  mqtt.server: " MQTT_DEFAULT_DOMAIN "\n"
		"  outputs: empty list"
		"\n";

	int rez;
	while (-1 != (rez = getopt_long(argc, argv, "hc:", long_options, NULL))) {
		switch (rez) {
		case 'c':
			if (conf.parse(optarg)) {
				printf("Config successfully loaded from: %s\n", optarg);
			} else {
				printf("Failed to parse config from: %s: %s\n", optarg, conf.failure());
			}
			break;
		case 'h':
		default:
			printf(usage, argv[0]);
			exit(0);
		};
	};
	// Allowed to load extra settings here.

	// Check mandatory options
	if (!conf.is_valid()) {
		printf("Final configuraton is invalid: %s\n", conf.failure());
		exit(1);
	}
	conf.dump();
}


int main(int argc, char* argv[])
{
	AppConfig conf;
	parse_options(argc, argv, conf);

	blynkMQTT = new BlynkMQTT(Blynk, conf);
	blynkMQTT->connect(conf.mqtt_server, conf.mqtt_port);
	
	blynkMQTT->add_in_map(InputMap(3, "blynk/input/json/slider"));
	//blynkMQTT->add_in_map(InputMap(0, "blynk/input/json/button/0"));
	//blynkMQTT->add_in_map(InputMap(1, "blynk/input/json/button/1"));

	Blynk.begin(conf.blynk_token, conf.blynk_server, conf.blynk_port);

	blynkMQTT->loop_start();
	while (blynkMQTT->should_run()) {
		Blynk.run();
		usleep(100 * 1000);
	}
	blynkMQTT->disconnect();
	blynkMQTT->loop_stop();
	blynkMQTT->clean();

	return 0;
}
