SRCDIR = modules
INCDIR = include
BINDIR = bin 





monitor: monitor.o utils.o
	$(CC) monitor.o utils.o -o monitor
test: test_utils.o utils.o
	$(CC) test_utils.o utils.o -o test



monitor.o: monitor.c utils.h
	$(CC) -c monitor.c
utils.o: utils.c utils.h
	$(CC) -c utils.c
test_utils.o: test_utils.c utils.h acutest.h
	$(CC) -c test_utils.c




clean:
   -rm *.o *~ core paper