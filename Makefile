CPP=g++
CPPFLAGS=`pkg-config --libs gtk+-2.0` `pkg-config --cflags gtk+-2.0` -fPIC -Wall
SOURCES=main.cpp utils.cpp plugin_base.cpp widget_base.cpp library_item.cpp library.cpp library_widget.cpp library_filter.cpp library_plugin.cpp
OUTPUT=build/library.so

all:	compile

compile:    main

main:	$(SOURCES)
	$(CPP) -pthread -ggdb -std=c++11 -shared -O2 -o $(OUTPUT) $(SOURCES)  $(CPPFLAGS)

clean:
	rm build/*

user_install:
	sh -c "mkdir ~/.local/lib > /dev/null; exit 0" > /dev/null
	sh -c "mkdir ~/.local/lib/deadbeef; exit 0" > /dev/null
	cp $(OUTPUT) ~/.local/lib/deadbeef

user_uninstall:
	rm ~/.local/lib/deadbeef/library.so
	
run:	user_install
	sh -c "killall deadbeef > /dev/null; exit 0"
	deadbeef