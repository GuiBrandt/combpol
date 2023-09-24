FROM gcc:13.2 as builder
COPY . /usr/src/projecao/
WORKDIR /usr/src/projecao
RUN make build

FROM manjarolinux/base:20230924
RUN yes | sudo pacman -S make
COPY --from=builder /usr/src/projecao/build/release/combpol-projecao /combpol-projecao
WORKDIR /
CMD ["bash", "-c", "./combpol-projecao examples/*.in"]
