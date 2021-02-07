all : compile run

compile : shell.c 
	gcc shell.c -o shell

run :
	echo "For Interactive mode, type ./shell\n"
	echo "For Batch mode, type ./shell FILENAME\n"

clean: 
	rm -f shell