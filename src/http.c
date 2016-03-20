#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "http.h"
#include "date.h"

void parse_req_line(struct request *req, char *req_line);

int socket_init(const char *port)
{
	int sock_fd;
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM,
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, port, &hints, &res);
	sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock_fd == -1) {
		perror("Bad socket");
		exit(EXIT_FAILURE);
	}

	int bind_result = bind(sock_fd, res->ai_addr, res->ai_addrlen);
	if (bind_result < 0) {
		perror("Could not bind");
		exit(EXIT_FAILURE);
	}

	int8_t listen_result = listen(sock_fd, 5);
	if (listen_result < 0) {
		perror("Could not listen");
		exit(EXIT_FAILURE);
	}

	return sock_fd;
}

void parse_req_line(struct request *req, char *req_line)
{
	char *method = strtok(req_line, " ");
	if (method == NULL) {
		perror("Could not read method");
		exit(EXIT_FAILURE);
	}

	char *url = strtok(NULL, " ");
	if (url == NULL) {
		perror("Could not read path");
		exit(EXIT_FAILURE);
	}

	struct query_pair q_pairs[32];
	int q_pairs_n = 0;
	char path[2083 + 1];
	// Check if there is a query string
	if (index(url, '?') == NULL) {
		// Nope, there was not, so we'll assume the URL is the path
		memcpy(path, &url[0], strlen(url));
	} else {
		// Yes, there was, so we'll have to split the query into pairs

		size_t qs_start = strcspn(url, "?") + 1;
		char qs[2083 + 1];

		size_t qs_len = strlen(url) - qs_start;
		memcpy(qs, &url[qs_start], qs_len);
		qs[qs_len] = '\0';

		memcpy(path, &url[0], qs_start);
		path[qs_start] = '\0';

		char *pairs[32];

		// Split the query string into key-value pairs
		if (index(qs, '&') == NULL) {
			pairs[0] = qs;
			q_pairs_n = 1;
		} else {
			char *pair = strtok(qs, "&");
			do {
				pairs[q_pairs_n] = pair;
				++q_pairs_n;
			} while ((pair = strtok(NULL, "&")) != NULL);
		}

		// Split the pairs and assign them to the request
		for (int i = 0; i < q_pairs_n; ++i) {
			size_t delim_pos = strcspn(pairs[i], "=");

			size_t key_len = delim_pos;
			size_t value_len = strlen(pairs[i]) - 1 - delim_pos;

			struct query_pair qp;
			memcpy(qp.key, &(pairs[i])[0], key_len);
			memcpy(qp.value, &(pairs[i])[delim_pos] + 1, value_len);

			q_pairs[i] = qp;
		}
	}

	strncpy(req->method, method, sizeof(req->method) - 1);
	strncpy(req->path, path, sizeof(req->path) - 1);
	req->query_pairs = q_pairs;
	req->query_pairs_n = q_pairs_n;
}

void res_send(int client, struct response *res)
{
	const char *res_tmpl =
		"HTTP/1.1 200 OK\r\n"
		"Date: %s\r\n"
		"Connection: close\r\n"
		"Server: Pokoy\r\n"
		"Accept-Ranges: bytes\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: %d\r\n"
		"Last-Modified: %s\r\n"
		"\r\n"
		"%s";

	char date[POKOY_DATE_LEN];
	rfc2822date(date);

	const char *body = res->body;
	uint16_t body_len = strlen(body);

	char *final_res = calloc(1000, sizeof(char));
	sprintf(final_res, res_tmpl, date, body_len, date, body);

	send(client, final_res, strlen(final_res), 0);

	free(final_res);

	close(client);
}

/**
 * Read a request and parse it into req
 *
 * Headers with GET requests are not supported!
 */
void req_parse(int sock, struct request *req)
{
	size_t buf_len = 1024;
	char buf[buf_len];

	req->headers_n = 0;

	char c = '\0';
	size_t i = 0;
	size_t prev_nl = 0;
	while (i < buf_len - 1) {
		int n = recv(sock, &c, 1, 0);
		if (n == 0) {
			return;
		}

		if (c == '\n') {
			// Handle the first line
			if (prev_nl == 0) {
				if (strncmp(buf, "GET", 3) == 0) {
					return;
				}

				parse_req_line(req, buf);
			}
			// Handle the change from head to body
			else if (i == prev_nl + 2) {
				break;
			}
			// Handle headers
			else {
				// -1 to trim the final \r
				int h_len = i - prev_nl - 1;
				char *header = calloc(h_len, sizeof(char));
				memcpy(header, &buf[prev_nl + 1], h_len - 1);
				header[h_len] = '\0';

				req->headers[req->headers_n] = header;
				req->headers_n++;
			}

			prev_nl = i;
		}

		buf[i] = c;
		i++;
	}

	for (size_t i = 0; i < req->headers_n; ++i) {
		if (strncmp(req->headers[i], "Content-Length", 14) != 0) {
			continue;
		}

		sscanf(req->headers[i], "Content-Length: %zu+", &req->body_len);
	}

	for (size_t i = 0; i < req->body_len; i++) {
		recv(sock, &req->body[i], 1, 0);
	}
	req->body[req->body_len] = '\0';
}
