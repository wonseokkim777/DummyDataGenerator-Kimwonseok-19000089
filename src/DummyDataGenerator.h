#pragma once
#include <string>
#include <vector>
#include <random>
#include "SimpleJsonParser.h"

// ── 도메인 구조체 ────────────────────────────────────
struct ProductionOrder {
    std::string orderId;        // ORD-YYYYMMDD-NNNNNN
    std::string customerId;     // 고객사명
    std::string sampleName;     // 시료명 (자동 조합 생성)
    std::string sampleType;     // Wafer / Die / Package 등
    std::string lotNumber;      // LOT-YYYY-NNN-XX
    std::string processStep;    // 공정 단계
    std::string productionLine; // Line-A ~ Line-E
    std::string equipmentId;    // 장비 ID
    std::string operatorId;     // OPR-NNN
    std::string materialCode;   // 원자재 코드
    int    waferSizeMm;         // 웨이퍼 직경 mm
    int    nodeSizeNm;          // 공정 노드 nm
    std::string status;         // 주문 상태
    std::string priority;       // 우선순위
    int    quantity;
    double unitPrice;
    std::string orderDate;      // YYYY-MM-DD
    std::string dueDate;        // YYYY-MM-DD
    double thicknessUm;         // 두께 μm
    int    defectCount;
    double yieldRate;           // %
};

// ── 생성기 ───────────────────────────────────────────
class DummyDataGenerator {
public:
    explicit DummyDataGenerator(unsigned seed = 0); // 0 = 랜덤 시드

    // 템플릿 로드 (실패 시 내부 기본값 사용)
    void loadTemplate(const std::string& path);

    std::vector<ProductionOrder> generate(int count);
    std::string toJson(const std::vector<ProductionOrder>& orders);
    void saveToFile(const std::string& path,
                    const std::vector<ProductionOrder>& orders);

private:
    SimpleJsonParser tpl_;
    bool             tplLoaded_ = false;
    std::mt19937     rng_;
    int              seq_ = 1;

    // ── 필드별 선택지 (템플릿 또는 기본값) ──
    std::vector<std::string> customers_;
    std::vector<std::string> sampleTypes_;
    std::vector<std::string> processSteps_;
    std::vector<std::string> statuses_;
    std::vector<std::string> priorities_;
    std::vector<std::string> productionLines_;

    // ── 숫자 범위 ──
    int    quantityMin_, quantityMax_;
    double priceMin_,    priceMax_;
    double thicknessMin_,thicknessMax_;
    int    defectMin_,   defectMax_;
    double yieldMin_,    yieldMax_;

    void applyDefaults();
    void applyTemplate();

    ProductionOrder makeOrder(int idx);
    const std::string& pick(const std::vector<std::string>& v);
    int    randInt(int lo, int hi);
    double randDouble(double lo, double hi);
    static std::string today();
    static std::string offsetDate(const std::string& base, int days);
    static std::string fmtDate(int y, int m, int d);
};
