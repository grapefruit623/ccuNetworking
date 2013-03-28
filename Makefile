cc=gcc
server: hw1.c
	$(cc) hw1.c -o myServer
run:
	$(CURDIR)/myServer
clean:
	rm myServer
