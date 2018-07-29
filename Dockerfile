FROM gcc:latest
RUN \
  apt-get update && \
  apt-get install -y jq ccache ruby flex bison gperf python python-serial build-essential gcc clang git libssl-dev autoconf libtool cmake doxygen pkg-config unzip wget
WORKDIR /usr/esp
RUN wget -q https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
RUN tar -xzf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
RUN gem install bundler
WORKDIR /usr/src
RUN git clone https://github.com/openssl/openssl
RUN cd openssl && ./config && make && make install
RUN git clone https://github.com/benmcollins/libjwt.git
RUN git clone https://github.com/DaveGamble/cJSON.git
RUN git clone https://github.com/papawattu/jansson.git
RUN cd jansson && cmake . && make && make install
RUN cd cJSON && mkdir build && cd build && cmake .. && make && make install
RUN cd libjwt && autoreconf -i && ./configure && make && make install
WORKDIR /usr/src
RUN \ 
    git clone --recursive https://github.com/throwtheswitch/cmock.git && \
    git clone --recursive https://github.com/espressif/esp-idf.git 
WORKDIR /usr/src/esp-idf/components
RUN \
    git clone --recursive https://github.com/papawattu/espmqtt.git && \
    git clone --recursive https://github.com/papawattu/libjwt.git && \
    git clone --recursive https://github.com/papawattu/jansson.git
RUN cd jansson && cmake .
WORKDIR /usr/src/cmock
RUN bundle install
RUN curl https://sdk.cloud.google.com | bash
ENV PATH $PATH:/root/google-cloud-sdk/bin
ENV CMOCK_DIR /usr/src/cmock
ENV CJSON_DIR /usr/src/cJSON
ENV LD_LIBRARY_PATH /usr/local/lib
ENV LD_RUN_PATH /usr/local/lib
COPY . /usr/src/phev-esp
WORKDIR /usr/src/phev-esp
RUN make test
RUN make test
ENV IDF_PATH /usr/src/esp-idf
ENV PATH $PATH:/usr/esp/xtensa-esp32-elf/bin
ENV PATH /usr/esp/xtensa-esp32-elf/bin:$IDF_PATH/tools:$PATH
RUN date +%s > /root/build_number
RUN BUILD=`cat /root/build_number` make oldconfig && make
RUN cp /usr/src/phev-esp/build/phev-esp.bin /root/firmware-`cat /root/build_number`.bin
RUN gsutil cp /root/firmware-`cat /root/build_number`.bin gs://espimages/develop/ 
RUN gcloud iot devices configs get-value --device my-device2 --region us-central1 --registry my-registry | jq .latestBuild=`cat /root/build_number` > /root/config.json
RUN gcloud iot devices configs update --config-file =/root/config.json --device my-device2 --region us-central1 --registry my-registry