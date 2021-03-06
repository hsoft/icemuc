CC = cc
LIBRARY_NAME = icemu
TARGET_LIB = lib$(LIBRARY_NAME).a
TEST_TARGET = test_icemu

PKGCONFIG_LIBS = ncurses
CFLAGS = -Wall `pkg-config --cflags $(PKGCONFIG_LIBS)`

SRCS = pin.c pinlist.c chip.c shiftregister.c decoder.c counter.c \
	gate.c led.c mcu.c util.c ui.c sim.c
OBJS = $(addprefix src/, $(SRCS:%.c=%.o))
TEST_SRCS = main.c pin.c shiftregister.c decoder.c counter.c \
	gate.c circuit.c util.c
TEST_OBJS = $(addprefix tests/, $(TEST_SRCS:%.c=%.o))
CUNIT_LINKING ?= `pkg-config --libs cunit`

.PHONY: all
all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	ar rcs $@ $^

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(OBJS) $(TARGET_LIB) $(TEST_OBJS) $(TEST_TARGET)


$(TEST_TARGET): PKGCONFIG_LIBS += cunit
$(TEST_TARGET): $(TARGET_LIB) $(TEST_OBJS)
	$(CC) -static -L. -L/usr/lib64 -o $@ $(TEST_OBJS) -l$(LIBRARY_NAME) $(CUNIT_LINKING)

.PHONY: test
test: $(TEST_TARGET)
	./$(TEST_TARGET)
