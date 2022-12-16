#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <zmq.h>

#include "constants.h"
#include "prime.h"
#include "zhelpers.h"

typedef struct {
  const char *socket_server_addr;
  void *context;
} worker_args_t;

long long current_ms_timestamp() {
  struct timeval te;
  gettimeofday(&te, NULL);
  return te.tv_sec * 1000LL + te.tv_usec / 1000;
}

static void *worker_routine(void *args) {
  //  Socket to talk to dispatcher
  worker_args_t *args_t = (worker_args_t *)args;
  void *responder = zmq_socket(args_t->context, ZMQ_REP);
  ZMQ_CHECK(zmq_connect(responder, args_t->socket_server_addr));

  while (1) {
    char nth_buffer[8];
    zmq_recv(responder, nth_buffer, 8, 0);
    printf("Received nth=%s\n", nth_buffer);
    int nth = atoi(nth_buffer);
    long nth_prime = get_nth_prime(nth);
    char nth_prime_buffer[15];
    sprintf(nth_prime_buffer, "%ld", nth_prime);
    zmq_send(responder, nth_prime_buffer, 15, 0);
    printf("Replied %sth prime = %s\n", nth_buffer, nth_prime_buffer);
  }

  zmq_close(responder);
  free(args);
  return NULL;
}

int main() {
  // ZMQ code inspired from
  // https://github.com/booksbyus/zguide/blob/master/examples/C/mtserver.c
  void *context = zmq_ctx_new();

  //  Socket to talk to clients
  void *clients = zmq_socket(context, ZMQ_ROUTER);
  char proxy_backend_addr[25];
  sprintf(proxy_backend_addr, "tcp://localhost:%s", PROXY_BACKEND_PORT);
  printf("Connecting to the proxy...\n");
  ZMQ_CHECK(zmq_connect(clients, proxy_backend_addr));

  //  Socket to talk to workers
  void *workers = zmq_socket(context, ZMQ_DEALER);
  char workers_socket_addr[] = "inproc://workers";

  ZMQ_CHECK(zmq_bind(workers, workers_socket_addr));

  //  Launch pool of worker threads
  int thread_nbr;
  for (thread_nbr = 0; thread_nbr < N_MAX_WORKERS; thread_nbr++) {
    pthread_t worker;
    worker_args_t *args = malloc(sizeof(worker_args_t));
    args->context = context;
    args->socket_server_addr = workers_socket_addr;
    pthread_create(&worker, NULL, worker_routine, args);
  }

  //  Connect work threads to client threads via a queue proxy
  printf("Connecting client to workers...\n");
  ZMQ_CHECK(zmq_proxy(clients, workers, NULL));

  //  We never get here, but clean up anyhow
  zmq_close(clients);
  zmq_close(workers);
  zmq_ctx_destroy(context);
  return 0;
}
