uart_rw:
	g++ -o data data_main.cpp data_send.cpp data.h
.PHONY: clean
clean:
	rm -f data *.o
