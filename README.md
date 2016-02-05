
aim:
jsonpath config file, selecting from a mqtt topic, posting as blynk variables

maybe: (probably)
blynk variables back to mqtt topics.  


actual currently:
static maps of topics for both in and out, with plain vars, no jsonpath :)
http://imgur.com/NRtM95o

```
$ mosquitto_pub -t blynk/output/json/4 -m "kak  dongs"
$ mosquitto_pub -t blynk/output/json/6 -m 801
```

```
$ mosquitto_sub -t blynk/# -v
blynk/input/json/slider 465
blynk/input/json/slider 747
blynk/input/json/slider 829
blynk/input/json/slider 840
blynk/input/json/slider 777
blynk/input/json/slider 584
blynk/input/json/slider 531
blynk/input/json/slider 517
blynk/input/json/slider 517
blynk/input/json/slider 517
blynk/input/json/button/1 0
blynk/input/json/button/1 1
blynk/input/json/button/1 0
blynk/input/json/button/0 0
blynk/input/json/slider 520
blynk/input/json/slider 662
```
