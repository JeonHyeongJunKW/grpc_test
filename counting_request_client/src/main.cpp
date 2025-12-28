// Copyright 2025 Hyeongjun Jeon

#include <iostream>

#include "grpcpp/grpcpp.h"

#include "counting_request_client/counting_request_client.hpp"


int main() {
  std::shared_ptr<grpc::Channel> channel =
    grpc::CreateChannel("counting-response-server:9000", grpc::InsecureChannelCredentials());

  std::cout << "Waiting for channel to connect in 100 seconds" << std::endl;
  auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(100);
  if (channel->WaitForConnected(deadline)) {
    std::cout << "Channel connected successfully!" << std::endl;
    CountingRequestClient client(channel);
    client.send_random_counting_request();
  } else {
    std::cerr << "Failed to connect to server within timeout period" << std::endl;
    return 0;
  }

  return 0;
}
