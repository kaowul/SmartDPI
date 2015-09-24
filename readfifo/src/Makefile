readfifo: main.o  utils.o 
	$(CC) $(LDFLAGS) main.o  utils.o -o readfifo -luci

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c 

clean:
	rm -rf *.o readfifo
