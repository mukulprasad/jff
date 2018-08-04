#!/bin/bash
gcc -Wall -g jffmain.c jffrawproc.c -o jff
gcc -Wall -g -O2 jfftech.c -o jfftech -lm
gcc -Wall -g -O2 jffsort.c -o jffsort -lm
gcc -Wall -g -O2 jffreport.c -o jffreport -lm

#mv jff ./tt
#mv jfftech ./tt
#mv jffsort ./tt

