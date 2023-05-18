FROM python:latest
RUN apt update && \
    apt install -y --allow-unauthenticated \
    g++ \
    nano \
    neofetch \
    bc && \
    pip3 install numpy scipy
WORKDIR /lab5
COPY . .
RUN chmod +x build.sh perfomance_test.sh && ./build.sh 
