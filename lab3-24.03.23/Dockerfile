FROM debian:latest
RUN apt update && \
    apt install -y --allow-unauthenticated \
    g++ \
    gnuplot \
    nano \
    screenfetch \
    bc
WORKDIR /lab3
COPY . .
RUN ./build.sh
