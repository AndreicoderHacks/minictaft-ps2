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
EE_LDFLAGS =
EE_LIBS    = -lgskit -ldmakit -lpad -lc -lm -lgcc

EE_INCS = -Isrc

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
