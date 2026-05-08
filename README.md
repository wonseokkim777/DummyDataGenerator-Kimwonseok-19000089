# 반도체 시료 생산주문관리 더미 데이터 생성기

테스트용 JSON 더미 데이터를 생성하는 C++17 콘솔 애플리케이션입니다.  
**도메인**: 반도체 시료 생산주문관리 (Semiconductor Sample Production Order Management)

---

## 특징

- **단일 구조체** `ProductionOrder` — 21개 필드로 구성된 간결한 도메인 모델
- **템플릿 기반 커스터마이징** — `data/templates/production_order_template.json` 수정만으로 코드 변경 없이 생성 값 제어
- **CLI 인수 지원** — 생성 건수·시드·출력 경로를 실행 시점에 지정
- **재현 가능한 데이터** — 시드 고정 시 항상 동일한 결과
- **외부 라이브러리 없음** — C++17 표준 라이브러리만 사용

---

## 요구 사항

| 항목 | 버전 |
|---|---|
| Visual Studio | 2022 (v143 toolset) |
| C++ 표준 | C++17 |
| 대상 플랫폼 | Windows x64 / x86 |

---

## 빌드

### Visual Studio IDE
1. `DummyDataGenerator2.sln` 파일을 열기
2. 구성: **Debug \| x64** (또는 Release \| x64)
3. **빌드 → 솔루션 빌드** (`Ctrl+Shift+B`)

### MSBuild (커맨드라인)
```bat
msbuild DummyDataGenerator2\DummyDataGenerator2.vcxproj ^
        /p:Configuration=Debug /p:Platform=x64 /t:Build
```

빌드 결과물: `DummyDataGenerator2\x64\Debug\DummyDataGenerator2.exe`

---

## 실행

```bat
REM 기본 실행 (10건, 랜덤 시드)
DummyDataGenerator2\x64\Debug\DummyDataGenerator2.exe

REM 100건 생성, 시드 고정
DummyDataGenerator2\x64\Debug\DummyDataGenerator2.exe -n 100 -s 42

REM 생성 결과를 콘솔에 출력
DummyDataGenerator2\x64\Debug\DummyDataGenerator2.exe -n 5 -s 42 -p

REM 출력 파일 경로 지정
DummyDataGenerator2\x64\Debug\DummyDataGenerator2.exe -n 200 -o output\test.json

REM 커스텀 템플릿 사용
DummyDataGenerator2\x64\Debug\DummyDataGenerator2.exe -n 50 -t my_template.json
```

### 옵션

| 옵션 | 기본값 | 설명 |
|---|---|---|
| `-n <count>` | `10` | 생성 건수 |
| `-s <seed>` | 랜덤 | RNG 시드 (고정 시 재현 가능) |
| `-o <path>` | `output/dummy_YYYYMMDD_HHMMSS.json` | 출력 파일 경로 |
| `-t <path>` | `data/templates/production_order_template.json` | JSON 템플릿 파일 경로 |
| `-p` | off | 생성 결과를 콘솔에 출력 |
| `-h` | — | 도움말 출력 |

### 콘솔 출력 예시

```
[Semiconductor Sample Production Order - Dummy Data Generator]
Template loaded: data/templates/production_order_template.json
Generating... (3 records)

========================================================================
 Generated Orders (3 records)
========================================================================

 [  1] ORD-20260508-000001
------------------------------------------------------------------------
  Customer   : 앰코테크놀로지
  Sample     : Die-130nm-Ann-3337  (Die, 130nm, 200mm wafer)
  Lot        : LOT-2026-001-BD
  Process    : Anneal  [Line-C / STR-023 / OPR-032]
  Material   : Si-W05809
  Schedule   : 2026-05-08 -> 2026-07-16
  Status     : OnHold        Priority: Normal
  Qty        : 61   Price: 3,554,959 KRW   Thickness: 522.56 um
  Defects    : 49   Yield: 93.21%

 [  2] ...

========================================================================
 Total: 3 records
========================================================================

Saved: output/dummy_20260508_141431.json
```

---

## 생성 데이터 구조

### 최상위 스키마

```json
{
  "generatedAt": "2026-05-08",
  "count": 100,
  "orders": [ ... ]
}
```

### ProductionOrder 필드

