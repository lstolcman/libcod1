FROM ubuntu:22.04 AS base

RUN mkdir -p /data
RUN touch /data/hello.txt

# Export the data to host using buildx
FROM scratch AS export
COPY --from=base /data/hello.txt .
