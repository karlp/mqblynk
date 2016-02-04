
#include <stdio.h>

#include "mq.h"


void BlynkMQTT::on_connect(int rc) {
    printf("got connected!: %d\n", rc);
    rc = subscribe(NULL, "blynk/+/json/#", 0);
    printf("susbcribe returned: %d: %s\n", rc, mosqpp::strerror(rc));
}

void BlynkMQTT::on_message(const struct mosquitto_message* message) {
    printf("Got message on topic: %s\n", message->topic);
}

void BlynkMQTT::on_log(int level, const char* str) {
    printf("mqlog: level: %d: %s\n", level, str);
}
