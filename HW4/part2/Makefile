all: matmul
matmul: main.cc matmul.cc
	mpicxx -o matmul main.cc matmul.cc  
clean:
	rm -f *.o matmul