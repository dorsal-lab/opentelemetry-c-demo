#!/usr/bin/env bash

# Run the whole client, proxy and server stack
# Usage: ./run.sh <n_clients> <n_server>
# Note that each server is multithreaded and launchs N_MAX_WORKERS worker threads

set -e

N_CLIENTS=1
N_SERVERS=1
if [[ -n "$1" ]]
then
    N_CLIENTS=$1
fi
if [[ -n "$2" ]]
then
    N_SERVERS=$2
fi

echo "Building all targets ..."

mkdir -p build
cmake -B build -S . -D CMAKE_BUILD_TYPE=Release
cmake --build build/ --target all --

echo "Starting a LTTng session ..."
lttng create --output=ctf-traces/
lttng enable-event -u 'opentelemetry:*'
lttng add-context -u -t vtid
lttng start

# Start the proxy
if lsof -Pi :5559 -sTCP:LISTEN -t >/dev/null ; then
    echo "ERROR: Port 5559 already in use"
    exit
fi
if lsof -Pi :5560 -sTCP:LISTEN -t >/dev/null ; then
    echo "ERROR: Port 5560 already in use"
    exit
fi
# Kill any application running on proxy ports
# PROXY_PID=$(lsof -t -i:5559) && (echo $PROXY_PID | xargs -n1 kill)
# PROXY_PID=$(lsof -t -i:5560) && (echo $PROXY_PID | xargs -n1 kill)
echo "Starting proxy ..."
./build/proxy &
PROXY_PID=$!

# Servers
echo "Starting servers ..."
SERVERS_PID=()
for _ in $(seq 1 "$N_SERVERS"); do
    ./build/server &
    SERVERS_PID+=( $! )
done

# Clients
echo "Starting clients ..."
CLIENTS_PID=()
for _ in $(seq 1 "$N_CLIENTS"); do
    ./build/client &
    CLIENTS_PID+=( $! )
done

# Termination handling
# stty -echoctl # hide ^C
# stop_everything() {
#     for pid in "${CLIENTS_PID[@]}"; do
#         kill "$pid" || true
#     done
#     kill $PROXY_PID || true
#     for pid in "${SERVERS_PID[@]}"; do
#         kill "$pid" || true
#     done
# }
# trap 'stop_everything' SIGINT

# Wait for clients to finish
for pid in "${CLIENTS_PID[@]}"; do
    wait "$pid"
done
# Wait for proxy
wait $PROXY_PID
# Wait for servers
for pid in "${SERVERS_PID[@]}"; do
    wait "$pid"
done

echo "Stop LTTng session ..."
lttng stop

echo "View traces ..."
lttng view | sed 's/\(.\{400\}\).*/\1.../'

echo "Destroting LTTng session ..."
lttng destroy

echo "Done!"
