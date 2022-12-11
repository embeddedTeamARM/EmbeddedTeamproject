CC=gcc
OBJS=main.o bluetooth.o init.o led.o rotaryencoder.o rtc.o saveload.o
HEADER=header_define.h header_function.h
TARGET=app
LIBS=-lwiringPi -lpthread

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

main.o: $(HEADER) main.c $(LIBS)
bluetooth.o: $(HEADER) bluetooth.c $(LIBS)
init.o: $(HEADER) init.c $(LIBS)
led.o: $(HEADER) led.c $(LIBS)
rotaryencoder.o: $(HEADER) rotaryencoder.c $(LIBS)
rtc.o: $(HEADER) rtc.c $(LIBS)
saveload.o: $(HEADER) saveload.c $(LIBS)

clean:
	rm -f $(TARGET)
	rm -f *.o
	rm -f *.ini