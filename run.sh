#!/usr/bin/env bash
set -ex

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
cmake -B build
cmake --build build/ --config Debug --target all --

# Kill any application running on proxy ports
PROXY_PID=$(lsof -t -i:5559) && kill "$PROXY_PID"
PROXY_PID=$(lsof -t -i:5560) && kill "$PROXY_PID"

# Proxy
echo "Starting proxy ..."
./build/proxy &> /dev/null &
PROXY_PID=$!

# Servers
echo "Starting servers ..."
SERVERS_PID=()
for _ in $(seq 1 "$N_SERVERS"); do
    ./build/server &> /dev/null &
    SERVERS_PID+=( $! )
done

# Clients
echo "Starting clients ..."
CLIENTS_PID=()
for _ in $(seq 1 "$N_CLIENTS"); do
    ./build/client &
    CLIENTS_PID+=( $! )
done

# Wait for clients to finish
for pid in "${CLIENTS_PID[@]}"; do
    wait "$pid"
done
# Kill proxy
kill $PROXY_PID
# Kill servers
for pid in "${SERVERS_PID[@]}"; do
    kill "$pid"
done

echo "Done!"
