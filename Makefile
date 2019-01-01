all:
	gcc backdoor.c utils.c -o backdoor

clean:
	rm ./backdoor