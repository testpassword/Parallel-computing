FROM debian:latest
RUN apt update && \
    apt install -y --allow-unauthenticated \
    g++ \
    clang \
    gnuplot \
    nano \
    bc
WORKDIR /lab1
COPY . .
RUN chmod +x perfomance_test.sh 
# && ./perfomance_test.sh