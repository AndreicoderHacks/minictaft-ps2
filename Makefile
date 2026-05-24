EE_BIN = minicraft.elf

SRCS = main.c world.c player.c entity.c \
       render.c input.c crafting.c particles.c

OBJS = $(SRCS:.c=.o)

PS2SDK ?= /usr/local/ps2dev/ps2sdk
GSKIT  ?= /usr/local/ps2dev/gsKit

CC      = mips64r5900el-ps2-elf-gcc
CFLAGS  = -O2 -Wall -G0 -D_EE \
          -I$(PS2SDK)/ee/include \
          -I$(PS2SDK)/common/include \
          -I$(GSKIT)/include \
          -I.

LDFLAGS = -nostartfiles \
          -T$(PS2SDK)/ee/startup/linkfile \
          -L$(PS2SDK)/ee/lib \
          -L$(GSKIT)/lib

LIBS = -lgskit -ldmakit -lpad -lc -lkernel -lcglue -lm -lgcc

all: $(EE_BIN)

$(EE_BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EE_BIN)
