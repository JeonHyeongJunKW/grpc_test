# grpc_test

마이크로 서비스에서 gRPC 통신 구축 연습을 위한 테스트 레포지토리

## 📋 프로젝트 개요

이 레포지토리는 gRPC를 사용한 마이크로서비스 간 통신을 학습하고 테스트하기 위한 프로젝트입니다.
간단한 카운팅 서비스를 통해 gRPC의 서버 스트리밍 패턴을 구현하고 있습니다.

## 🏗️ 프로젝트 구조

```
grpc_test/
├── proto/                          # Protocol Buffers 정의 파일
│   └── counting_service.proto      # 카운팅 서비스 정의
├── counting_request_client/        # gRPC 클라이언트 구현
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
- **Response**: `CountingResponse` (stream of numbers)
- **패턴**: Server Streaming RPC

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

3. **다중 언어 지원**
   - Python과 C++에서 동일한 proto 파일 사용
   - 언어 간 상호 운용성

## 🔍 참고사항

- 설치 스크립트는 sudo 권한이 필요할 수 있습니다
- C++ gRPC 빌드는 시간이 오래 걸릴 수 있습니다 (약 10-30분)
- 빌드 시 `-j 4` 옵션으로 4개의 코어를 사용하여 병렬 컴파일합니다
