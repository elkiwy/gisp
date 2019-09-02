FROM alpine AS builder
RUN apk add build-base gcc libc-dev
WORKDIR /gisp
COPY . .
RUN make 

FROM alpine
RUN apk add build-base gdb
COPY Makefile .
COPY --from=builder /gisp/build /build
ENTRYPOINT ["make", "debug"]

