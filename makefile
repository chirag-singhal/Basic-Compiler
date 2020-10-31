compile : 
	gcc -o output driver.c token.c print.c parsetree.c grammar.c typeExpression.c

run_t1 : compile
	./output test_cases/t1.txt

run_t2 : compile
	./output test_cases/t2.txt

run_t3 : compile
	./output test_cases/t3.txt

run_t4 : compile
	./output test_cases/t4.txt

run_t5 : compile
	./output test_cases/t5.txt

run_t6 : compile
	./output test_cases/t6.txt

end:
	rm ./output
