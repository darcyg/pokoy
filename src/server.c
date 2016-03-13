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
		sock = accept(
			sock,
			(struct sockaddr *) &name,
			&name_len
		);

		struct request req = req_parse(sock);
		struct response res = req_handle(srv->router, &req);
		res_send(sock, &res);
	}

	close(sock);
}

struct response req_handle(struct router *rtr, struct request *req)
{
	for (int i = 0; i < ROUTES_MAX; ++i) {
		bool ok = route_match(rtr->matchers[i], req->path);

		if (ok) {
			//rtr->routes[i]();
		}
	}

	struct response res = {
		.code = 200,
		.status = "OK",
		.body = "{ \"ohh\": \"yes\" }"
	};

	return res;
}
