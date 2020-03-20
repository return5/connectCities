#!/bin/bash
#compile script for connect_cities
#compiles connect_cities and creates executable named 'connect_cities'
#defaults to using gcc, but uses clang if user supplies '-c' or '--clang' as argument to script. 
#use argumnet --help to bring up help info

#prints the help message to terminal.
printHelp() {
	printf "compile script for connect_cities\n"
	printf "compiles connectcities and creates executable names 'connect_cities'\n"
	printf "defaults to using gcc, but uses clang if user supplies '-c' to script.\n\n"
	printf "options:\n"
	printf " -c   --clang 	use clang instead of gcc\n"
	printf "-help  --help 	prints this page then exits\n"
}

#checks if GCC is installed, and if so it compiles the game with given flags.  if not installed then gives error message and exits.
useGCC() {
	(gcc -v >/dev/null 2>&1 && 
	printf "using GCC\n" &&
	gcc connectcities.c -orogueclone -Wall -Wextra -O2 -lm -Iheaders -finline-functions -Wswitch-enum -lncurses -std=gnu11 -oconnect_cities ) ||
	printf "error, gcc seems to not be installed.\n"
}

#compiles game using Clang
useClang() {
	printf "using Clang\n"
	clang connectcities.c -Iheaders -Wall -Wextra -O2 -lm -lncurses -finline-functions -Wswitch-enum -std=gnu11 -oconnect_cities   
}

main() {
	case "$1" in
		-help | --help)
			printHelp && exit
			;;
		-c | --clang)
			(clang -v >/dev/null 2>&1 && useClang) || (printf "falling back to GCC.\n" && useGCC)
			;;
		*)
			useGCC
			;;
	esac
}

main "$1"
