# grpc_test

gRPC를 사용한 마이크로서비스 간 통신을 학습하기 위한 테스트 프로젝트입니다.

## 📋 프로젝트 개요

간단한 카운팅 서비스를 통해 gRPC의 서버 스트리밍 패턴을 구현합니다.

**구성 요소:**
- **counting_response_server** (Python): 카운팅 요청을 받아 숫자를 스트리밍으로 반환하는 gRPC 서버
- **counting_request_client** (C++): 랜덤한 범위의 카운팅을 요청하는 gRPC 클라이언트
- **proto**: gRPC 서비스 정의 (Protocol Buffers)

## 🏗️ 프로젝트 구조

```
grpc_test/
├── docker-compose.yml              # Docker Compose 설정
├── proto/
│   └── counting_service.proto      # gRPC 서비스 정의
├── counting_response_server/       # Python 서버
│   ├── main.py
│   ├── build.sh
│   └── Dockerfile.release
├── counting_request_client/        # C++ 클라이언트
│   ├── include/
│   ├── src/
│   ├── CMakeLists.txt
│   ├── build.sh
│   └── Dockerfile.release
└── scripts/                        # 설치 스크립트
    ├── install_grpc_python.sh
    └── install_grpc_cplusplus.sh
```

## 🚀 실행 방법

프로젝트 루트 경로에서 다음 명령어를 실행합니다:
```bash
docker compose up --build
```

## 📝 gRPC 서비스 정의

```proto
syntax = "proto3";

package counting_service;

message CountingRequest {
  int32 start_number = 1;
  int32 end_number = 2;
}

message CountingResponse {
  int32 numbers = 1;
}

service CountingService {
  rpc CountNumbers(CountingRequest) returns (stream CountingResponse);
}
```

**동작 방식:**
- **클라이언트**: 랜덤한 start_number와 end_number (1~200)를 생성하여 서버에 요청
- **서버**: start_number부터 end_number까지 숫자를 순차적으로 스트리밍 응답

## 🎯 학습 내용

### 1. gRPC Server Streaming
- 서버에서 클라이언트로 연속적인 데이터 스트리밍
- `ClientReader`와 `ServerWriter` 활용

### 2. 다중 언어 지원
- Python (서버)과 C++ (클라이언트)에서 동일한 proto 파일 사용
- 언어 간 상호 운용성

### 3. Docker 환경에서의 마이크로서비스 통신
- Docker Compose를 통한 서비스 간 네트워크 설정
- DNS 기반 서비스 디스커버리 (서비스 이름으로 통신)
- 채널 연결 관리 및 타임아웃 처리

## 💡 주요 개념

### 채널 연결 대기
```cpp
std::shared_ptr<grpc::Channel> channel =
  grpc::CreateChannel("counting-response-server:9000", grpc::InsecureChannelCredentials());

auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(100);
if (channel->WaitForConnected(deadline)) {
  // 연결 성공
} else {
  // 타임아웃
}
```

### 스트리밍 응답 처리
```cpp
std::unique_ptr<grpc::ClientReader<CountingResponse>> reader(
  stub_->CountNumbers(&context, request));

while (reader->Read(&response)) {
  // 각 메시지 처리
}
grpc::Status status = reader->Finish();
```
