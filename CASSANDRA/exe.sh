while true
do
	echo -n "CQL >> "
	flex -i  cassandra.l 
	gcc -c lex.yy.c 
	gcc -c cassandra.c
	gcc -c operations/keyspaces/op_k.c
	gcc -c operations/tables/op_t.c -ljansson
	gcc -c operations/CURD/curd.c -ljansson
	gcc -c error.c
	gcc -c AST.c
	gcc -o cql lex.yy.o cassandra.o  op_k.o op_t.o curd.o error.o AST.o -lfl -ljansson
	./cql
done