FROM gcc:latest
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
RUN curl https://sdk.cloud.google.com | bash
ENV PATH $PATH:/root/google-cloud-sdk/bin
ENV CMOCK_DIR /usr/src/cmock
ENV CJSON_DIR /usr/src/cJSON
COPY . /usr/src/phev-esp
WORKDIR /usr/src/phev-esp
RUN make test
RUN make test
ENV IDF_PATH /usr/src/esp-idf
ENV PATH $PATH:/usr/esp/xtensa-esp32-elf/bin
ENV PATH /usr/esp/xtensa-esp32-elf/bin:$IDF_PATH/tools:$PATH
RUN date +%s > /root/build_number
RUN BUILD=`cat /root/build_number` make -j8
#RUN make -j8
RUN cp /usr/src/phev-esp/build/phev-esp.bin /root/firmware-`cat /root/build_number`.bin
RUN gsutil cp /root/firmware-`cat /root/build_number`.bin gs://espimages/develop/ 