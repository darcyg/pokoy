#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "pokoy.h"
#include "http.h"
#include "router.h"

struct response req_handle(struct router *rtr, struct request *req);

void server_run(struct server *srv)
{
	int sock = socket_init(srv->port_no);

	struct sockaddr_in name;
	unsigned int name_len = sizeof(name);

	while (true) {
		int conn_fd = accept(
			sock,
			(struct sockaddr *) &name,
			&name_len
		);

		struct request req = req_parse(conn_fd);
		struct response res = req_handle(srv->router, &req);
		res_send(conn_fd, &res);

		close(conn_fd);
	}
}

struct response req_handle(struct router *rtr, struct request *req)
{
	for (int i = 0; i < rtr->matchers_n; ++i) {
		bool ok = route_match(rtr->matchers[i], req->path);

		if (ok) {
			struct response res = rtr->routes[i]();
			return res;
		}
	}

	struct response res = {
		.code = 404,
		.status = "Not Found",
		.body = "{}"
	};

	return res;
}
