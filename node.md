# 목표

- Scanner 구현부와 UI 분리
- 기능 DLL화

## 세부 목표

- 

## 전체 기능 목록

- LogUtils
- CMDLineUtils
- IPCUtils
- FileUtils
- hashUtils
- PEParser
- RegWatch
- ScanEngine

## 공유 기능

- FileUtils : 파일탐색
- IPCUtils : 프로세스간 통신
- LogUtil : 로킹

## Scanner 기능

- CMDLineUtils : 명령줄

## ScannerDaemon 기능

- HashUtils : 해시
- PEParser
    - MemUtil
        - PEProcessUtil
        - PEMemMapUtil
- RegWatch : 레지스트리 모니터링
- ScanEngine : Scanner 관련