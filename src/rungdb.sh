#!/bin/sh
echo reset halt | nc -N localhost 4444
arm-none-eabi-gdb -x gdb_target "$@"
