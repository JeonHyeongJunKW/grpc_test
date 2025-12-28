FROM ubuntu:22.04

ARG USERNAME=vscode
RUN useradd -ms /bin/bash $USERNAME

RUN sed -i 's|http://archive.ubuntu.com|http://mirror.kakao.com|g' /etc/apt/sources.list && \
  sed -i 's|http://security.ubuntu.com|http://mirror.kakao.com|g' /etc/apt/sources.list

RUN apt-get clean && \
  rm -rf /var/lib/apt/lists/* && \
  apt-get update && \
  apt-get install -y --no-install-recommends \
  build-essential \
  cmake \
  curl \
  gdb \
  git \
  htop \
  iftop \
  iperf \
  python3 \
  python3-pip \
  wget \
  xclip \
  zip \
  unzip \
  vim \
  net-tools && \
  rm -rf /var/lib/apt/lists/*

COPY ./scripts/install_grpc_python.sh /usr/local/bin/scripts/install_grpc_python.sh
RUN bash -c "source /usr/local/bin/scripts/install_grpc_python.sh && install_grpc_python && rm -rf /usr/local/bin/scripts/install_grpc_python.sh"
RUN apt install -y protobuf-compiler protobuf-compiler-grpc

USER $USERNAME
WORKDIR /home/$USERNAME
