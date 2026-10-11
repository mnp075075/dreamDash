TARGET = gdCast.elf
OBJS = src/game.o romdisk.o
KOS_ROMDISK_DIR = romdisk

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean: rm-elf
	-rm -f $(OBJS)

rm-elf:
	-rm -f $(TARGET)

$(TARGET): $(OBJS)
	kos-c++ -o $(TARGET) $(OBJS) -lkosutils -lkmg $(KOS_LIBS)

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)