// Copyright 2025 Hyeongjun Jeon

#ifndef COUNTING_REQUEST_CLIENT__COUNTING_REQUEST_CLIENT_HPP_
#define COUNTING_REQUEST_CLIENT__COUNTING_REQUEST_CLIENT_HPP_

#include <random>

#include "grpcpp/grpcpp.h"

#include "counting_service.grpc.pb.h"
#include "counting_service.pb.h"


class CountingRequestClient {
 public:
  explicit CountingRequestClient(std::shared_ptr<grpc::Channel> channel);
  virtual ~CountingRequestClient();

  void send_random_counting_request();

 private:
  void change_prefix(std::string target_prefix);
  std::unique_ptr<counting_service::CountingService::Stub> stub_;
  int start_number_ = 0;
  int end_number_ = 0;

  std::string old_prefix_ = "Old number: ";
  std::string new_prefix_ = "New number: ";
};
#endif  // COUNTING_REQUEST_CLIENT__COUNTING_REQUEST_CLIENT_HPP_
