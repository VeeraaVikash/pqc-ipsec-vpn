CC = gcc
CFLAGS = -fPIC -Wall -Wextra -O2 \
    -I/usr/include/strongswan \
    -I/usr/include/strongswan/utils \
    -I/usr/include/strongswan/plugins
LDFLAGS = -shared -loqs -lcrypto

TARGET = libstrongswan-pqc-ke.so
SOURCES = pqc_ke_plugin.c pqc_ke_handler.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install:
	sudo cp $(TARGET) /usr/lib/ipsec/plugins/
	sudo chmod 644 /usr/lib/ipsec/plugins/$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all install clean
