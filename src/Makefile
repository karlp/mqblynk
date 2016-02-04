BLYNK_HOME=../blynk

CC = g++
CFLAGS  = -I $(BLYNK_HOME) -I $(BLYNK_HOME)/linux -DLINUX
LDFLAGS = -lrt -lpthread
CFLAGS += -ggdb3 -O2
CFLAGS += -Wall -Weffc++ -Wextra

# Disable some warnings
CFLAGS += -Wno-variadic-macros -Wno-unused-parameter -Wno-vla
	
SOURCES=main.cpp \
	$(BLYNK_HOME)/linux/BlynkDebug.cpp \
	$(BLYNK_HOME)/utility/BlynkHandlers.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=mqblink

all: $(SOURCES) $(EXECUTABLE)

clean:
	-rm $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
