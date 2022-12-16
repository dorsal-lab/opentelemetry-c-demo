#ifndef Z_HELPERS_H
#define Z_HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <zmq.h>

#define ZMQ_CHECK(ZMQ_CALL)                                                    \
  ({                                                                           \
    if ((ZMQ_CALL) != 0) {                                                     \
      printf("ERROR %s:%d : %s\n", __FILE__, __LINE__, zmq_strerror(errno));   \
      exit(1);                                                                 \
    }                                                                          \
  })

#endif // !Z_HELPERS_H
