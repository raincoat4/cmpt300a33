
make:
	gcc -c avltree.c hashmap.c main.c
	gcc -o test avltree.o hashmap.o main.o
clean:
	rm *.o test