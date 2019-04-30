### MARKHAM'S NOTES:

# This Makefile includes the Arduino core sources. (Set ARDDIR here:)
ARDDIR 				= C:/prog/Arduino/hardware/arduino/avr

CORE_LIB = $(ARDDIR)/arduino_core.a

# You should build a tiny Makefile for each of your projects and just declare a few
# things in it, then include this Makefile in it. Your Makefile should be in the same
# directory as your TARGET file (see below).
#
# Exemplia gratia:
#
# TARGET = foo 												# the name of your main file, with no extension
# SRC = foo.c bar.c  									# the C source files
# PSRC = baz.cpp qux.cpp 							# the C++ source files
# ARDLIBS = SoftwareSerial						# extra Arduino libraries
# include /d/dev/avr/Makefile.base 		# this line includes the Makefile.base

# A number of necessary variables are set to defaults below, but you can override them
# in your own Makefile:
# F_CPU (default is 16000000)
# MCU (default is atmega328p)
# AVRDUDE_PORT (default is com8)
#
# To include Arduino libraries, define
# ARDLIBS = SoftwareSerial Servo...
#
# This Makefile requires that you have no spaces in your file paths, including
# your Arduino directory (where you core, variants, and libs are stored). If
# necessary, copy your Arduino source files to another directory and define that
# directory in the variables for this file.

# This Makefile is modified from
# WinAVR Sample makefile written by Eric B. Weddington, Jörg Wunsch, et al.
# Modified (bringing often-changed options to the top) by Elliot Williams
# Modified and published by markham anderson at https://gist.github.com/entrity/5424505
# Modified by uli@vnks.de

# The following tasks are defined in this Makefile:
# make all = Make software and program
# make clean = Clean out built project files.
# make program = Download the hex file to the device, using avrdude.

# Supply the following variables:
# SRC = source files ending with .c
# PSRC = source files ending with .cpp

### These macros pertain to compiler flags
# Target file name (without extension).
ifndef TARGET
TARGET = main
endif
# List any extra directories to look for include files here.
ifndef EXTRAINCDIRS
EXTRAINCDIRS = 
endif
ifndef LDFLAGS
LDFLAGS = 
endif
ifndef CFLAGS
CFLAGS = 
endif


### These macros pertain to hardware settings
ifndef MCU
MCU = atmega328p
endif
ifndef AVRDUDE_PROGRAMMER
AVRDUDE_PROGRAMMER = arduino
# AVRDUDE_PROGRAMMER = usbtiny
endif
ifndef AVRDUDE_PORT
AVRDUDE_PORT = usb
endif
ifndef BAUDRATE
BAUDRATE = 57600
endif
ifndef F_CPU
F_CPU = 16000000
endif
ARDUINO = 100

CFLAGS += -D F_CPU=$(F_CPU) -D ARDUINO=$(ARDUINO)


