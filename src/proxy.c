#include <assert.h>
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
  ZMQ_CHECK(zmq_proxy(frontend, backend, NULL));

  //  We never get here...
  zmq_close(frontend);
  zmq_close(backend);
  zmq_ctx_destroy(context);
  return 0;
}
