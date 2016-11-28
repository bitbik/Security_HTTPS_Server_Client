TARGET=server client
CFLAGS=-Wall
CC=gcc

all: $(TARGET)

server:  
	$(CC) $(CFLAGS) -o $@ -I "/import/linux/home/xchen119/Desktop/cs558/openssl-1.0.2-beta3/complied/include/" -L "/import/linux/home/xchen119/Desktop/cs558/openssl-1.0.2-beta3/complied/lib/" server.c -lssl -lcrypto -ldl 

client:  
	$(CC) $(CFLAGS) -o $@ -I "/import/linux/home/xchen119/Desktop/cs558/openssl-1.0.2-beta3/complied/include/" -L "/import/linux/home/xchen119/Desktop/cs558/openssl-1.0.2-beta3/complied/lib/" client.c -lssl -lcrypto -ldl 

clean:
	rm -rf *.o *.a $(TARGET)
