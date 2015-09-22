test: test.c 
	gcc  -o test test.c 
test2: test2.c 
	gcc  -o test2 test2.c 
clean: 
	rm -rf *.o test
