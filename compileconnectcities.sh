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

#use GCC to compile the game
useGCC() {
	printf "using GCC\n"
	gcc connectcities.c -orogueclone -Wall -Wextra -O2 -Iheaders -finline-functions -Wswitch-enum -lncurses -std=gnu11 -oconnect_cities
}

#compiles game using Clang
useClang() {
	printf "using Clang\n"
	clang connectcities.c -Iheaders -Wall -Wextra -O2 -lncurses -finline-functions -Wswitch-enum -std=gnu11 -oconnect_cities   
}

main() {
    case "$1" in                #check argument of positional parameter 1
        -help | --help)         #if argument passed to script is the help flag
            printHelp && exit   #run the help function then exit
            ;;
        -c | --clang)           #if argument passed is the clang flag
            #check if clang is installed, if it is, call the useClang function, other wise fallback to GCC
            (clang -v >/dev/null 2>&1 && useClang) || (printf "falling back to GCC.\n" && (gcc -v >/dev/null 2>&1 && useGCC) || printf "error, make sure GCC is installed.\n")   
            ;;
        *)
            #checks to see if gcc is installed, and if so call useGCC, otherwise print error
            (gcc -v >/dev/null 2>&1 && useGCC) || printf "error, make sure GCC is installed.\n"
            ;;
    esac
}

#call main function and pass it positonal paramter 1
main "$1"
