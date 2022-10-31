# otel-experiment

TODO 

## Run

```bash
docker build --no-cache -t otel-experiment . &&  docker run otel-experiment:latest
```

Example of program execution output :

```plaintext
$ docker build -t otel-experiment . &&  docker run otel-experiment:latest
Sending build context to Docker daemon  70.66kB
Step 1/8 : FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-ubuntu-22.04
 ---> f66cbe0da8ed
Step 2/8 : ARG DEBIAN_FRONTEND=noninteractive
 ---> Using cache
 ---> 474031c58a79
Step 3/8 : RUN apt-get update &&     apt-get install -y software-properties-common &&     apt-add-repository ppa:lttng/stable-2.13 &&     apt-get update
 ---> Using cache
 ---> 828c0c047be3
Step 4/8 : RUN apt-get install -y wget     curl     zip     unzip     tar     curl      time     libcurl4-openssl-dev     build-essential     libczmq-dev     linux-headers-$(uname -r)     pkg-config     kmod     libnuma-dev     uuid-dev     libpopt-dev     liburcu-dev     libxml2-dev     babeltrace2     numactl     binutils     libc-dev     libstdc++-10-dev     gcc-10     g++-10     clangd-12     clang-tidy     clang-format     cmake     git     make     ninja-build
 ---> Using cache
 ---> ceba3ea4a68b
Step 5/8 : ENV CC=gcc-10 CXX=g++-10
 ---> Using cache
 ---> 6a3f74042cfe
Step 6/8 : WORKDIR /code
 ---> Using cache
 ---> 2e7865ff3a51
Step 7/8 : COPY . .
 ---> da945d962380
Step 8/8 : CMD time ./run.sh 6 3
 ---> Running in f2f5e097dc81
Removing intermediate container f2f5e097dc81
 ---> 0dbeaa6adb65
Successfully built 0dbeaa6adb65
Successfully tagged otel-experiment:latest
+ N_CLIENTS=1
+ N_SERVERS=1
+ [[ -n 6 ]]
+ N_CLIENTS=6
+ [[ -n 3 ]]
+ N_SERVERS=3
+ echo 'Building all targets ...'
+ mkdir -p build
Building all targets ...
+ cd build
+ cmake ..
-- The C compiler identification is GNU 10.4.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/gcc-10 - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /code/build
+ make
[ 14%] Building C object CMakeFiles/client.dir/src/client.c.o
[ 28%] Linking C executable client
[ 28%] Built target client
[ 42%] Building C object CMakeFiles/proxy.dir/src/proxy.c.o
[ 57%] Linking C executable proxy
[ 57%] Built target proxy
[ 71%] Building C object CMakeFiles/server.dir/src/server.c.o
[ 85%] Building C object CMakeFiles/server.dir/src/prime.c.o
[100%] Linking C executable server
[100%] Built target server
++ lsof -t -i:5559
+ PROXY_PID=
++ lsof -t -i:5560
Starting proxy ...
+ PROXY_PID=
+ echo 'Starting proxy ...'
+ PROXY_PID=97
+ echo 'Starting servers ...'
+ SERVERS_PID=()
Starting servers ...
+ ./proxy
++ seq 1 3
+ for _ in $(seq 1 "$N_SERVERS")
+ SERVERS_PID+=($!)
+ for _ in $(seq 1 "$N_SERVERS")
+ ./server
+ SERVERS_PID+=($!)
+ for _ in $(seq 1 "$N_SERVERS")
+ ./server
+ SERVERS_PID+=($!)
+ echo 'Starting clients ...'
+ CLIENTS_PID=()
Starting clients ...
+ ./server
++ seq 1 6
+ for _ in $(seq 1 "$N_CLIENTS")
+ CLIENTS_PID+=($!)
+ for _ in $(seq 1 "$N_CLIENTS")
+ ./client
+ CLIENTS_PID+=($!)
+ for _ in $(seq 1 "$N_CLIENTS")
+ ./client
+ CLIENTS_PID+=($!)
+ for _ in $(seq 1 "$N_CLIENTS")
+ CLIENTS_PID+=($!)
+ for _ in $(seq 1 "$N_CLIENTS")
+ ./client
+ CLIENTS_PID+=($!)
+ for _ in $(seq 1 "$N_CLIENTS")
+ CLIENTS_PID+=($!)
+ ./client
+ for pid in "${CLIENTS_PID[@]}"
+ wait 103
+ ./client
+ ./client
Connecting to the proxy
Requesting nth_prime_buffer=2777
Received 2777th prime = 25163
Requesting nth_prime_buffer=2915
Received 2915th prime = 26561
Requesting nth_prime_buffer=3793
Received 3793th prime = 35617
Requesting nth_prime_buffer=2335
Received 2335th prime = 20743
Requesting nth_prime_buffer=3386
Received 3386th prime = 31469
Requesting nth_prime_buffer=2492
Received 2492th prime = 22259
Requesting nth_prime_buffer=2649
Received 2649th prime = 23819
Requesting nth_prime_buffer=3421
Received 3421th prime = 31847
Requesting nth_prime_buffer=2362
Received 2362th prime = 21011
Requesting nth_prime_buffer=2027
Received 2027th prime = 17627
Connecting to the proxy
Requesting nth_prime_buffer=2777
Received 2777th prime = 25163
Requesting nth_prime_buffer=2915
Received 2915th prime = 26561
Requesting nth_prime_buffer=3793
Received 3793th prime = 35617
Requesting nth_prime_buffer=2335
Received 2335th prime = 20743
Requesting nth_prime_buffer=3386
Received 3386th prime = 31469
Requesting nth_prime_buffer=2492
Received 2492th prime = 22259
Requesting nth_prime_buffer=2649
Received 2649th prime = 23819
Requesting nth_prime_buffer=3421
Received 3421th prime = 31847
Requesting nth_prime_buffer=2362
Received 2362th prime = 21011
Requesting nth_prime_buffer=2027
Received 2027th prime = 17627
Connecting to the proxy
Requesting nth_prime_buffer=2777
Received 2777th prime = 25163
Requesting nth_prime_buffer=2915
Received 2915th prime = 26561
Requesting nth_prime_buffer=3793
Received 3793th prime = 35617
Requesting nth_prime_buffer=2335
Received 2335th prime = 20743
Requesting nth_prime_buffer=3386
Received 3386th prime = 31469
Requesting nth_prime_buffer=2492
Received 2492th prime = 22259
Requesting nth_prime_buffer=2649
Received 2649th prime = 23819
Requesting nth_prime_buffer=3421
Received 3421th prime = 31847
Requesting nth_prime_buffer=2362
Received 2362th prime = 21011
Requesting nth_prime_buffer=2027
Received 2027th prime = 17627
+ for pid in "${CLIENTS_PID[@]}"
+ wait 104
+ for pid in "${CLIENTS_PID[@]}"
+ wait 105
Connecting to the proxy
Requesting nth_prime_buffer=2777
Received 2777th prime = 25163
Requesting nth_prime_buffer=2915
Received 2915th prime = 26561
Requesting nth_prime_buffer=3793
Received 3793th prime = 35617
Requesting nth_prime_buffer=2335
Received 2335th prime = 20743
Requesting nth_prime_buffer=3386
Received 3386th prime = 31469
Requesting nth_prime_buffer=2492
Received 2492th prime = 22259
Requesting nth_prime_buffer=2649
Received 2649th prime = 23819
Requesting nth_prime_buffer=3421
Received 3421th prime = 31847
Requesting nth_prime_buffer=2362
Received 2362th prime = 21011
Requesting nth_prime_buffer=2027
Received 2027th prime = 17627
+ for pid in "${CLIENTS_PID[@]}"
+ wait 108
Connecting to the proxy
Requesting nth_prime_buffer=2777
Received 2777th prime = 25163
Requesting nth_prime_buffer=2915
Received 2915th prime = 26561
Requesting nth_prime_buffer=3793
Received 3793th prime = 35617
Requesting nth_prime_buffer=2335
Received 2335th prime = 20743
Requesting nth_prime_buffer=3386
Received 3386th prime = 31469
Requesting nth_prime_buffer=2492
Received 2492th prime = 22259
Requesting nth_prime_buffer=2649
Received 2649th prime = 23819
Requesting nth_prime_buffer=3421
Received 3421th prime = 31847
Requesting nth_prime_buffer=2362
Received 2362th prime = 21011
Requesting nth_prime_buffer=2027
Received 2027th prime = 17627
Connecting to the proxy
Requesting nth_prime_buffer=2777
Received 2777th prime = 25163
Requesting nth_prime_buffer=2915
Received 2915th prime = 26561
Requesting nth_prime_buffer=3793
Received 3793th prime = 35617
Requesting nth_prime_buffer=2335
Received 2335th prime = 20743
Requesting nth_prime_buffer=3386
Received 3386th prime = 31469
Requesting nth_prime_buffer=2492
Received 2492th prime = 22259
Requesting nth_prime_buffer=2649
Received 2649th prime = 23819
Requesting nth_prime_buffer=3421
Received 3421th prime = 31847
Requesting nth_prime_buffer=2362
Received 2362th prime = 21011
Requesting nth_prime_buffer=2027
Received 2027th prime = 17627
+ for pid in "${CLIENTS_PID[@]}"
+ wait 109
+ for pid in "${CLIENTS_PID[@]}"
+ wait 110
+ kill 97
+ for pid in "${SERVERS_PID[@]}"
+ kill 99
+ for pid in "${SERVERS_PID[@]}"
+ kill 100
+ for pid in "${SERVERS_PID[@]}"
+ kill 101
+ echo 'Done!'
Done!
1.01user 0.62system 0:02.09elapsed 78%CPU (0avgtext+0avgdata 19576maxresident)k
0inputs+1208outputs (0major+52329minor)pagefaults 0swaps
```
