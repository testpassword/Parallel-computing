FROM debian:latest
RUN apt update && \
    apt install -y --allow-unauthenticated \
    g++ \
    clang \
    gnuplot \
    nano \
    screenfetch \
    bc
WORKDIR /lab1
COPY . .
RUN chmod +x perfomance_test.sh
