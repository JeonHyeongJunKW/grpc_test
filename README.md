# grpc_test

gRPC를 사용한 마이크로서비스 간 통신을 학습하기 위한 테스트 프로젝트입니다.

## 📋 프로젝트 개요

gRPC의 **서버 스트리밍(Server Streaming)** 패턴과 **단방향 RPC(Unary RPC)**를 구현한 학습 프로젝트입니다.

**구성 요소:**
- **counting_response_server** (Python): 카운팅 요청을 받아 숫자를 스트리밍으로 반환하고, prefix 변경을 처리하는 gRPC 서버
- **counting_request_client** (C++): 랜덤한 범위의 카운팅을 요청하고, 중간 지점에서 prefix를 동적으로 변경하는 gRPC 클라이언트
- **proto**: gRPC 서비스 정의 (Protocol Buffers)

**동작 흐름:**
1. C++ 클라이언트가 랜덤한 start_number와 end_number를 생성 (1~200 사이)
2. 초기 prefix("Old number: ")를 서버에 설정
3. Python 서버에 CountingRequest를 전송하여 스트리밍 시작
4. 서버가 start_number부터 end_number까지 1초 간격으로 숫자를 스트리밍
5. 클라이언트가 중간 지점 도달 시 ChangePrefix RPC를 호출하여 prefix를 "New number: "로 변경
6. 변경된 prefix로 나머지 숫자들이 출력됨

**주요 기능:**
- 🔄 **Server Streaming**: 단일 요청에 대해 여러 응답을 순차적으로 스트리밍
- 🔧 **Unary RPC**: 실시간으로 서버 상태(prefix) 변경
- 🔀 **동시성**: ThreadPoolExecutor를 통한 병렬 요청 처리 (최대 10개)
- 🎲 **동적 로직**: 스트리밍 중간에 서버 상태를 변경하는 패턴

## 🏗️ 프로젝트 구조

```
grpc_test/
├── docker-compose.yml                  # Docker Compose 설정
├── .gitignore                          # Git 제외 파일 설정
├── proto/
│   └── counting_service.proto          # gRPC 서비스 정의
├── counting_response_server/           # Python 서버
│   ├── main.py                         # 서버 메인 코드
│   ├── build.sh                        # 빌드 스크립트
│   ├── Dockerfile.release              # 프로덕션 Dockerfile
│   ├── *_pb2.py                        # 생성된 protobuf 파일 (gitignore)
│   └── *_pb2_grpc.py                   # 생성된 gRPC 파일 (gitignore)
├── counting_request_client/            # C++ 클라이언트
│   ├── include/                        # 헤더 파일
│   ├── src/                            # 소스 파일
│   ├── CMakeLists.txt                  # CMake 빌드 설정
│   ├── build.sh                        # 빌드 스크립트
│   ├── build/                          # 빌드 결과물 (gitignore)
│   │   ├── counting_request_client     # 실행 파일
│   │   └── generated_proto/            # 생성된 protobuf 파일
│   └── Dockerfile.release              # 프로덕션 Dockerfile
├── .devcontainer/                      # VS Code Dev Container 설정
│   ├── counting_request_client/
│   └── counting_response_server/
└── scripts/                            # gRPC 설치 스크립트
    ├── install_grpc_python.sh
    └── install_grpc_cplusplus.sh
```

**빌드 디렉토리 구조 (Out-of-source Build):**
- ✅ 소스 코드와 빌드 결과물 완전 분리
- ✅ `build/` 디렉토리는 `.gitignore`에 포함되어 Git 추적 제외
- ✅ 생성된 protobuf 파일들도 자동으로 무시

## 🚀 실행 방법

### Docker Compose 사용 (권장)

프로젝트 루트 경로에서 다음 명령어를 실행합니다:

```bash
# Foreground 실행
docker compose up --build

# 또는 Background 실행
docker compose up -d --build
docker compose logs -f
```

### 실행 결과 예시

```
counting-response-server-1  | Counting Response Server started on port 9000.
counting-request-client-1   | Waiting for channel to connect in 100 seconds
counting-request-client-1   | Channel connected successfully!
counting-request-client-1   | Prefix changed successfully from 'Number: ' to 'Old number: '.
counting-request-client-1   | Send info 42 to 137
counting-response-server-1  | Received counting request from 42 to 137
counting-response-server-1  | Old number:
counting-request-client-1   | Received number: 42
counting-request-client-1   | Received number: 43
...
counting-request-client-1   | Received number: 89  (← 중간 지점!)
counting-request-client-1   | Prefix changed successfully from 'Old number: ' to 'New number: '.
counting-response-server-1  | New number:
counting-request-client-1   | Received number: 90
...
counting-request-client-1   | Received number: 137
counting-request-client-1   | Counting completed successfully.
```

