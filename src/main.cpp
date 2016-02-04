/*
 * Karl Palsson <karlp@etactica.com> Feb 2016
 * MIT, ISC, Apache2 or BSD 2 clause, at your choice
 */

//#define BLYNK_DEBUG
#define BLYNK_PRINT stdout
#include <BlynkApiLinux.h>
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

#include <BlynkWidgets.h>

BLYNK_WRITE(V1)
{
  BLYNK_LOG("Got a value: %s", param[0].asStr());
}

int main(int argc, char* argv[])
{
    const char *auth, *serv, *port;
    parse_options(argc, argv, auth, serv, port);

    Blynk.begin(auth, serv, port);

    while(true) {
        Blynk.run();
    }

    return 0;
}
