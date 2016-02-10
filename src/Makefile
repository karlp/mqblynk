BLYNK_HOME=../blynk

CC = g++
CFLAGS  = -I $(BLYNK_HOME) -I $(BLYNK_HOME)/linux -DLINUX
LDFLAGS = -lrt -lpthread -lubox
CFLAGS += -std=c++11 -ggdb3 -O2
CFLAGS += -Wall -Wextra

CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)

CFLAGS += -I /home/karlp/.local/include
LDFLAGS+= -L/home/karlp/.local/lib
LDFLAGS+= -lmosquittopp

CFLAGS+=-I /home/karlp/src/jsonpath
LDFLAGS+=-L /home/karlp/src/jsonpath -ljsonpath

# Disable some warnings
CFLAGS += -Wno-variadic-macros -Wno-unused-parameter -Wno-vla
	
SOURCES=main.cpp \
	$(BLYNK_HOME)/linux/BlynkDebug.cpp \
	$(BLYNK_HOME)/utility/BlynkHandlers.cpp

SOURCES+=mq.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=mqblink

all: $(SOURCES) $(EXECUTABLE)

clean:
	-rm -f $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
