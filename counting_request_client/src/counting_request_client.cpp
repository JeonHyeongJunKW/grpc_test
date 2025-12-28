// Copyright 2025 Hyeongjun Jeon

#include "counting_request_client/counting_request_client.hpp"


CountingRequestClient::CountingRequestClient(std::shared_ptr<grpc::Channel> channel)
{
  stub_ = counting_service::CountingService::NewStub(channel);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(1, 100);
  start_number_ = dis(gen);
  end_number_ = start_number_ + dis(gen);
}

CountingRequestClient::~CountingRequestClient()
{
  stub_.reset();
}

void CountingRequestClient::send_random_counting_request()
{
  counting_service::CountingRequest request;
  request.set_start_number(start_number_);
  request.set_end_number(end_number_);

  counting_service::CountingResponse response;
  grpc::ClientContext context;
  std::cout << "Send info " << start_number_ << " to " << end_number_ << std::endl;
  std::unique_ptr<grpc::ClientReader<counting_service::CountingResponse>> reader(
    stub_->CountNumbers(&context, request));
  while (reader->Read(&response)) {
    std::cout << "Received number: " << response.current_number() << std::endl;
  }
  grpc::Status status = reader->Finish();
  if (status.ok()) {
    std::cout << "Counting completed successfully." << std::endl;
  } else {
    std::cout << "Counting failed: " << status.error_message() << std::endl;
  }
}
