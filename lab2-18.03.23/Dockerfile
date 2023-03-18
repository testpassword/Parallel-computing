FROM debian:latest
RUN apt update && \
    apt install -y --allow-unauthenticated \
    g++ \
    gnuplot \
    nano \
    screenfetch \
    bc \
    curl
WORKDIR /lab2
COPY . .
RUN ./build.sh
ENV LD_LIBRARY_PATH=/lab2/src/FW_1.3.1_Lin64/lib