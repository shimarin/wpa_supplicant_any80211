PREFIX ?= /usr/local

.PHONY: all clean install

all: wpa_supplicant_any80211

wpa_supplicant_any80211: wpa_supplicant_any80211.cpp
	g++ -std=c++23 -o $@ $^

%.bin: %.cpp
	g++ -std=c++23 -g -o $@ $^

clean:
	rm -f wpa_supplicant_any80211 *.bin

install: wpa_supplicant_any80211
	install -D -m 755 wpa_supplicant_any80211 $(DESTDIR)$(PREFIX)/bin/wpa_supplicant_any80211
