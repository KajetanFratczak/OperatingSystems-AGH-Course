CC = gcc
CFLAGS = -Wall -std=c17

all: static_exec shared_exec dynamic_exec

# Automatic Variables
# < - aktualnie przetwarzany plik z listy składników
# @ - nazwa pliku docelowego
# ^ - składniki

# Wyjaśnienie z makefiletutorial.com
# # Outputs "hey", since this is the target name
#	echo $@
#
#	# Outputs all prerequisites newer than the target
#	echo $?
#
#	# Outputs all prerequisites
#	echo $^
#
#	# Outputs the first prerequisite
#	echo $<

# Kompilacja pliku collatz.c do pliku obiektowego collatz.o
collatz.o: collatz.c collatz.h
	$(CC) $(CFLAGS) -c $<
# $< = collatz.c

# Utworzenie biblioteki statycznej
libcollatz.a: collatz.o
	ar rcs $@ $^
# $@ = libcollatz.o
# $^ = collatz.o

# Utworzenie biblioteki współdzielonej
libcollatz.so: collatz.c collatz.h
	$(CC) $(CFLAGS) -fPIC -shared $< -o $@
# $< = collatz.c
# $@ = libcollatz.so

# Kompilacja pliku client.c do pliku obiektowego client.o
client.o: client.c
	$(CC) $(CFLAGS) -c $<

# Kompilacja programu korzystającego z biblioteki statycznej
static_exec: client.o libcollatz.a
	$(CC) $(CFLAGS) -o static_main $^
# $^ = client.o libcollatz.a

# Kompilacja programu korzystającego z biblioteki współdzielonej
shared_exec: client.o libcollatz.so
	$(CC) $(CFLAGS) -o shared_main $^ -L. -lcollatz
# $^ = client.o libcollatz.so

# Kompilacja programu korzystającego z biblioteki dynamicznie linkowanej
dynamic_exec: client.o libcollatz.so
	$(CC) $(CFLAGS) -o dynamic_main $^ -DDYNAMIC_LOAD -ldl
# $^ = client.o libcollatz.so

clean:
	rm -f *.o *.a *.so static_main shared_main dynamic_main

.PHONY: all clean static_exec shared_exec dynamic_exec

