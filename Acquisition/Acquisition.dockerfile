FROM ubuntu:20.04

ENV TZ=America/Brasilia
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone


RUN apt-get update \
&& apt-get upgrade -y \
&& apt-get install -y wget nano tzdata

WORKDIR /usr/muondecay

COPY . .

RUN ./scripts/build/install/python.sh
RUN ./scripts/build/install/root.sh
RUN ./scripts/build/install/ni-visa.sh

ENTRYPOINT [ "/bin/bash" ]
