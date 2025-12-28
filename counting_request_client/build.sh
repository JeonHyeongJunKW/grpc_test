function build_counting_request_client() {
  rm -rf ~/counting_request_client
  cp -r ../counting_request_client ~/counting_request_client
  cp -r ../proto ~/counting_request_client/proto
  cd ~/counting_request_client
  mkdir build
  cd build
  cmake ..
  make -j4
}
