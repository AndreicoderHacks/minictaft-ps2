EE_BIN = minicraft.elf

SRCS = src/main.c src/world.c src/player.c src/entity.c \
       src/render.c src/input.c src/crafting.c src/particles.c

OBJS = $(SRCS:.c=.o)

PS2SDK ?= /usr/local/ps2dev/ps2sdk
GSKIT  ?= /usr/local/ps2dev/gsKit

CC      = mips64r5900el-ps2-elf-gcc
CFLAGS  = -O2 -Wall -G0 -D_EE \
          -I$(PS2SDK)/ee/include \
          -I$(PS2SDK)/common/include \
          -I$(GSKIT)/include \
          -Isrc

LDFLAGS = -nostartfiles \
          -T$(PS2SDK)/ee/startup/linkfile \
          -L$(PS2SDK)/ee/lib \
          -L$(GSKIT)/lib

LIBS = -lgskit -ldmakit -lpad -lc -lm -lgcc -lkernel

all: $(EE_BIN)

$(EE_BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EE_BIN)
