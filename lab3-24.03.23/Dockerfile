FROM debian:latest
RUN apt update && \
    apt install -y --allow-unauthenticated \
    g++ \
    nano \
    screenfetch \
    bc
WORKDIR /lab3
COPY . .
RUN chmod +x build.sh && ./build.sh
