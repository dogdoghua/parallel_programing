all: matmul
matmul: main.cc matmul.cc
	mpicxx main.cc matmul.cc -o matmul 
clean:
	rm -f *.o matmul