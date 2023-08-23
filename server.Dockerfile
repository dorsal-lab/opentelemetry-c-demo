FROM ghcr.io/dorsal-lab/lttng-otelcpp:main

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

CMD mkdir -p build &&\
    cmake -B build -S . -D CMAKE_BUILD_TYPE=Release &&\
    cmake --build build/ --target server -- &&\
	lttng create --output=ctf-traces/ &&\
	lttng enable-event -u 'opentelemetry:*' &&\
	lttng add-context -u -t vtid &&\
	lttng start &&\
	mkdir -p /tmp/output &&\
	touch /tmp/output/output.log &&\
    script -c "./build/server" -f /tmp/output/output.log &&\
	lttng stop &&\
	lttng destroy
