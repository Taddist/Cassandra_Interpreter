flex -i  cassandra.l 
gcc -c lex.yy.c 
gcc -c cassandra.c tabsymb.c error_cassandra.c -ljansson
gcc -o cql lex.yy.o cassandra.o tabsymb.o error_cassandra.o -lfl -ljansson
./cql 
