#include "constants.h"
#include "zhelpers.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <zmq.h>

int main() {
  // ZMQ code inspired from
  // https://riptutorial.com/zeromq/example/29221/zeromq-hello-world-example-
  srand(0);

  printf("Connecting to the proxy\n");
  void *context = zmq_ctx_new();
  void *requester = zmq_socket(context, ZMQ_REQ);
  char proxy_frontend_addr[25];
  sprintf(proxy_frontend_addr, "tcp://localhost:%s", PROXY_FRONTEND_PORT);
  ZMQ_CHECK(zmq_connect(requester, proxy_frontend_addr));

  char nth_prime_buffer[8];
  char nth_prime_response_buffer[15];
  for (int i = 0; i < N_MAX_CLIENT_REQUESTS; i++) {
    sprintf(nth_prime_buffer, "%d", 2000 + rand() % 2000);
    printf("Requesting nth_prime_buffer=%s\n", nth_prime_buffer);
    zmq_send(requester, nth_prime_buffer, 6, 0);
    zmq_recv(requester, nth_prime_response_buffer, 15, 0);
    printf("Received %sth prime = %s\n", nth_prime_buffer,
           nth_prime_response_buffer);
  }

  zmq_close(requester);
  zmq_ctx_destroy(context);
  return 0;
}
