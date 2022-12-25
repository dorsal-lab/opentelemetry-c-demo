#include "constants.h"
#include "zhelpers.h"

#include <opentelemetry_c.h>
#include <zmq.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  init_tracing("otel-experiment-client", "0.0.1", "", "machine-client-0.0.1");
  // ZMQ code inspired from
  // https://riptutorial.com/zeromq/example/29221/zeromq-hello-world-example-

  printf("[client] Connecting to the proxy\n");
  void *context = zmq_ctx_new();

  // Socket to communicate connection message to proxy
  void *connecter = zmq_socket(context, ZMQ_DEALER);
  char proxy_connection_addr[25];
  sprintf(proxy_connection_addr, "tcp://localhost:%s",
          PROXY_CONNECTION_MANAGEMENT_PORT);
  ZMQ_CHECK(zmq_connect(connecter, proxy_connection_addr));
  s_send(connecter, "connect");

  // Socket to communicate with servers
  void *requester = zmq_socket(context, ZMQ_REQ);
  char proxy_frontend_addr[25];
  sprintf(proxy_frontend_addr, "tcp://localhost:%s", PROXY_FRONTEND_PORT);
  ZMQ_CHECK(zmq_connect(requester, proxy_frontend_addr));

  void *tracer = get_tracer();

  for (int i = 1; i < 10000; i += 10000 / N_MAX_CLIENT_REQUESTS) {
    void *span = start_span(tracer, "get-nth-prime", SPAN_KIND_CLIENT, "");
    char *span_ctx = extract_context_from_current_span(span);

    char nth[8];
    sprintf(nth, "%d", i);
    printf("[client] Requesting nth_prime_buffer=%s\n", nth);
    s_sendmore(requester, span_ctx);
    s_send(requester, nth);
    // Receive remote context, but ignore it
    free(s_recv(requester));
    char *nth_prime = s_recv(requester);
    printf("[client] Received %sth prime = %s\n", nth, nth_prime);

    free(span_ctx);
    free(nth_prime);

    end_span(span);
  }

  s_send(connecter, "disconnect");

  zmq_close(connecter);
  zmq_close(requester);
  zmq_ctx_destroy(context);
  return 0;
}