| 필드 | 타입 | 예시 | 설명 |
|---|---|---|---|
| `orderId` | string | `ORD-20260508-000001` | 주문 고유 ID |
| `customerId` | string | `삼성전자 DS부문` | 고객사명 |
| `sampleName` | string | `Wafer-28nm-Lit-0042` | 시료명 (자동 조합 생성) |
| `sampleType` | string | `Wafer` | Wafer / Die / Package 등 |
| `lotNumber` | string | `LOT-2026-001-AC` | 로트 번호 |
| `processStep` | string | `Lithography` | 공정 단계 |
| `productionLine` | string | `Line-A` | 생산 라인 |
| `equipmentId` | string | `STR-007` | 장비 ID |
| `operatorId` | string | `OPR-042` | 작업자 ID |
| `materialCode` | string | `Si-W00123` | 원자재 코드 |
| `waferSizeMm` | int | `300` | 웨이퍼 직경 (mm) |
| `nodeSizeNm` | int | `28` | 공정 노드 (nm) |
| `status` | string | `InProcess` | Draft / Confirmed / InProcess / Completed / OnHold / Cancelled |
| `priority` | string | `High` | Critical / High / Normal / Low |
| `quantity` | int | `75` | 수량 |
| `unitPrice` | number | `1250000.00` | 단가 |
| `orderDate` | string | `2026-05-08` | 주문일 (YYYY-MM-DD) |
| `dueDate` | string | `2026-07-12` | 납기일 (YYYY-MM-DD) |
| `thicknessUm` | number | `725.40` | 웨이퍼 두께 (μm) |
| `defectCount` | int | `3` | 결함 수 |
| `yieldRate` | number | `97.20` | 수율 (%) |

### 출력 예시 (1건)

```json
{
  "orderId":        "ORD-20260508-000001",
  "customerId":     "SK하이닉스",
  "sampleName":     "Die-130nm-Ann-3337",
  "sampleType":     "Die",
  "lotNumber":      "LOT-2026-001-AC",
  "processStep":    "Anneal",
  "productionLine": "Line-C",
  "equipmentId":    "CVD-031",
  "operatorId":     "OPR-188",
  "materialCode":   "Si-W09182",
  "waferSizeMm":    150,
  "nodeSizeNm":     130,
  "status":         "OnHold",
  "priority":       "Normal",
  "quantity":       61,
  "unitPrice":      3554959.26,
  "orderDate":      "2026-05-08",
  "dueDate":        "2026-07-16",
  "thicknessUm":    683.21,
  "defectCount":    12,
  "yieldRate":      93.21
}
```

---

## 템플릿 커스터마이징

`data/templates/production_order_template.json`을 수정합니다.

```json
{
  "templates": {
    "customers":       ["회사A", "회사B", ...],
    "sample_types":    ["Wafer", "Die", ...],
    "process_steps":   ["Lithography", "Etching", ...],
    "statuses":        ["Confirmed", "InProcess", ...],
    "priorities":      ["High", "Normal", ...],
    "production_lines":["Line-A", "Line-B", ...]
  },
  "ranges": {
    "quantity_min":   1,
    "quantity_max":   100,
    "unit_price_min": 50000,
    "unit_price_max": 5000000,
    "thickness_min":  500,
    "thickness_max":  900,
    "defect_min":     0,
    "defect_max":     50,
    "yield_min":      60.0,
    "yield_max":      99.9
  }
}
```

- `templates.*` — 각 필드의 가능한 값 목록 (무작위 선택)
- `ranges.*` — 숫자 필드의 최솟값 / 최댓값
- 템플릿 파일이 없거나 로드 실패 시 내부 기본값으로 자동 폴백

---

## 프로젝트 구조

```
DummyDataGenerator2/
├── DummyDataGenerator2.sln
├── DummyDataGenerator2/
│   └── DummyDataGenerator2.vcxproj  # C++17, /utf-8, _CRT_SECURE_NO_WARNINGS
├── src/
│   ├── main.cpp                     # CLI 진입점, 인수 파싱
│   ├── SimpleJsonParser.h / .cpp    # 경량 JSON 파서 (도트 표기법 조회)
│   └── DummyDataGenerator.h / .cpp  # 핵심 데이터 생성 로직
├── data/
│   └── templates/
│       └── production_order_template.json
└── output/                          # 생성된 더미 데이터 저장소
```

---

## 라이선스

내부 테스트 전용 PoC 코드입니다.