### These macros pertain to supporting Arduino libs
ifndef NO_ARDUINO
	LDFLAGS += -lm # -lm = math library
	ARDLIBDIR 		= $(ARDDIR)/libraries
	ARDCOREDIR 		= $(ARDDIR)/cores/arduino
	ifeq ($(MCU),atmega328p)
	EXTRAINCDIRS += $(ARDDIR)/variants/standard
	endif
	ifeq ($(MCU),atmega2560)
	EXTRAINCDIRS += $(ARDDIR)/variants/mega
	endif
	ifeq ($(MCU),attiny85)
	EXTRAINCDIRS += $(ARDDIR)/variants/tiny8
	endif
	# add Arduino sources and include directories to PSRC and EXTRAINCDIRS
	CORE_PSRC += $(wildcard $(ARDCOREDIR)/*.cpp)
	CORE_SRC += $(wildcard $(ARDCOREDIR)/*.c)
	CORE_ASRC += $(wildcard $(ARDCOREDIR)/*.S)
	EXTRAINCDIRS += $(ARDCOREDIR)
	PSRC += $(foreach lib,$(ARDLIBS),$(ARDLIBDIR)/$(lib)/src/$(lib).cpp)
	EXTRAINCDIRS += $(foreach lib,$(ARDLIBS),$(ARDLIBDIR)/$(lib)/src)
endif


############# Don't need to change below here for most purposes  (Elliot)

# Optimization level, can be [0, 1, 2, 3, s]. 0 turns off optimization.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s

# Output format. (can be srec, ihex, binary)
FORMAT = ihex


# List Assembler source files here.
# Make them always end in a capital .S.  Files ending in a lowercase .s
# will not be considered source files but generated files (assembler
# output from the compiler), and will be deleted upon "make clean"!
# Even though the DOS/Win* filesystem matches both .s and .S the same,
# it will preserve the spelling of the filenames, and gcc itself does
# care about how the name is spelled on its command-line.
ASRC = 

# Optional compiler flags.
#  -g:        generate debugging information (for GDB, or for COFF conversion)
#  -O*:       optimization level
#  -f...:     tuning, see gcc manual and avr-libc documentation
#  -Wall...:  warning level
#  -Wa,...:   tell GCC to pass this to the assembler.
#    -ahlms:  create assembler listing

# CFLAGS += -g -O$(OPT) -mcall-prologues\
# -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
# -Wall -flto -fvisibility=hidden \
# $(patsubst %,-I%,$(EXTRAINCDIRS))

# 链接时优化支持有问题，编译通不过，先临时删除了"-flto"
CFLAGS += -g -O$(OPT) -mcall-prologues\
-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
-Wall -fvisibility=hidden \
$(patsubst %,-I%,$(EXTRAINCDIRS))


# Optional assembler flags.
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 



# Optional linker flags.
#  -Wl,...:   tell GCC to pass this to linker.
#  -Map:      create map file
#  --cref:    add cross reference to  map file
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref



# Additional libraries

# Minimalistic printf version
#LDFLAGS += -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires -lm below)
#LDFLAGS += -Wl,-u,vfprintf -lprintf_flt


# Programming support using avrdude. Settings and variables.


AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)

# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE += -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_FLAGS += -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude> 
# to submit bug reports.
#AVRDUDE_FLAGS += -v -v

#Run while cable attached or don't
# AVRDUDE_FLAGS += -E reset #keep chip disabled while cable attached
#AVRDUDE_FLAGS += -E noreset

#AVRDUDE_WRITE_FLASH = -U lfuse:w:0x04:m #run with 8 Mhz clock

#AVRDUDE_WRITE_FLASH = -U lfuse:w:0x21:m #run with 1 Mhz clock #default clock mode

#AVRDUDE_WRITE_FLASH = -U lfuse:w:0x01:m #run with 1 Mhz clock no start up time

# ---------------------------------------------------------------------------

# Define directories, if needed.
DIRAVR = C:/prog/Arduino/hardware/tools/avr
DIRAVRBIN = $(DIRAVR)/bin
DIRAVRUTILS = $(DIRAVR)/utils/bin
DIRINC = .
DIRLIB = $(DIRAVR)/avr/lib


# Define programs and commands.
SHELL = sh

CC = $(DIRAVRBIN)/avr-gcc

OBJCOPY = $(DIRAVRBIN)/avr-objcopy
OBJDUMP = $(DIRAVRBIN)/avr-objdump
SIZE = $(DIRAVRBIN)/avr-size
NM = $(DIRAVRBIN)/avr-nm


# Programming support using avrdude.
AVRDUDE = $(DIRAVRBIN)/avrdude -C $(DIRAVR)/etc/avrdude.conf


REMOVE = rm -f
COPY = cp

HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf



# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:


# Define all core object files
CORE_OBJS = $(CORE_SRC:.c=.o) $(CORE_ASRC:.S=_asm.o) $(CORE_PSRC:.cpp=.o)

# Define all object files.
OBJ = $(SRC:.c=.o) $(ASRC:.S=_asm.o) $(PSRC:.cpp=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(SRC:.c=.lst) $(PSRC:.cpp=.lst) $(CORE_ASRC:.S=.lst) $(CORE_SRC:.c=.lst) $(CORE_PSRC:.cpp=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS 		= -mmcu=$(MCU) -I. $(CFLAGS) -Wa,-adhlns=$(<:.c=.lst)
ALL_CXXFLAGS 	= -mmcu=$(MCU) -I. $(CFLAGS) -std=c++98 -Wa,-adhlns=$(<:.cpp=.lst)
ALL_ASFLAGS 	= -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target: make program!
all: begin gccversion $(TARGET).elf $(TARGET).hex $(TARGET).eep $(TARGET).lss $(TARGET).sym \
	sizeafter finished end

# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.
begin:
	@echo
	@echo $(MSG_BEGIN)

finished:
	@echo $(MSG_ERRORS_NONE)

end:
	@echo $(MSG_END)
	@echo


# Display size of file.
sizebefore:
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); echo; fi

sizeafter:
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); echo; fi



# Display compiler version information.
gccversion : 
	@$(CC) --version
	@echo 
	@echo $(OBJ)




# Convert ELF to COFF for use in debugging / simulating in
# AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
	--change-section-address .data-0x800000 \
	--change-section-address .bss-0x800000 \
	--change-section-address .noinit-0x800000 \
	--change-section-address .eeprom-0x810000 


coff: $(TARGET).elf
	@echo
	@echo $(MSG_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-avr $< $(TARGET).cof


extcoff: $(TARGET).elf
	@echo
	@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof




# Program the device.  
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) -b $(BAUDRATE) -v




# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	NM -n $< > $@


# Archive Arduino core library
$(CORE_LIB): $(CORE_OBJS)
	$(AR) rcs $(CORE_LIB) $(CORE_OBJS)

# Link: create ELF output file from object files.
.SECONDARY: $(TARGET).elf
.PRECIOUS: $(OBJ)
%.elf: $(OBJ) $(CORE_LIB)
	@echo
	@echo $(LDFLAGS)
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $(OBJ) $(CORE_LIB) --output $@ $(LDFLAGS)


# Compile: create object files from C source files.
%.o: %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C source files.
%.s: %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Compile: create object files from C++ source files
%.o: %.cpp
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CXXFLAGS) $< -o $@

# Compile: create assembler files from C source files.
%.s: %.cpp
	$(CC) -S $(ALL_CXXFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
%_asm.o: %.S
	@echo
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Target: clean project.
clean: begin clean_list finished end


clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).eep
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).cof
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).a90
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lnk
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(TARGET).lst
	$(REMOVE) $(CORE_OBJS)
	$(REMOVE) $(CORE_LIB)
	$(REMOVE) $(OBJ)
	$(REMOVE) $(LST)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) $(PSRC:.cpp=.s)
	$(REMOVE) $(PSRC:.cpp=.d)
	$(REMOVE) $(CORE_SRC:.c=.s)
	$(REMOVE) $(CORE_SRC:.c=.d)
	$(REMOVE) $(CORE_PSRC:.cpp=.s)
	$(REMOVE) $(CORE_PSRC:.cpp=.d)
	$(REMOVE) *~


# Automatically generate C source code dependencies. 
# (Code originally taken from the GNU make user manual and modified 
# (See README.txt Credits).)
#
# Note that this will work with sh (bash) and sed that is shipped with WinAVR
# (see the SHELL variable defined above).
# This may not work with other shells or other seds.

%.d: %.c
	set -e; $(CC) -MM $(ALL_CFLAGS) $< \
	| sed 's,\(.*\)\.o[ :]*,\1.o \1.d : ,g' > $@; \
	[ -s $@ ] || rm -f $@

%.d: %.cpp
	set -e; $(CC) -MM $(ALL_CFLAGS) $< \
	| sed 's,\(.*\)\.o[ :]*,\1.o \1.d : ,g' > $@; \
	[ -s $@ ] || rm -f $@

# Remove the '-' if you want to see the dependency files generated.
-include $(SRC:.c=.d)

-include $(PSRC:.cpp=.d)

# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion coff extcoff \
	clean clean_list program
 