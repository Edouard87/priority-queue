compiler = gcc
tester_binary = ./bin/tester
library_binary = ./bin/priority-queue
library_dependencies = ./lib/priority-queue.c
tester_dependencies = ./tests/tester.c
compiler_args = -g3 -lcunit -v -Q -lm -ggdb3

test:
	$(compiler) $(library_dependencies) $(tester_dependencies) -o $(tester_binary).out $(compiler_args) && $(tester_binary).out

memcheck:
	$(compiler) $(library_dependencies) $(tester_dependencies) -o $(tester_binary).out $(compiler_args) && valgrind --leak-check=full --track-origins=yes -v $(tester_binary).out

clean:
	rm -r ./bin/*.out

check:
	chmod +x $(PWD)/scripts/check.sh && $(PWD)/scripts/check.sh

build:
	gcc -c -Wall -Werror -fpic ./lib/priority-queue.c 

profile: 
	$(compiler) $(library_dependencies) $(tester_dependencies) -o $(tester_binary).out $(compiler_args) && valgrind --leack-check
	