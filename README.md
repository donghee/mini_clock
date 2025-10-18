# mini_clock

Hot Reload 기능을 지원하는 미니 아날로그 시계

<img width="330" height="330" alt="image" src="https://github.com/user-attachments/assets/696297e5-5c60-4358-923f-ed0cb2a7d234" />

## Features

- Raylib 기반
- Hot Reload: 실행 중 시계 디자인 실시간 변경
- 투명한 배경의 200x200 미니 시계
- 항상 최상위 창으로 표시

## Build

```bash
cmake -B build -S .
cmake --build build
```

## Run

```bash
cd build
./main
```

## Hot Reload

시계 실행 중 디자인을 변경하려면:

```bash
# 1. clock_lib.cpp 수정 (색상, 디자인 등)
# 2. 라이브러리만 재빌드
./rebuild_lib.sh
# 또는
cd build && cmake --build . --target clock

# 변경사항이 자동으로 반영됨 (약 1초 후)
```

자세한 사용법은 [README_HOTRELOAD.md](README_HOTRELOAD.md) 참고

## Requirements

- CMake 3.10+
- C++17 compiler
- Raylib (자동 다운로드)

## License

Copyright (c) 2025 Donghee Park
