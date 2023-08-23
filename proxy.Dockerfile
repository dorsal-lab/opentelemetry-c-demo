FROM ghcr.io/dorsal-lab/lttng-otelcpp:main as builder

WORKDIR /

RUN apt-get update && apt-get install -y make gcc git

# Get the sources and checkout at stable release 0.98
# see https://github.com/wolfcw/libfaketime/releases
RUN git clone https://github.com/wolfcw/libfaketime.git && \
    cd libfaketime && \
    git checkout dc2ae5eef31c7a64ce3a976487d8e57d50b8d594 && \
    make

FROM ghcr.io/dorsal-lab/lttng-otelcpp:main as final

# Add a +3s time drift
COPY --from=builder /libfaketime/src/libfaketime.so.1 /usr/local/lib
ENV LD_PRELOAD=/usr/local/lib/libfaketime.so.1
ENV FAKETIME="+3s"

WORKDIR /tmp/dorsal-lab
RUN git clone https://github.com/dorsal-lab/opentelemetry-c.git &&\
    cd opentelemetry-c &&\
	git checkout 9a03a58e2b37f0d2c11ad5352130295f1631980e &&\
	mkdir -p build &&\
	cd build &&\
	cmake -DBATCH_SPAN_PROCESSOR_ENABLED=ON \
		-DLTTNG_EXPORTER_ENABLED=ON \
		-DBUILD_EXAMPLES=OFF \
		-DBUILD_SHARED_LIBS=ON \
		-DCMAKE_INSTALL_PREFIX=/usr/local/ \
		.. &&\
	make -j $(nproc) &&\
	make install

RUN ldconfig

WORKDIR /code
COPY . .

EXPOSE 5559
EXPOSE 5560

CMD mkdir -p build &&\
    cmake -B build -S . -D CMAKE_BUILD_TYPE=Release &&\
    cmake --build build/ --target proxy -- &&\
	lttng create --output=ctf-traces/ &&\
	lttng enable-event -u 'opentelemetry:*' &&\
	lttng add-context -u -t vtid &&\
	lttng start &&\
	mkdir -p /tmp/output &&\
	touch /tmp/output/output.log &&\
    script -c "./build/proxy" -f /tmp/output/output.log &&\
	lttng stop &&\
	lttng destroy
