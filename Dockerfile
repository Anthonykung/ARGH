# Jetson JetPack SDK supporst only Ubuntu 18.04

FROM ubuntu:18.04
COPY . /app
RUN make /app
CMD python /app/app.py