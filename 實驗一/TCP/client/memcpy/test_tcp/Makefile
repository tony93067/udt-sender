all: client background_client_downlink background_client_uplink

% : %.c
	gcc -o $@ $< -lnsl -lpthread
clean:
	rm -f client
	rm -f background_client_downlink
	rm -f background_client_uplink
