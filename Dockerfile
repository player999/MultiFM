FROM ubuntu:latest
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update
RUN apt-get install -y cmake
RUN apt-get install -y qt5-default
RUN apt-get install -y libavcodec-dev
RUN apt-get install -y librtlsdr-dev
RUN apt-get install -y libhackrf-dev
RUN apt-get install -y doxygen
RUN apt-get install -y texlive
RUN apt-get install -y graphviz
RUN apt-get install -y libgtest-dev
RUN apt-get install -y libboost-all-dev
RUN apt-get install -y git
RUN apt-get install -y g++
RUN apt-get install -y texlive-latex-extra

