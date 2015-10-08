readfifo: main.o  utils.o conf.o parse_dpi.o
	$(CC) $(LDFLAGS) main.o  utils.o conf.o parse_dpi.o -o readfifo -luci

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c 

conf.o: conf.c
	$(CC) $(CFLAGS) -c conf.c 

parse_dpi.o: parse_dpi.c
	$(CC) $(CFLAGS) -c parse_dpi.c 
clean:
	rm -rf *.o readfifo
