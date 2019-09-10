FROM alpine 
RUN apk add build-base gcc libc-dev gdb cairo-dev
RUN mkdir /usr/local/include
RUN cp -r /usr/include/cairo /usr/local/include/cairo
WORKDIR /gisp
COPY . .


