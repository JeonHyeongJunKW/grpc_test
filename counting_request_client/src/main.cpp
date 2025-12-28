#include <iostream>

#include "grpcpp/grpcpp.h"

#include "counting_request_client/counting_request_client.hpp"


int main() {
  // Your code logic here
  std::cout << "Counting request client started" << std::endl;
  std::shared_ptr<grpc::Channel> channel =
    grpc::CreateChannel("counting-response-server:9000", grpc::InsecureChannelCredentials());
  CountingRequestClient client(channel);
  client.send_random_counting_request();
  return 0;
}
