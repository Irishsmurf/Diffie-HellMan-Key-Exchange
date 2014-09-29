#!/bin/bash

echo "lolcompile.."
for i in $(find *.c) 
	do
		gcc -o ../bin/${i%.*} $i;
	done

