# grpc_test

마이크로 서비스에서 gRPC 통신 구축 연습을 위한 테스트 레포지토리

## 📋 프로젝트 개요

이 레포지토리는 gRPC를 사용한 마이크로서비스 간 통신을 학습하고 테스트하기 위한 프로젝트입니다.
간단한 카운팅 서비스를 통해 gRPC의 서버 스트리밍 패턴을 구현하고 있습니다.

**주요 구성 요소:**
- **counting_response_server**: 카운팅 요청을 받아 숫자를 스트리밍으로 반환하는 gRPC 서버
- **counting_request_client**: 랜덤한 범위의 카운팅을 요청하는 gRPC 클라이언트
- **proto**: gRPC 서비스 정의를 포함하는 Protocol Buffers 파일

## 🏗️ 프로젝트 구조

```
grpc_test/
├── proto/                          # Protocol Buffers 정의 파일
│   └── counting_service.proto      # 카운팅 서비스 정의
├── counting_request_client/        # gRPC 클라이언트 구현 (C++)
│   ├── include/
│   │   └── counting_request_client/
│   │       └── counting_request_client.hpp
│   ├── src/
│   │   ├── counting_request_client.cpp  # 클라이언트 로직
│   │   └── main.cpp                     # 엔트리 포인트
│   ├── CMakeLists.txt
│   └── build.sh                    # 클라이언트 빌드 스크립트
├── counting_response_server/       # gRPC 서버 구현
│   └── build.sh                    # 서버 빌드 스크립트
├── scripts/                        # 설치 및 설정 스크립트
│   ├── install_grpc_cplusplus.sh   # C++ gRPC 설치 스크립트
│   └── install_grpc_python.sh      # Python gRPC 설치 스크립트
└── README.md
```

## 🔧 주요 기능

### CountingService

카운팅 서비스는 시작 숫자와 끝 숫자를 받아 그 사이의 숫자들을 스트리밍으로 반환하는 서비스입니다.

**서비스 정의:**
- **RPC Method**: `CountNumbers`
- **Request**: `CountingRequest` (start_number, end_number)
- **Response**: `CountingResponse` (stream of current_number)
- **패턴**: Server Streaming RPC

**클라이언트 동작 방식:**
1. 랜덤한 start_number와 end_number를 생성 (1~200 범위)
2. gRPC 채널을 통해 서버에 요청 전송
3. `ClientReader`를 사용하여 서버로부터 스트리밍 응답 수신
4. 각 숫자를 실시간으로 출력
5. 스트림 종료 후 상태 확인

**서버 동작 방식:**
- 클라이언트 요청을 받아 start_number부터 end_number까지 순차적으로 스트리밍
- 각 숫자를 `CountingResponse` 메시지로 전송

## 🚀 시작하기

### 사전 요구사항

- Linux 환경 (Ubuntu/Debian 권장)
- Python 3.x 또는 C++ 컴파일러
- Git

### gRPC 설치

#### Python용 gRPC 설치
```bash
bash scripts/install_grpc_python.sh
```

#### C++용 gRPC 설치
```bash
bash scripts/install_grpc_cplusplus.sh
```

C++ gRPC는 다음을 설치합니다:
- gRPC v1.76.0
- Protocol Buffers 컴파일러
- 필요한 빌드 도구 (build-essential, autoconf, libtool, pkg-config)

### 빌드 및 실행

#### 서버 빌드 및 실행
```bash
cd counting_response_server
bash build.sh
```

#### 클라이언트 빌드 및 실행
```bash
cd counting_request_client
bash build.sh
```

## 📝 Protocol Buffers 정의

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

## 🎯 학습 목표

1. **gRPC 기본 개념 이해**
   - Protocol Buffers 스키마 정의
   - gRPC 서비스 인터페이스 설계

2. **Server Streaming RPC 구현**
   - 서버에서 클라이언트로 연속적인 데이터 스트리밍
   - 스트림 처리 및 관리
   - `ClientReader`와 `ServerWriter` 사용법

3. **다중 언어 지원**
   - Python과 C++에서 동일한 proto 파일 사용
   - 언어 간 상호 운용성

4. **마이크로서비스 통신 패턴**
   - 서비스 간 네트워크 통신 설정
   - DNS 기반 서비스 디스커버리
   - 에러 핸들링 및 연결 관리

## ⚠️ 주의사항 및 트러블슈팅

### DNS Resolution 에러

클라이언트 코드에서 `grpc::CreateChannel("counting-response-server:9000", ...)` 를 사용할 때 다음과 같은 에러가 발생할 수 있습니다:

```
errors resolving counting-response-server:9000: [field:hostname lookup error:address lookup failed for counting-response-server:9000: Domain name not found]
```

**원인:**
- `counting-response-server` 호스트명이 DNS에 등록되지 않았거나 `/etc/hosts` 파일에 정의되지 않음
- Docker/Kubernetes 환경이 아닌 일반 로컬 환경에서는 해당 호스트명을 찾을 수 없음

**해결 방법:**

1. **localhost 사용 (로컬 테스트):**
   ```cpp
   std::shared_ptr<grpc::Channel> channel =
     grpc::CreateChannel("localhost:9000", grpc::InsecureChannelCredentials());
   ```

2. **IP 주소 직접 지정:**
   ```cpp
   std::shared_ptr<grpc::Channel> channel =
     grpc::CreateChannel("127.0.0.1:9000", grpc::InsecureChannelCredentials());
   ```

3. **/etc/hosts 파일 수정 (Docker/서비스 이름 사용 시):**
   ```bash
   sudo nano /etc/hosts
   # 다음 줄 추가
   127.0.0.1 counting-response-server
   ```

4. **Docker Compose 사용 (권장):**
   - Docker Compose에서는 서비스 이름이 자동으로 DNS에 등록됨
   - 네트워크 설정 없이도 서비스 이름으로 통신 가능

**학습 포인트:**
- gRPC 클라이언트는 채널 생성 시 DNS 또는 IP 주소로 서버 위치를 파악
- 마이크로서비스 환경에서는 서비스 디스커버리 메커니즘이 필요
- 로컬 개발 환경과 컨테이너 환경의 네트워킹 차이 이해 필요

### 스트리밍 응답 처리

```cpp
std::unique_ptr<grpc::ClientReader<counting_service::CountingResponse>> reader(
  stub_->CountNumbers(&context, request));
while (reader->Read(&response)) {
  std::cout << "Received number: " << response.current_number() << std::endl;
}
grpc::Status status = reader->Finish();
```

**중요 개념:**
- `ClientReader::Read()`: 블로킹 방식으로 다음 메시지를 기다림
- 서버가 메시지를 보낼 때마다 `Read()`가 true를 반환하며 루프 계속
- 서버가 스트림을 종료하면 `Read()`가 false를 반환하여 루프 종료
- `Finish()`로 최종 RPC 상태를 확인해야 함 (성공/실패 판단)

## 🔍 참고사항

- 설치 스크립트는 sudo 권한이 필요할 수 있습니다
- C++ gRPC 빌드는 시간이 오래 걸릴 수 있습니다 (약 10-30분)
- 빌드 시 `-j 4` 옵션으로 4개의 코어를 사용하여 병렬 컴파일합니다
- 클라이언트와 서버는 동일한 proto 파일로부터 생성된 코드를 사용해야 합니다
- InsecureChannelCredentials는 테스트용이며, 실제 프로덕션에서는 TLS 사용을 권장합니다
