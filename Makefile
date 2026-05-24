# Makefile - Minicraft PS2
# Requires: ps2dev SDK + gsKit installed
# Setup: https://github.com/ps2dev/ps2dev
#
# Build:  make
# Output: minicraft.elf  (run via FreeMcBoot, OPL, or PCSX2)

EE_BIN = minicraft.elf

EE_OBJS = \
	src/main.o \
	src/world.o \
	src/player.o \
	src/entity.o \
	src/render.o \
	src/input.o \
	src/crafting.o \
	src/particles.o

EE_CFLAGS  = -O2 -Wall -G0
EE_LDFLAGS = -L$(PS2SDK)/ee/lib -L$(GSKIT)/lib

EE_LIBS = \
	-lgskit \
	-ldmakit \
	-lpad \
	-lc \
	-lm \
	-lpthread \
	-lkernelstubs \
	-lgcc

EE_INCS = \
	-I$(PS2SDK)/ee/include \
	-I$(PS2SDK)/common/include \
	-I$(GSKIT)/include \
	-Isrc

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

include $(PS2SDK)/Makefile.pref
include $(PS2SDK)/Makefile.eeglobal
