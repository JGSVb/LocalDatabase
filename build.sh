FLAGS="-Wall -Wextra -Wpedantic"
CC="gcc"
OUT="main"

rm $OUT

$CC *.c $FLAGS -o $OUT 
ret=$?

if [ $ret != 0 ]
then
	printf "Impossível continuar. Código de saída: %d\n" $ret
	exit $ret
fi

if [ "$1" = "run" ]
then
	./$OUT
fi
