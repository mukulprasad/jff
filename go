#!/bin/bash

rm -f ./dat/*
rm -f ll.log
rm -f bulk.op today.op

./jff ./tt >> ll.log
./jffsort ./dat >>ll.log
./jfftech ./dat >>ll.log
./jffreport ./dat >>ll.log


