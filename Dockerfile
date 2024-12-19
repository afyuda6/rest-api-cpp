FROM ubuntu:20.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    g++ \
    sqlite3 \
    libsqlite3-dev \
    cmake

WORKDIR /rest-api-cpp

COPY . /rest-api-cpp

RUN cmake . && make

EXPOSE 8080

CMD ["./rest-api-cpp"]