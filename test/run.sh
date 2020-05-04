#!/bin/sh

cflags='-Werror -Wno-constant-logical-operand'

run_c() {
    expr=$1
    { echo '#include<stdio.h>\n#include<math.h>\nint main(){printf("%f",(double)'
      echo "$expr"
      echo ');}'; } | gcc -xc $cflags - -lm 2>/dev/null && ./a.out
}

run_hw3_1() {
    expr=$1
    res="$(echo $expr | ../hw3_1 2>/dev/null)"
    [ $? = 0 ] && { echo "$res" | awk '{print $2}'; }
}

run_hw3_2() {
    expr=$1
    res="$(echo $expr | ../hw3_2 2>/dev/null)"
    [ $? = 0 ] && { echo "$res" | awk '{print $2}'; }
}

gcc -o rand rand.c
# make -C ..

while true; do
    expr=$(./rand $1)
    [ $? != 0 ] && continue

    out_c=$(run_c $expr)
    [ $? != 0 ] && continue

    if [ "$1" = '1' ]; then
        out_hw3=$(run_hw3_1 $expr)
    else
        out_hw3=$(run_hw3_2 $expr)
    fi
    [ $? != 0 ] && continue

    printf .
    if [ "$out_c" != "$out_hw3" ] \
            && [ "$out_c" != -"$out_hw3" ] \
            && [ -"$out_c" != "$out_hw3" ]; then
        echo
        echo Expression: $expr
        echo C outputs: $out_c
        echo hw3 outputs: $out_hw3
        break
    fi
done