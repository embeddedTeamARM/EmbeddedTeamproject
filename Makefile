CC=gcc
OBJS=teamproject.o
TARGET=app
LIBS=-lwiringPi -lpthread

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

teamproject.o: $(HEADER) teamproject.c $(LIBS)

clean:
	rm -f $(TARGET)
	rm -f *.o
	rm -f *.ini