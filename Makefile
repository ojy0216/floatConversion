CC := gcc

OUTPUT		= main 
SRCS		= main.c fConvert.c atoh.c

CFLAGS		= -Wall

$(OUTPUT) : $(SRCS)
	$(CC) $(SRCS) -o $(OUTPUT) $(CFLAGS)

clean:
	rm -f $(OUTPUT)
