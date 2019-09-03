FROM alpine AS builder
RUN apk add build-base gcc libc-dev
WORKDIR /gisp
COPY . .
RUN make clean && make 

FROM alpine
RUN apk add build-base gdb gcc libc-dev
COPY . .
ENTRYPOINT ["make", "debug"]

