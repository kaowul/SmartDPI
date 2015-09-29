readfifo: main.o  utils.o conf.o
	$(CC) $(LDFLAGS) main.o  utils.o conf.o -o readfifo -luci

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c 

conf.o: conf.c
	$(CC) $(CFLAGS) -c conf.c 
clean:
	rm -rf *.o readfifo
