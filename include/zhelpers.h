#ifndef Z_HELPERS_H
#define Z_HELPERS_H

#include <zmq.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ZMQ_CHECK(ZMQ_CALL)                                                    \
  ({                                                                           \
    if ((ZMQ_CALL) != 0) {                                                     \
      printf("ERROR %s:%d : %s\n", __FILE__, __LINE__, zmq_strerror(errno));   \
      exit(1);                                                                 \
    }                                                                          \
  })

// // Inspiration
// // https://github.com/booksbyus/zguide/blob/master/examples/C/zhelpers.h

// Receive 0MQ string from socket and convert into C string
// Caller must free returned string.  Returns NULL if context is being
// terminated.
static char *s_recv(void *socket) {
  zmq_msg_t message;
  zmq_msg_init(&message);
  int size = zmq_msg_recv(&message, socket, 0);
  if (size == -1)
    return NULL;
  char *string = malloc(size + 1);
  memcpy(string, zmq_msg_data(&message), size);
  zmq_msg_close(&message);
  string[size] = 0;
  return string;
}

// Convert C string to 0MQ string and send to socket
static int s_send(void *socket, char *string) {
  zmq_msg_t message;
  zmq_msg_init_size(&message, strlen(string));
  memcpy(zmq_msg_data(&message), string, strlen(string));
  int size = zmq_msg_send(&message, socket, 0);
  zmq_msg_close(&message);
  return size;
}

// Sends string as 0MQ string, as multipart non-terminal
static int s_sendmore(void *socket, char *string) {
  zmq_msg_t message;
  zmq_msg_init_size(&message, strlen(string));
  memcpy(zmq_msg_data(&message), string, strlen(string));
  int size = zmq_msg_send(&message, socket, ZMQ_SNDMORE);
  zmq_msg_close(&message);
  return size;
}

static void zmq_msg_recv_send(zmq_msg_t *message, void *receiver,
                              void *sender) {
  zmq_msg_init(message);
  zmq_msg_recv(message, receiver, 0);
  zmq_msg_send(message, sender, 0);
  zmq_msg_close(message);
}

static void zmq_msg_recv_sendmore(zmq_msg_t *message, void *receiver,
                                  void *sender) {
  zmq_msg_init(message);
  zmq_msg_recv(message, receiver, 0);
  zmq_msg_send(message, sender, ZMQ_SNDMORE);
  zmq_msg_close(message);
}

static void zmq_exchange_multipart_message(zmq_msg_t *message, void *receiver,
                                           void *sender) {
  while (1) {
    zmq_msg_init(message);
    zmq_msg_recv(message, receiver, 0);
    int more = zmq_msg_more(message);
    zmq_msg_send(message, sender, more ? ZMQ_SNDMORE : 0);
    zmq_msg_close(message);
    if (!more) {
      break;
    }
  }
}

#endif // !Z_HELPERS_H
