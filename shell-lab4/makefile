yazh:	posixfunc.c yazh.l yazh.y symtab.c main.c
	bison -d yazh.y
	flex -o yazh.lex.c yazh.l
	clang -o $@ posixfunc.c yazh.tab.c yazh.lex.c symtab.c main.c
