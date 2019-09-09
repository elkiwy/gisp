FROM alpine 
RUN apk add build-base gcc libc-dev gdb
WORKDIR /gisp
COPY . .


