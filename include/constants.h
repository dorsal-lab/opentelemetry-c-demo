#ifndef CONSTANTS_H
#define CONSTANTS_H

#define PROXY_FRONTEND_PORT "5559"
#define PROXY_BACKEND_PORT "5560"
#define PROXY_CONNECTION_MANAGEMENT_PORT "5561"
#define PROXY_TERMINATION_MANAGEMENT_PORT "5562"

// Maximal number of worker thread a single server can start
#define N_MAX_WORKERS 8

#define N_MAX_CLIENT_REQUESTS 50

#endif // !CONSTANTS_H
