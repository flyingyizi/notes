TARGET=t

GCC_PREFIX=/c/Users/tu_xu/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-

REMOVE = rm -f
SIZE = size
CC=${GCC_PREFIX}gcc
LD=${GCC_PREFIX}ld
OBJDUMP=${GCC_PREFIX}objdump
OBJCOPY=${GCC_PREFIX}objcopy

SRC += $(wildcard *.c)
ASRC += $(wildcard *.S)
# Define all object files
OBJS = $(SRC:.c=.o) $(ASRC:.S=_asm.o) 

# Optimization level, can be [0, 1, 2, 3, s]. 0 turns off optimization.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s
# 链接时优化支持有问题，编译通不过，先临时删除了"-flto"
CFLAGS += -O$(OPT) 
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
LDFLAGS += -T linker.lds  

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS 		=  -I. $(CFLAGS) 
ALL_ASFLAGS 	=  -I. -x assembler-with-cpp $(ASFLAGS)

all: $(TARGET) sizeafter
	${OBJDUMP} -h $(TARGET)
	${OBJCOPY} -O binary   $(TARGET)   $(TARGET).out  
	ls -lh   $(TARGET).out  

$(TARGET): ${OBJS}
	$(LD) ${LDFLAGS} $^ -o $@

# Compile: create assembler files from C source files.
%.s: %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $@

# Compile: create object files from C++ source files
%.o: %.c
	$(CC) -c $(ALL_CFLAGS) $< -o $@

# Compile: create assembler files from C source files.
%.s: %.cpp
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
%_asm.o: %.S
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

# Target: clean project.
clean: clean_list

clean_list :
	$(REMOVE) $(OBJS)  $(TARGET)  $(TARGET).map $(TARGET).out
	$(REMOVE) $(SRC:.c=.lst)
	$(REMOVE) *~

FILESIZE = $(SIZE) -A $(TARGET)
sizeafter:
	@if [ -f $(TARGET) ]; then echo; echo "size is:"; $(FILESIZE); echo; fi
