FROM gcc:latest
WORKDIR /home

RUN apt-get update && \
	apt-get install --yes cmake && \
	git clone https://gitlab.com/cmocka/cmocka.git && \
	cd cmocka && \
	mkdir build && \
	cd build && \	
    cmake /home/cmocka && \
	make install

ENV CMOCKA_HOME="/home/cmocka"
ENV LD_LIBRARY_PATH="/usr/local/lib"

WORKDIR /home/src