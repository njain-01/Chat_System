compile: server.c client.c
	gcc -o server server.c
	gcc -o client client.c
run: 
	./server
clean:
	rm -f server
	rm -f client
