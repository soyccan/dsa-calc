#!/bin/sh

cflags='-Werror -Wno-constant-logical-operand'

run_c() {
    expr=$1
    { echo '#include <stdio.h>\nint main(){printf("%d",'
      echo "$expr"
      echo ');}'; } | gcc -x c $cflags - 2>/dev/null && ./a.out
}

run_hw3() {
    expr=$1
    res="$(echo $expr | ../hw3_1 2>/dev/null)"
    [ $? = 0 ] && { echo "$res" | awk '{print $2}'; }
}

gcc -o rand rand.c
# make -C ..

while true; do
    expr=$(./rand)
    [ $? != 0 ] && continue

    out_c=$(run_c $expr)
    [ $? != 0 ] && continue

    out_hw3=$(run_hw3 $expr)
    [ $? != 0 ] && continue

    printf .
    if [ "$out_c" != "$out_hw3" ]; then
        echo
        echo Expression: $expr
        echo C outputs: $out_c
        echo hw3 outputs: $out_hw3
        break
    fi
done