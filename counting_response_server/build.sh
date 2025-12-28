function build_counting_response_server() {
  # Get the directory where this script is located
  SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

  # Generate protobuf files in current directory
  cd "$SCRIPT_DIR"

  python3 -m grpc_tools.protoc -I../proto \
    --python_out=. \
    --pyi_out=. \
    --grpc_python_out=. \
    ../proto/counting_service.proto

  echo "Build complete! Generated files in $SCRIPT_DIR"
}
