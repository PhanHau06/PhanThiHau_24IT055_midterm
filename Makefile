CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = myls

OBJS = main.o options.o listing.o sort.o display.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c options.h listing.h
options.o: options.c options.h
listing.o: listing.c listing.h options.h sort.h display.h
sort.o: sort.c sort.h listing.h options.h
display.o: display.c display.h listing.h options.h

.c.o:

	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
