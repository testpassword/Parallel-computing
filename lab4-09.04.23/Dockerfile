FROM debian:latest
RUN apt update && \
    apt install -y --allow-unauthenticated \
    g++ \
    nano \
    neofetch \
    bc
WORKDIR /lab4
COPY . .
RUN chmod +x build.sh perfomance_test.sh && ./build.sh
