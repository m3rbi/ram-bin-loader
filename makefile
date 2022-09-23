SOURCES=src/channel.c src/handler.c src/main.c src/message.c src/run_detached.c src/server.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=proc_server

all: $(EXECUTABLE) clean 

$(EXECUTABLE): $(OBJECTS)
	gcc $(OBJECTS) -o $@ -g

.c.o:
	gcc -g -c $< -o $@

clean:
	rm -rf $(OBJECTS)
