CCFLAGS := -W

SOURCES := \
	frame.c

all: frame
.PHONY: lint
lint: \
	*.{h,c}

.PHONY: clean
clean:
	-rm -f *.o *.d

frame: $(SOURCES:.c=.o)
	$(CC) $(CCFLAGS) -o $@ $^
