#include "constants.h"
#include "prime.h"
#include "zhelpers.h"

#include <opentelemetry_c.h>
#include <zmq.h>

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  pthread_t tid;
  const char *socket_server_addr;
  void *context;
} worker_args_t;

static void *worker_routine(void *args) {
  worker_args_t *args_t = (worker_args_t *)args;

  //  Socket to listen to proxy termination
  void *terminator = zmq_socket(args_t->context, ZMQ_SUB);
  zmq_setsockopt(terminator, ZMQ_SUBSCRIBE, "", 0);
  char proxy_termination_addr[25];
  sprintf(proxy_termination_addr, "tcp://localhost:%s",
          PROXY_TERMINATION_MANAGEMENT_PORT);
  printf("[server] Connecting to the terminator...\n");
  ZMQ_CHECK(zmq_connect(terminator, proxy_termination_addr));

  //  Socket to talk to dispatcher
  void *responder = zmq_socket(args_t->context, ZMQ_REP);
  ZMQ_CHECK(zmq_connect(responder, args_t->socket_server_addr));

  void *tracer = get_tracer();

  zmq_pollitem_t items[] = {{terminator, 0, ZMQ_POLLIN, 0},
                            {responder, 0, ZMQ_POLLIN, 0}};

  while (1) {
    zmq_msg_t message;
    zmq_poll(items, 2, -1);
    if (items[0].revents & ZMQ_POLLIN) {
      break;
    }
    if (items[1].revents & ZMQ_POLLIN) {
      char *span_ctx = s_recv(responder);
      void *span = start_span(tracer, "get-nth-prime-response",
                              SPAN_KIND_SERVER, span_ctx);

      char *nth_s = s_recv(responder);
      printf("[server] Received nth=%s\n", nth_s);
      int nth = atoi(nth_s);
      long nth_prime = get_nth_prime(nth);
      char nth_prime_buffer[15];
      sprintf(nth_prime_buffer, "%ld", nth_prime);
      s_sendmore(responder, span_ctx);
      s_send(responder, nth_prime_buffer);
      printf("[server] Replied %sth prime = %s\n", nth_s, nth_prime_buffer);

      free(nth_s);
      free(span_ctx);

      end_span(span);
    }
  }

  zmq_close(terminator);
  zmq_close(responder);
  return NULL;
}

int main() {
  init_tracer_provider("opentelemetry-c-demo-server", "0.0.1", "",
                       "machine-server-0.0.1");
  // ZMQ code inspired from
  // https://github.com/booksbyus/zguide/blob/master/examples/C/mtserver.c
  void *context = zmq_ctx_new();

  //  Socket to listen to proxy termination
  void *terminator = zmq_socket(context, ZMQ_SUB);
  zmq_setsockopt(terminator, ZMQ_SUBSCRIBE, "", 0);
  char proxy_termination_addr[25];
  sprintf(proxy_termination_addr, "tcp://localhost:%s",
          PROXY_TERMINATION_MANAGEMENT_PORT);
  printf("[server] Connecting to the terminator...\n");
  ZMQ_CHECK(zmq_connect(terminator, proxy_termination_addr));

  //  Socket to talk to clients
  void *clients = zmq_socket(context, ZMQ_ROUTER);
  char proxy_backend_addr[25];
  sprintf(proxy_backend_addr, "tcp://localhost:%s", PROXY_BACKEND_PORT);
  printf("[server] Connecting to the proxy...\n");
  ZMQ_CHECK(zmq_connect(clients, proxy_backend_addr));

  //  Socket to talk to workers
  void *workers = zmq_socket(context, ZMQ_DEALER);
  char workers_socket_addr[] = "inproc://workers";

  ZMQ_CHECK(zmq_bind(workers, workers_socket_addr));

  //  Launch pool of worker threads
  worker_args_t threads_args[N_MAX_WORKERS];
  for (int i = 0; i < N_MAX_WORKERS; i++) {
    threads_args[i].context = context;
    threads_args[i].socket_server_addr = workers_socket_addr;
    pthread_create(&(threads_args[i].tid), NULL, worker_routine,
                   &threads_args[i]);
  }

  //  Initialize poll set
  zmq_pollitem_t items[] = {{terminator, 0, ZMQ_POLLIN, 0},
                            {clients, 0, ZMQ_POLLIN, 0},
                            {workers, 0, ZMQ_POLLIN, 0}};

  // Connect work threads to client threads via a queue proxy
  printf("[server] Connecting client to workers...\n");

  zmq_msg_t message;
  while (1) {
    zmq_poll(items, 3, -1);
    if (items[0].revents & ZMQ_POLLIN) {
      printf("[server] Received kill signal\n");
      break;
    }
    if (items[1].revents & ZMQ_POLLIN) {
      zmq_exchange_one_multipart_message(&message, clients, workers);
    }
    if (items[2].revents & ZMQ_POLLIN) {
      zmq_exchange_one_multipart_message(&message, workers, clients);
    }
  }

  // Waiting for threads to gracefully stop
  for (int i = 0; i < N_MAX_WORKERS; i++) {
    pthread_join(threads_args[i].tid, NULL);
  }

  zmq_close(terminator);
  zmq_close(clients);
  zmq_close(workers);
  zmq_ctx_destroy(context);
  return 0;
}
