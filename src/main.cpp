/*
 * Karl Palsson <karlp@etactica.com> Feb 2016
 * MIT, ISC, Apache2 or BSD 2 clause, at your choice
 */

//#define BLYNK_DEBUG
#define BLYNK_PRINT stdout
#include <getopt.h>
#include <BlynkApiLinux.h>
#include <BlynkSocket.h>
#include "mq.h"

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
void parse_options(int argc, char* argv[], struct state_data *st)
{
	static struct option long_options[] = {
		{"token", required_argument, 0, 't'},
		{"blynk", required_argument, 0, 'b'},
		{"mqtt", required_argument, 0, 'm'},
		{"port", required_argument, 0, 'p'},
		{0, 0, 0, 0}
	};

	// Set default values
	st->blynk.token = NULL;
	st->blynk.server = BLYNK_DEFAULT_DOMAIN;
	st->blynk.port = TOSTRING(BLYNK_DEFAULT_PORT);
	st->mqtt.server = MQTT_DEFAULT_DOMAIN;

	const char* usage =
		"Usage: %s [options]\n"
		"\n"
		"Options:\n"
		"  -t auth, --token=auth     Your auth token\n"
		"  -b addr, --blynk=addr     Blynk Server name (default: " BLYNK_DEFAULT_DOMAIN ")\n"
		"  -p num,  --port=num       Blynk Server port (default: " TOSTRING(BLYNK_DEFAULT_PORT) ")\n"
		"  -m addr, --mqtt=addr      MQTT Server name (default: " MQTT_DEFAULT_DOMAIN ")\n"
		"\n";

	int rez;
	while (-1 != (rez = getopt_long(argc, argv, "b:m:p:t:", long_options, NULL))) {
		switch (rez) {
		case 'b':
			st->blynk.server = optarg;
			break;
		case 'm':
			st->mqtt.server = optarg;
			break;
		case 'p':
			st->blynk.port = optarg;
			break;
		case 't':
			st->blynk.token = optarg;
			break;
		default: printf(usage, argv[0]);
			exit(1);
		};
	};

	// Check mandatory options
	if (!st->blynk.token) {
		printf(usage, argv[0]);
		exit(1);
	}
}

int main(int argc, char* argv[])
{
	struct state_data state = {};
	parse_options(argc, argv, &state);

	blynkMQTT = new BlynkMQTT(Blynk);
	blynkMQTT->connect(state.mqtt.server);

	blynkMQTT->add_out_map(OutputMap("blynk/output/json/4", 4, "lcd"));
	blynkMQTT->add_out_map(OutputMap("blynk/output/json/2", 2, "int"));
	blynkMQTT->add_out_map(OutputMap("blynk/output/json/6", 6, "int"));

	blynkMQTT->add_in_map(InputMap(3, "blynk/input/json/slider"));
	blynkMQTT->add_in_map(InputMap(0, "blynk/input/json/button/0"));
	blynkMQTT->add_in_map(InputMap(1, "blynk/input/json/button/1"));

	Blynk.begin(state.blynk.token, state.blynk.server, state.blynk.port);

	while (true) {
		Blynk.run();
		int rc = blynkMQTT->loop(100);
		if (rc) {
			printf("um, remember to fix this? :%d -> %s\n", rc, mosqpp::strerror(rc));
		}
	}

	return 0;
}
