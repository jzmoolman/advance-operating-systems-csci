yazh:	yazh.l yazh.y symtab.h main.c
	bison -d yazh.y
	flex -o yazh.lex.c yazh.l
	clang -o $@ yazh.tab.c yazh.lex.c main.c
