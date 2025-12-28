function build_counting_response_server() {
  rm -rf ~/counting_response_server
  cp -r ../counting_response_server ~/counting_response_server
  cp -r ../proto/* ~/counting_response_server
  cd ~/counting_response_server
  python3 -m grpc_tools.protoc -I. --python_out=. --pyi_out=. --grpc_python_out=. ./counting_service.proto
}
