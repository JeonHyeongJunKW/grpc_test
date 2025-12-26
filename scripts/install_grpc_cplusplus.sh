function install_grpc_cplusplus() {
  apt install -y build-essential autoconf libtool pkg-config
  mkdir -p ~/tools
  cd ~/tools/ || exit
  git clone --recurse-submodules -b v1.76.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc
  cd grpc
  mkdir -p cmake/build
  pushd cmake/build
  cmake -DgRPC_INSTALL=ON \
        -DgRPC_BUILD_TESTS=OFF \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        ../..
  make -j 4
  make install
  popd
  cd ~/
  rm -rf ~/tools/grpc
}
