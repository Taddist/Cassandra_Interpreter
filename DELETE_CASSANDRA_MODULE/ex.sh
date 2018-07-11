flex -i cassandra.l
gcc -c lex.yy.c
gcc -c cassandra.c
gcc -c tabsymb.c -ljansson
gcc -c error.c
gcc -o zzc tabsymb.o error.o lex.yy.o cassandra.o -lfl -ljansson
./zzc