## 📝 gRPC 서비스 정의

`proto/counting_service.proto` 파일에 정의된 서비스:

```proto
syntax = "proto3";

package counting_service;

message CountingRequest {
  int32 start_number = 1;
  int32 end_number = 2;
}

message CountingResponse {
  int32 current_number = 1;
}

message PrefixRequest {
  string new_prefix = 1;
}

message PrefixResponse {
  string old_prefix = 1;
}

service CountingService {
  rpc CountNumbers(CountingRequest) returns (stream CountingResponse);
  rpc ChangePrefix(PrefixRequest) returns (PrefixResponse);
}
```

**RPC 메서드:**
- `CountNumbers`: 단일 요청을 받아 여러 응답을 스트리밍으로 반환 (Server Streaming)
- `ChangePrefix`: 서버의 출력 prefix를 변경 (Unary RPC)

## 🎯 주요 구현 내용

### 1. Python 서버 (counting_response_server)

**핵심 코드:**
```python
class CountingResponseServicer(counting_service_pb2_grpc.CountingServiceServicer):
    def __init__(self):
        self.prefix = "Number: "

    def CountNumbers(self, request, context):
        print(f"Received counting request from {request.start_number} to {request.end_number}")
        start_num = request.start_number
        end_num = request.end_number
        for number in range(start_num, end_num + 1):
            time.sleep(1)  # 1초 지연
            print(self.prefix)  # 현재 prefix 출력
            response = counting_service_pb2.CountingResponse(current_number=number)
            yield response  # 스트리밍 응답

    def ChangePrefix(self, request, context):
        old_prefix = self.prefix
        self.prefix = request.new_prefix
        return counting_service_pb2.PrefixResponse(old_prefix=old_prefix)

def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    counting_service_pb2_grpc.add_CountingServiceServicer_to_server(
        CountingResponseServicer(), server)
    server.add_insecure_port('[::]:9000')  # IPv4/IPv6 모두 수신
    server.start()
    server.wait_for_termination()
```

**특징:**
- `yield`를 사용한 제너레이터로 스트리밍 구현
- `[::]:9000` 주소로 IPv4와 IPv6 모두 지원
- ThreadPoolExecutor로 동시 요청 처리 (최대 10개)
- **공유 상태**: `self.prefix`를 통해 모든 클라이언트가 상태 공유 (Race condition 주의)

### 2. C++ 클라이언트 (counting_request_client)

**채널 연결 및 타임아웃 처리:**
```cpp
std::shared_ptr<grpc::Channel> channel =
  grpc::CreateChannel("counting-response-server:9000", grpc::InsecureChannelCredentials());

auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(100);
if (channel->WaitForConnected(deadline)) {
  // 연결 성공
  CountingRequestClient client(channel);
  client.send_random_counting_request();
} else {
  // 타임아웃
  std::cerr << "Failed to connect to server within timeout period" << std::endl;
}
```

**랜덤 범위 생성:**
```cpp
CountingRequestClient::CountingRequestClient(std::shared_ptr<grpc::Channel> channel)
{
  stub_ = counting_service::CountingService::NewStub(channel);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(1, 100);
  start_number_ = dis(gen);           // 1~100 사이 랜덤
  end_number_ = start_number_ + dis(gen);  // start + (1~100)
}
```

**스트리밍 응답 수신 및 동적 prefix 변경:**
```cpp
void CountingRequestClient::send_random_counting_request()
{
  // 초기 prefix 설정
  this->change_prefix("Old number: ");

  counting_service::CountingRequest request;
  request.set_start_number(start_number_);
  request.set_end_number(end_number_);

  grpc::ClientContext context;
  std::unique_ptr<grpc::ClientReader<counting_service::CountingResponse>> reader(
    stub_->CountNumbers(&context, request));

  while (reader->Read(&response)) {
    std::cout << "Received number: " << response.current_number() << std::endl;

    // 중간 지점 계산
    const bool is_middle_number =
      (end_number_ - start_number_) % 2 == 0 ?
      (response.current_number() == (start_number_ + end_number_) / 2) :
      (response.current_number() == (start_number_ + end_number_ + 1) / 2);

    // 중간 지점에서 prefix 변경
    if (is_middle_number) {
      this->change_prefix("New number: ");
    }
  }

  grpc::Status status = reader->Finish();
  if (status.ok()) {
    std::cout << "Counting completed successfully." << std::endl;
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
  }
}
```

