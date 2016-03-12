#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <ctype.h>

#include <sys/socket.h>
#include <netinet/in.h>

int startup(uint16_t port);
void process_request(int client);
int get_line(int sock, uint8_t *buf, size_t size);

void serve_http(uint16_t port_no)
{
	int server_sock = startup(port_no);

	puts("Listening");

	struct sockaddr_in client_name;
	unsigned int client_name_len = sizeof(client_name);
	while (true) {
		puts("Waiting for request");

		int client_sock = accept(
			server_sock,
			(struct sockaddr *) &client_name,
			&client_name_len
		);

		puts("Handling request");

		process_request(client_sock);
	}

	close(server_sock);
}

int startup(uint16_t port)
{
	int descr = 0;
	struct sockaddr_in name;

	descr = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(descr, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
	if (descr == -1) {
		perror("Bad socket");
		exit(EXIT_FAILURE);
	}

	memset(&name, 0, sizeof(name));
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	int bind_result = bind(descr, (struct sockaddr *)&name, (socklen_t) sizeof(name));
	if (bind_result < 0) {
		perror("Could not bind");
		exit(EXIT_FAILURE);
	}

	int8_t listen_result = listen(descr, 5);

	if (listen_result < 0) {
		perror("Could not listen");
		exit(EXIT_FAILURE);
	}

	return descr;
}

void process_request(int client)
{
	uint8_t buf[1024];

	get_line(client, buf, sizeof(buf));

	uint8_t method[255];
	uint16_t i = 0, j = 0;
	while (!isspace(buf[j]) && i < sizeof(method) - 1) {
		method[i] = buf[j];
		++i;
		++j;
	}
	method[i] = '\0';

	uint8_t url[255];
	while (j < sizeof(buf) && isspace(buf[j])) {
		++j;
	}

	i = 0;
	while (i < sizeof(url) - 1 && j < sizeof(buf) && !isspace(buf[j])) {
		url[i] = buf[j];
		i++;
		j++;
	}
	url[i] = '\0';

	printf("%s: %s\n", method, url);

	const char *response =
		"HTTP/1.1 200 OK\r\n"
		"Date: Thu, 20 May 2004 21:12:58 GMT\r\n"
		"Connection: close\r\n"
		"Server: Pokoy\r\n"
		"Accept-Ranges: bytes\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 12\r\n"
		"Last-Modified: Thu, 20 May 2004 21:12:58 GMT\r\n"
		"\r\n"
		"{ \"yes\": 1 }";

	send(client, response, strlen(response), 0);

	close(client);
}

int get_line(int sock, uint8_t *buf, size_t size)
{
	uint8_t c = '\0';
	uint16_t i = 0;
	while (i < size - 1 && c != '\n') {
		int n = recv(sock, &c, 1, 0);
		if (n > 0) {
			if (c == '\r') {
				n = recv(sock, &c, 1, MSG_PEEK);
				if (n > 0 && c == '\n') {
					recv(sock, &c, 1, 0);
				} else {
					c = '\n';
				}
			}
			buf[i] = c;
			i++;
		} else {
			c = '\n';
		}
	}

	buf[i] = '\0';

	return i;
}
