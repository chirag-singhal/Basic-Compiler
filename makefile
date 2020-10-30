compile : 
	gcc -o output driver.c token.c print.c parsetree.c grammar.c typeExpression.c

run_t1 : compile
	./output t1.txt

run_t2 : compile
	./output t2.txt

run_t3 : compile
	./output t3.txt

run_t4 : compile
	./output t4.txt

run_t5 : compile
	./output t5.txt

run_t6 : compile
	./output t6.txt

end:
	rm ./output
