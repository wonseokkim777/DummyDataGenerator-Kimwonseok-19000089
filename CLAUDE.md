# CLAUDE.md — DummyDataGenerator2

반도체 시료 생산주문관리 더미 데이터 생성기 프로젝트에 대한 Claude Code 작업 가이드입니다.

---

## 프로젝트 개요

반도체 시료 생산주문관리(Semiconductor Sample Production Order Management) 도메인의
테스트용 더미 데이터를 생성하는 C++17 PoC 도구.

## 목적

- 반도체 공정 관리 시스템 테스트 시 필요한 현실적인 더미 데이터 제공
- JSON 템플릿 파일 기반으로 커스터마이징 가능한 데이터 생성
- 재현 가능한(reproducible) 데이터를 위한 시드(seed) 지원
- 외부 라이브러리 없이 C++ 표준 라이브러리만 사용

## 아키텍처

```
DummyDataGenerator-KyusungPark-19017482/
├── CMakeLists.txt                              # CMake 빌드 정의
├── src/
│   ├── main.cpp                               # CLI 진입점, 인수 파싱
│   ├── SimpleJsonParser.h / .cpp              # 경량 JSON 파서
│   └── DummyDataGenerator.h / .cpp           # 핵심 데이터 생성 로직
├── data/
│   └── templates/
│       └── production_order_template.json    # 생성 값 정의 템플릿
└── output/                                   # 생성된 더미 데이터 저장소
```

## 핵심 클래스 및 구조체

### `ProductionOrder` (구조체)

| 필드 | 타입 | 설명 |
|------|------|------|
| `orderId` | string | ORD-YYYYMMDD-NNNNNN |
| `customerId` | string | 고객사명 |
| `sampleName` | string | 시료명 (자동 조합 생성) |
| `sampleType` | string | Wafer / Die / Package 등 |
| `lotNumber` | string | LOT-YYYY-NNN-XX |
| `processStep` | string | 공정 단계 |
| `productionLine` | string | 생산 라인 (Line-A ~ Line-E) |
| `equipmentId` | string | 장비 ID |
| `operatorId` | string | 작업자 ID (OPR-NNN) |
| `materialCode` | string | 원자재 코드 |
| `waferSizeMm` | int | 웨이퍼 직경 mm |
| `nodeSizeNm` | int | 공정 노드 nm |
| `status` | string | 주문 상태 |
| `priority` | string | 우선순위 |
| `quantity` | int | 수량 |
| `unitPrice` | double | 단가 |
| `orderDate` | string | 주문일 (YYYY-MM-DD) |
| `dueDate` | string | 납기일 (YYYY-MM-DD) |
| `thicknessUm` | double | 두께 μm |
| `defectCount` | int | 결함 수 |
| `yieldRate` | double | 수율 % |

### `DummyDataGenerator`

| 메서드 | 설명 |
|--------|------|
| `loadTemplate(path)` | JSON 템플릿 로드 (실패 시 기본값 사용) |
| `generate(count)` | `ProductionOrder` 벡터 생성 |
| `saveToFile(path, orders)` | JSON 파일 저장 (디렉토리 자동 생성) |
| `toJson(orders)` | JSON 문자열 변환 |

### `SimpleJsonParser`

외부 라이브러리 없이 구현한 경량 JSON 파서.

- 도트 표기법으로 중첩 값 조회: `"templates.sample_types"`, `"ranges.quantity_min"`
- 지원 API: `getStringArray()`, `getNumber()`, `getInt()`
- **제약**: 키가 문자열 값 안에 중복 등장하는 특수 JSON은 미지원 (PoC 범위)

## 빌드

```bash
cmake -B build -S .
cmake --build build --config Release
```

빌드 결과물: `build/bin/DummyDataGenerator` (Linux/macOS) 또는 `build/bin/Release/DummyDataGenerator.exe` (Windows)

## 실행

```bash
# 기본 실행 (10개, 랜덤 시드)
./build/bin/DummyDataGenerator

# 100개 생성, 시드 고정
./build/bin/DummyDataGenerator -n 100 -s 42

# 출력 파일 지정
./build/bin/DummyDataGenerator -n 200 -o output/test.json
```

## 템플릿 커스터마이징

`data/templates/production_order_template.json` 수정:

- `templates.*` — 각 필드의 가능한 값 목록 (string 또는 number 배열)
- `ranges.*` — 숫자 필드의 최솟값/최댓값

## 의존성

- **C++17** 표준 라이브러리만 사용 (외부 라이브러리 없음)
- CMake 3.16 이상
- 컴파일러: GCC 9+ / Clang 9+ / MSVC 2019+
