FROM gcc:4.9
RUN \
  apt-get update && \
  apt-get install -y ruby flex bison gperf python python-serial cmake
WORKDIR /usr/esp
RUN wget -q https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
RUN tar -xzf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
RUN gem install bundler
WORKDIR /usr/src
RUN \ 
    git clone --recursive https://github.com/throwtheswitch/cmock.git && \
    git clone --recursive https://github.com/espressif/esp-idf.git 
WORKDIR /usr/src/esp-idf/components
RUN \
    git clone --recursive https://github.com/DaveGamble/cJSON.git && \
    git clone --recursive https://github.com/papawattu/espmqtt.git && \
    git clone --recursive https://github.com/papawattu/libjwt.git && \
    git clone --recursive https://github.com/papawattu/jansson.git
WORKDIR /usr/src/esp-idf/components/jansson
RUN cmake .
WORKDIR /usr/src/cmock
RUN bundle install
COPY . /usr/src/phev-esp
WORKDIR /usr/src/phev-esp
ENV CMOCK_DIR /usr/src/cmock
ENV CJSON_DIR /usr/src/cJSON
ENV BUILD_NUMBER `date +%s`
RUN make test
RUN make test
ENV IDF_PATH /usr/src/esp-idf
ENV PATH $PATH:/usr/esp/xtensa-esp32-elf/bin
ENV PATH /usr/esp/xtensa-esp32-elf/bin:$IDF_PATH/tools:$PATH
RUN make -j8
WORKDIR /usr/
RUN curl https://sdk.cloud.google.com | bash
ENV PATH $PATH:/root/google-cloud-sdk/bin
RUN cp /usr/src/phev-esp/build/phev-esp.bin /root/firmware-${BUILD_NUMBER}.bin
RUN gsutil cp /root/firmware-${BUILD_NUMBER}.bin gs://espimages/develop/ 