function build_counting_request_client() {
  # Get the directory where this script is located
  SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

  # Build C++ project (CMake will generate protobuf files automatically)
  cd "$SCRIPT_DIR"
  rm -rf build
  mkdir -p build
  cd build
  cmake ..
  make -j4

  echo "Build complete! Executable: $SCRIPT_DIR/build/counting_request_client"
}
