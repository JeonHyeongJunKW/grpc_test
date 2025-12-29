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
  this->change_prefix(old_prefix_);
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
    const bool is_middle_number =
      (end_number_ - start_number_) % 2 == 0 ?
      (response.current_number() == (start_number_ + end_number_) / 2) :
      (response.current_number() == (start_number_ + end_number_ + 1) / 2);
    if (is_middle_number) {
      this->change_prefix(new_prefix_);
    }
  }
  grpc::Status status = reader->Finish();
  if (status.ok()) {
    std::cout << "Counting completed successfully." << std::endl;
  } else {
    std::cout << "Counting failed: " << status.error_message() << std::endl;
  }
}

void CountingRequestClient::change_prefix(std::string target_prefix)
{
  counting_service::PrefixRequest request;
  request.set_new_prefix(target_prefix);

  counting_service::PrefixResponse response;
  grpc::ClientContext context;

  grpc::Status status = stub_->ChangePrefix(&context, request, &response);
  if (status.ok()) {
    std::cout << "Prefix changed successfully from '" << response.old_prefix()
              << "' to '" << target_prefix << "'." << std::endl;
  } else {
    std::cout << "Failed to change prefix: " << status.error_message() << std::endl;
  }
}
