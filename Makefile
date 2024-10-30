pipe: complile_pipe run_pipe clean_pipe

complile_pipe: pipes.c
	gcc pipes.c -o redir

run_pipe: 
	./redir input.txt "wc -l" output.txt

clean_pipe: 
	rm redir