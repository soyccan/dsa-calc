#!/bin/sh

# disassemble next line
printf "target stop-hook add\nbt\ndi --pc\nDONE\n" > .lldbinit

printf "$1" >> .lldbinit
shift
lldb -s .lldbinit $@