webserv: webserv.c
	gcc webserv.c -o webserv
	gcc skyler_server.c -o skyler
	gcc varun_web.c -o varun
	gcc plot.c -o plot.cgi
	chmod 755 ls.cgi
	gcc my-histogram.c -o hist

clean:
	rm webserv