
MCU = atmega328p

# This is the name of the file that shall be created. (It is also the name of my primary source file, without the file extension.)
TARGET = demo
# create a variable that only pertains to this project
#MY_OWN_LIBRARY_DIR = /usr/home/MJ/Arduino/libraries/mj_midi
# "EXTRAINCDIRS" is a variable used by the base makefile. The base makefile creates a -I compiler flag for every item in the "EXTRAINCDIRS" list.
#EXTRAINCDIRS = $(MY_OWN_LIBRARY_DIR)

# specify *.c source files pertaining to my project
# SRC = demo.c 

# specify *.cpp source files pertaining to my project
#PSRC = temp.cpp $(MY_OWN_LIBRARY_DIR)/midi-listener.cpp $(MY_OWN_LIBRARY_DIR)/midi-song.cpp
PSRC = 	esp8266.cpp

# specify additional (non-core) Arduino libraries to include
ARDLIBS = SoftwareSerial  

# include my base makefile
include arduino-base.mk