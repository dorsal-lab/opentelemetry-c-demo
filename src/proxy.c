#include <assert.h>
#include <stdio.h>
#include <zmq.h>

#include "constants.h"
#include "zhelpers.h"

int main() {
	// ZMQ code inspired from
	// https://github.com/booksbyus/zguide/blob/master/examples/C/msgqueue.c
	void *context = zmq_ctx_new();

	printf("Starting proxy...\n");

	//  Socket facing clients
	void *frontend = zmq_socket(context, ZMQ_ROUTER);
	char proxy_frontend_addr[25];
	sprintf(proxy_frontend_addr, "tcp://*:%s", PROXY_FRONTEND_PORT);
	printf("Binding proxy to frontend...\n");
	ZMQ_CHECK(zmq_bind(frontend, proxy_frontend_addr));

	//  Socket facing services
	void *backend = zmq_socket(context, ZMQ_DEALER);
	char proxy_backend_addr[25];
	sprintf(proxy_backend_addr, "tcp://*:%s", PROXY_BACKEND_PORT);
	printf("Binding proxy to backend...\n");
	ZMQ_CHECK(zmq_bind(backend, proxy_backend_addr));

	//  Start the proxy
	//  Initialize poll set
	zmq_pollitem_t items[] = {{frontend, 0, ZMQ_POLLIN, 0},
	                          {backend, 0, ZMQ_POLLIN, 0}};
	int count = 0;
	while (1) {
		zmq_msg_t message;
		zmq_poll(items, 2, -1);
		if (items[0].revents & ZMQ_POLLIN) {
			while (1) {
				//  Process all parts of the message
				zmq_msg_init(&message);
				zmq_msg_recv(&message, frontend, 0);
				int more = zmq_msg_more(&message);
				zmq_msg_send(&message, backend, more ? ZMQ_SNDMORE : 0);
				zmq_msg_close(&message);
				if (!more) {
					break; //  Last message part
				}
			}
			count++;
			printf("%d\n", count);
		}
		if (items[1].revents & ZMQ_POLLIN) {
			while (1) {
				//  Process all parts of the message
				zmq_msg_init(&message);
				zmq_msg_recv(&message, backend, 0);
				int more = zmq_msg_more(&message);
				zmq_msg_send(&message, frontend, more ? ZMQ_SNDMORE : 0);
				zmq_msg_close(&message);
				if (!more) {
					break; //  Last message part
				}
			}
		}
	}

	//  We never get here...
	zmq_close(frontend);
	zmq_close(backend);
	zmq_ctx_destroy(context);
	return 0;
}
