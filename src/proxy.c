#include "constants.h"
#include "zhelpers.h"

#include <opentelemetry_c.h>
#include <zmq.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

int main() {
  init_tracing("otel-experiment-proxy", "0.0.1", "", "machine-proxy-0.0.1");

  // ZMQ code inspired from
  // https://github.com/booksbyus/zguide/blob/master/examples/C/msgqueue.c
  void *context = zmq_ctx_new();

  printf("[proxy ] Starting proxy...\n");

  //  Socket to manage clients connections
  void *connector = zmq_socket(context, ZMQ_ROUTER);
  char proxy_connector_addr[25];
  sprintf(proxy_connector_addr, "tcp://*:%s", PROXY_CONNECTION_MANAGEMENT_PORT);
  printf("[proxy ] Binding proxy to connector...\n");
  ZMQ_CHECK(zmq_bind(connector, proxy_connector_addr));

  //  Socket to manage proxy termination
  void *terminator = zmq_socket(context, ZMQ_PUB);
  char proxy_terminator_addr[25];
  sprintf(proxy_terminator_addr, "tcp://*:%s",
          PROXY_TERMINATION_MANAGEMENT_PORT);
  printf("[proxy ] Binding proxy to terminator...\n");
  ZMQ_CHECK(zmq_bind(terminator, proxy_terminator_addr));

  //  Socket facing clients
  void *frontend = zmq_socket(context, ZMQ_ROUTER);
  char proxy_frontend_addr[25];
  sprintf(proxy_frontend_addr, "tcp://*:%s", PROXY_FRONTEND_PORT);
  printf("[proxy ] Binding proxy to frontend...\n");
  ZMQ_CHECK(zmq_bind(frontend, proxy_frontend_addr));

  //  Socket facing services
  void *backend = zmq_socket(context, ZMQ_DEALER);
  char proxy_backend_addr[25];
  sprintf(proxy_backend_addr, "tcp://*:%s", PROXY_BACKEND_PORT);
  printf("[proxy ] Binding proxy to backend...\n");
  ZMQ_CHECK(zmq_bind(backend, proxy_backend_addr));

  // Create a tracer object
  void *tracer = get_tracer();

  //  Initialize poll set
  zmq_pollitem_t items[] = {{frontend, 0, ZMQ_POLLIN, 0},
                            {backend, 0, ZMQ_POLLIN, 0},
                            {connector, 0, ZMQ_POLLIN, 0}};

  // ROUTER to DEALER pattern
  // https://zguide.zeromq.org/docs/chapter3/#advanced-request-reply
  int n_connected_clients = 0;
  int message_sent_from_frontend_to_backend = 0;
  int message_sent_from_backend_to_frontend = 0;
  zmq_msg_t message;
  while (1) {
    zmq_poll(items, 3, -1);
    if (items[0].revents & ZMQ_POLLIN) {
      // Address
      zmq_msg_recv_sendmore(&message, frontend, backend);
      // Delimiter
      zmq_msg_recv_sendmore(&message, frontend, backend);
      // Context
      zmq_msg_init(&message);
      zmq_msg_recv(&message, frontend, 0);
      void *span =
          start_span(tracer, "proxy-frontend-to-backend", SPAN_KIND_INTERNAL,
                     (char *)zmq_msg_data(&message));
      zmq_msg_send(&message, backend, ZMQ_SNDMORE);
      zmq_msg_close(&message);
      // Message
      zmq_msg_recv_send(&message, frontend, backend);
      end_span(span);
      message_sent_from_frontend_to_backend = 1;
    }
    if (items[1].revents & ZMQ_POLLIN) {
      // Address
      zmq_msg_recv_sendmore(&message, backend, frontend);
      // Delimiter
      zmq_msg_recv_sendmore(&message, backend, frontend);
      // Context
      zmq_msg_init(&message);
      zmq_msg_recv(&message, backend, 0);
      void *span =
          start_span(tracer, "proxy-backend-to-frontend", SPAN_KIND_INTERNAL,
                     (char *)zmq_msg_data(&message));
      zmq_msg_send(&message, frontend, ZMQ_SNDMORE);
      zmq_msg_close(&message);
      // Message
      zmq_msg_recv_send(&message, backend, frontend);
      end_span(span);
      message_sent_from_backend_to_frontend = 1;
    }
    if (items[2].revents & ZMQ_POLLIN) {
      free(s_recv(connector)); // Delimiter
      char *message = s_recv(connector);
      if (strcmp(message, "connect") == 0) {
        n_connected_clients++;
      } else if (strcmp(message, "disconnect") == 0) {
        n_connected_clients--;
      }
      free(message);
      if (n_connected_clients == 0 && message_sent_from_frontend_to_backend &&
          message_sent_from_backend_to_frontend) {
        break;
      }
    }
  }

  s_send(terminator, "KILL");

  zmq_close(connector);
  zmq_close(terminator);
  zmq_close(frontend);
  zmq_close(backend);
  zmq_ctx_destroy(context);
  return 0;
}
