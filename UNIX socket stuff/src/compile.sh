#!/bin/bash

for FILE in $(find  *.c)
	do
		#$name = echo $FILE | sed s/.c//g
		#echo $FILE
		$ base = ${FILE%.${ext}}
		echo $base
		
	done
