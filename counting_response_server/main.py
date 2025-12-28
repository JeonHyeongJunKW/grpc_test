# Copyright 2025 Hyeongjun Jeon
from concurrent import futures
import time

import grpc
import counting_service_pb2 as counting_service_pb2
import counting_service_pb2_grpc as counting_service_pb2_grpc


class CountingResponseServicer(counting_service_pb2_grpc.CountingServiceServicer):
    def __init__(self):
        pass

    def CountNumbers(self, request, context):
        print(f"Received counting request from {request.start_number} to {request.end_number}")
        start_num = request.start_number
        end_num = request.end_number
        for number in range(start_num, end_num + 1):
            time.sleep(1)  # Simulate delay
            response = counting_service_pb2.CountingResponse(current_number=number)
            yield response


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    counting_service_pb2_grpc.add_CountingServiceServicer_to_server(
        CountingResponseServicer(), server)

    server.add_insecure_port('[::]:9000')
    server.start()
    print("Counting Response Server started on port 9000.")
    server.wait_for_termination()


if __name__ == "__main__":
    serve()
