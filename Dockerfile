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

CMD time ./run.sh 6 3
