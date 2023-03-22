FROM ghcr.io/augustinsangam/lttng-otelcpp:main

WORKDIR /tmp/augustinsangam
RUN git clone --depth 1 https://github.com/augustinsangam/opentelemetry-c.git
RUN cd opentelemetry-c &&\
	git checkout 72381f92fc3e58070110bd287877f08985595f04 &&\
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
