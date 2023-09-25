FROM gcc:8 as builder
COPY ./src /usr/projecao/src
COPY ./Makefile /usr/projecao/
WORKDIR /usr/projecao
RUN make build

FROM ubuntu:focal
COPY --from=builder /usr/projecao/build/release/combpol-projecao /combpol-projecao
WORKDIR /
CMD ["bash", "-c", "./combpol-projecao examples/*.in"]