**특징:**
- `WaitForConnected()`로 서버 연결 대기 (최대 100초)
- `ClientReader`로 스트리밍 데이터 수신
- Mersenne Twister(mt19937) 난수 생성기 사용
- **동적 로직**: 스트리밍 중간에 `ChangePrefix` RPC를 호출하여 서버 상태 변경
- **중간 지점 계산**: 홀수/짝수 개수에 따라 정확한 중간값 계산

### 3. Docker Compose 네트워킹

```yaml
services:
  counting-request-client:
    build:
      context: .
      dockerfile: counting_request_client/Dockerfile.release
  counting-response-server:
    environment:
      - PYTHONUNBUFFERED=1
    build:
      context: .
      dockerfile: counting_response_server/Dockerfile.release
```

**주요 포인트:**
- 서비스 이름이 DNS로 자동 등록 (`counting-response-server`)
- 명시적인 네트워크 설정 없이 같은 compose 파일의 서비스끼리 통신 가능
- `PYTHONUNBUFFERED=1`로 Python 출력 버퍼링 비활성화

## 💡 학습 포인트

### 1. gRPC 통신 패턴
- **Server Streaming**: 단일 요청에 대해 여러 응답을 순차적으로 전송
- **Unary RPC**: 단일 요청-응답 패턴으로 서버 상태 변경
- 실시간 데이터 피드와 제어 명령을 함께 사용하는 패턴
- Python: `yield`로 구현, C++: `ClientReader`로 수신

### 2. 다중 언어 지원 (Polyglot)
- 동일한 `.proto` 파일에서 Python과 C++ 코드 자동 생성
- 언어 간 완벽한 상호 운용성
- Protocol Buffers를 통한 효율적인 직렬화

### 3. Docker 마이크로서비스 통신
- Docker Compose의 내부 DNS를 통한 서비스 디스커버리
- 서비스 이름으로 다른 컨테이너에 접근 가능
- 채널 연결 타임아웃 처리의 중요성

### 4. 동시성 및 상태 관리
- **서버**: ThreadPoolExecutor를 사용한 병렬 요청 처리 (최대 10개)
- **주의사항**: 공유 상태(`self.prefix`) 접근 시 Race Condition 가능
- **해결방법**: 스레드 락 사용 또는 클라이언트별 상태 관리 필요
- **클라이언트**: 블로킹 방식의 스트림 수신, 중간에 Unary RPC 호출

### 5. gRPC ClientContext
- 요청 메타데이터 및 설정 관리
- 타임아웃(deadline) 설정 가능 (기본값: 무제한)
- 압축, 인증, 메타데이터 전송 등 다양한 옵션 제공
- HTTP 헤더와 유사한 역할

## 🔧 빌드 스크립트

### Python 서버

**빌드 방법:**
```bash
cd counting_response_server
source build.sh
build_counting_response_server
```

**빌드 스크립트 내용:**
```bash
function build_counting_response_server() {
  SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

  # Generate protobuf files in current directory (not subdirectory)
  cd "$SCRIPT_DIR"
  python3 -m grpc_tools.protoc -I../proto \
    --python_out=. \
    --pyi_out=. \
    --grpc_python_out=. \
    ../proto/counting_service.proto

  echo "Build complete! Generated files in $SCRIPT_DIR"
}
```

**생성되는 파일 (서버 디렉토리에 직접 생성):**
- `counting_service_pb2.py` - Protocol Buffers 메시지 정의
- `counting_service_pb2.pyi` - 타입 힌트 파일
- `counting_service_pb2_grpc.py` - gRPC 서비스 스텁

**설계 결정:**
- ✅ **같은 디렉토리 생성**: Python gRPC의 import 문제 방지 (가장 안정적)
- ✅ `.gitignore`에서 `*_pb2*.py` 패턴으로 자동 제외
- ✅ `protoc`가 생성하는 절대 경로 import와 호환

### C++ 클라이언트

**빌드 방법:**
```bash
cd counting_request_client
source build.sh
build_counting_request_client
```

**빌드 스크립트 내용:**
```bash
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
```

**빌드 결과물:**
- `build/counting_request_client` - 실행 파일
- `build/generated_proto/*.pb.cc` - 생성된 Protocol Buffers C++ 파일
- `build/generated_proto/*.grpc.pb.cc` - 생성된 gRPC C++ 파일

