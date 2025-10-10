# Hot Reload 사용 방법

## 개요

시계 로직과 렌더링 부분을 `libclock.so` 동적 라이브러리로 분리하여 Hot reload가 가능하도록 구현했습니다.

## 파일 구조

- `clock_lib.h` - 시계 라이브러리 헤더 (API 정의)
- `clock_lib.cpp` - 시계 로직 및 렌더링 구현
- `main.cpp` - 메인 애플리케이션 (라이브러리 동적 로딩)
- `libclock.so` - 컴파일된 공유 라이브러리
- `rebuild_lib.sh` - 라이브러리만 재빌드하는 스크립트

## 빌드 방법

```bash
# 전체 프로젝트 빌드
cmake -B build -S .
cmake --build build

# 실행
cd build
./main
```

## Hot Reload 테스트 방법

### 1. 애플리케이션 실행

```bash
cd build
./main
```

시계 창이 나타나며 왼쪽 상단에 라이브러리 버전(v1)이 표시됩니다.

### 2. 라이브러리 수정

다른 터미널에서 `clock_lib.cpp`를 수정합니다. 예를 들어:

```cpp
// LIB_VERSION을 증가시킵니다
#define LIB_VERSION 2

// 또는 시계 색상을 변경합니다
DrawClockHand(clock->center, clock->secondAngle, clock->secondHandLength, 1.0f, BLUE);  // RED -> BLUE
```

### 3. 라이브러리만 재빌드

```bash
./rebuild_lib.sh
```

또는 수동으로:

```bash
cd build
cmake --build . --target clock
```

### 4. 자동 리로드 확인

애플리케이션이 실행 중인 상태에서 약 1초 후 자동으로 라이브러리가 리로드됩니다:
- 콘솔에 "Library file changed, reloading..." 메시지가 출력됩니다
- 콘솔에 "Loaded library version X" 메시지가 출력됩니다
- 왼쪽 상단의 버전 번호가 업데이트됩니다
- 변경된 내용이 즉시 반영됩니다 (예: 초침 색상 변경)

## Hot Reload 동작 원리

1. **파일 모니터링**: 메인 애플리케이션이 1초마다 `libclock.so` 파일의 수정 시간을 확인합니다.

2. **동적 로딩**: 파일이 변경되면 `dlopen`/`dlclose`를 사용하여 라이브러리를 언로드하고 재로드합니다.

3. **함수 포인터 갱신**: 새로운 라이브러리에서 함수 포인터를 다시 로드합니다.

4. **버전 확인**: `GetLibVersion()` 함수로 라이브러리 버전을 확인할 수 있습니다.

## 수정 가능한 부분

`clock_lib.cpp`에서 다음을 실시간으로 수정할 수 있습니다:

- 시계 바늘 색상, 두께, 길이
- 시계판 디자인 (숫자, 눈금 스타일)
- 시계 배경색
- 렌더링 로직
- 시간 계산 로직

## 주의사항

1. **구조체 변경**: `AnalogClock` 구조체의 멤버를 변경하면 메인 애플리케이션도 재컴파일해야 합니다.

2. **API 변경**: 함수 시그니처를 변경하면 메인 애플리케이션도 재컴파일해야 합니다.

3. **빌드 완료 대기**: 라이브러리 빌드가 완료될 때까지 기다린 후 리로드가 발생합니다 (1초 간격으로 체크).

## 개발 워크플로우

1. `./main` 실행 (한 번만)
2. `clock_lib.cpp` 수정
3. `./rebuild_lib.sh` 실행
4. 변경사항이 자동으로 반영됨 → 2번으로 돌아가기

이 방식으로 애플리케이션을 재시작하지 않고도 시계 디자인과 로직을 빠르게 반복 개발할 수 있습니다.
