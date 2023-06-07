FROM ghcr.io/dorsal-lab/lttng-otelcpp:main

WORKDIR /tmp/dorsal-lab
RUN git clone https://github.com/dorsal-lab/opentelemetry-c.git
RUN cd opentelemetry-c &&\
	git checkout 1621239a982eaf3bc606938fff56f166fc066e95 &&\
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