**특징:**
- ✅ **Out-of-source Build**: 소스 코드와 빌드 결과물 완전 분리
- ✅ **워크스페이스 내 빌드**: 더 이상 홈 디렉토리로 복사하지 않음
- ✅ **즉시 반영**: 소스 코드 수정 후 빌드하면 바로 반영
- ✅ **자동 proto 생성**: CMake가 protobuf 파일 자동 생성

## 📚 개발 환경 설정

### Dev Container 사용 (권장)

VS Code에서 Dev Container를 사용하여 개발할 수 있습니다:

1. VS Code에서 프로젝트 열기
2. Command Palette (Ctrl+Shift+P) → "Dev Containers: Reopen in Container"
3. 개발하고 싶은 컨테이너 선택:
   - `counting_request_client` (C++ 개발 환경)
   - `counting_response_server` (Python 개발 환경)

### 로컬 개발 워크플로우

**C++ 클라이언트 개발:**
```bash
# Dev Container 내에서
cd /workspaces/grpc_test/counting_request_client
source build.sh && build_counting_request_client
./build/counting_request_client
```

**Python 서버 개발:**
```bash
# Dev Container 내에서
cd /workspaces/grpc_test/counting_response_server
source build.sh && build_counting_response_server
python3 main.py
```

### gRPC 설치 (로컬 환경)

프로젝트는 Docker 환경에서 실행되므로 로컬에 gRPC를 설치할 필요는 없지만, 개발 환경 구성이 필요한 경우:
- Python: `scripts/install_grpc_python.sh`
- C++: `scripts/install_grpc_cplusplus.sh`

### 빌드 시스템

**C++ (CMake):**
- CMake 3.16 이상
- Out-of-source build 방식
- `build/` 디렉토리에 빌드 결과물 생성
- protobuf 파일 자동 생성

**Python:**
- `grpc_tools.protoc`를 사용한 코드 생성
- 생성된 파일은 서버 디렉토리에 저장
- 모든 생성 파일은 `.gitignore`에 포함

### 통신 포트
- gRPC 서버: `9000`
- 프로토콜: HTTP/2 (gRPC 기본)
- 보안: InsecureChannel (테스트용, 프로덕션에서는 TLS 사용 권장)

## 🐛 트러블슈팅

### 클라이언트가 서버에 연결하지 못하는 경우
1. **서비스 이름 확인**: `docker-compose.yml`의 서비스 이름과 클라이언트 코드의 주소가 일치하는지 확인
   - 예: `counting-response-server:9000`
2. **포트 번호 확인**: 서버와 클라이언트 모두 `9000` 포트 사용
3. **컨테이너 재시작**: `docker compose down && docker compose up --build`

### 빌드 실패 시
1. **빌드 디렉토리 정리**:
   ```bash
   # C++ 클라이언트
   rm -rf counting_request_client/build

   # Python 서버 (생성된 파일 삭제)
   rm -f counting_response_server/*_pb2.py
   rm -f counting_response_server/*_pb2.pyi
   rm -f counting_response_server/*_pb2_grpc.py
   ```

2. **Docker 이미지 재빌드**:
   ```bash
   docker compose down
   docker compose build --no-cache
   docker compose up
   ```

### 소스 코드 수정이 반영되지 않는 경우
- ✅ **Dev Container**: 워크스페이스에서 직접 빌드하므로 즉시 반영됨
- ✅ **Docker Compose**: 이미지를 재빌드해야 함 (`--build` 옵션)

### 로그 확인
```bash
# 모든 서비스 로그
docker compose logs -f

# 특정 서비스만
docker compose logs -f counting-response-server
docker compose logs -f counting-request-client

# Background 실행 후 로그 보기 (권장)
docker compose up -d
docker compose logs -f
```

## 🎓 학습 자료

### 프로젝트에서 사용된 개념

1. **Out-of-source Build**
   - 소스 코드와 빌드 결과물을 분리하는 표준 방식
   - CMake 공식 권장 방법
   - 참고: [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)

2. **gRPC Server Streaming**
   - 단일 요청에 대해 여러 응답을 스트리밍
   - 참고: [gRPC Concepts](https://grpc.io/docs/what-is-grpc/core-concepts/)

3. **Protocol Buffers**
   - 효율적인 직렬화 포맷
   - 다중 언어 지원
   - 참고: [Protocol Buffers Guide](https://protobuf.dev/)

4. **Docker 마이크로서비스**
   - Docker Compose를 통한 서비스 오케스트레이션
   - 내부 DNS를 통한 서비스 디스커버리
   - 참고: [Docker Compose Networking](https://docs.docker.com/compose/networking/)
