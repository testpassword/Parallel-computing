FROM python:latest
RUN apt update && \
    apt install -y --allow-unauthenticated \
    g++ \
    nano \
    neofetch \
    bc && \
    pip3 install numpy scipy
WORKDIR /lab4
COPY . .
RUN chmod +x extra_task/build.sh && ./extra_task/build.sh
RUN chmod +x build.sh perfomance_test.sh && ./build.sh 
