CC := gcc

OUTPUT		= main 
SRCS		= main.c f2d.c

CFLAGS		= -Wall

$(OUTPUT) : $(SRCS)
	$(CC) $(SRCS) -o $(OUTPUT) $(CFLAGS)

clean:
	rm -f $(OUTPUT)
