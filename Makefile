################
# config
################
ROMSIZE = 32768

##############
# path to the retro68 toolchain
RETRO68_TOOLCHAIN=/home/zigzagjoe/Retro68-Build/toolchain

CC = ${RETRO68_TOOLCHAIN}/bin/m68k-apple-macos-gcc
LD = ${RETRO68_TOOLCHAIN}/bin/m68k-apple-macos-ld

CSRC = $(wildcard drvr/*.c) $(wildcard init/*.c)
ASRC = decl/declrom.S

COMFLAGS = -c -march=68020 -mcpu=68020 -I ./include -I .
ASFLAGS = ${COMFLAGS}
CFLAGS  = ${COMFLAGS} -mpcrel -O3

# output files
COBJ = $(CSRC:%.c=obj/%.o)
SOBJ = $(ASRC:%.S=obj/%.o)
ROMFILE = obj/declrom.ROM

# targets
all: ${ROMFILE}

obj/%.o: %.S
	@mkdir -p $(@D)
	${CC} ${ASFLAGS} $< -o $@

obj/%.o: %.c
	@mkdir -p $(@D)
	${CC} ${CFLAGS} $< -o $@

${ROMFILE}: linker.ld ${COBJ} ${SOBJ} 
# linker script must be first
	${LD} -o $@.tmp -T $^ -Map $(ROMFILE:%.ROM=%.map)
# calculate the CRC
	python3 tool/gencrc.py --output $@ --pad ${ROMSIZE} $@.tmp

clean:
	rm -rf ./obj
