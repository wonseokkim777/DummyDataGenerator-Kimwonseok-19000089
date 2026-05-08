#include "DummyDataGenerator.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <filesystem>
#include <ctime>

namespace fs = std::filesystem;

// ════════════════════════════════════════════════════
// 생성자
// ════════════════════════════════════════════════════
DummyDataGenerator::DummyDataGenerator(unsigned seed) {
    rng_.seed(seed != 0 ? seed : std::random_device{}());
    applyDefaults();
}

// ── 기본 참조 데이터 (템플릿 없을 때 사용) ───────────
void DummyDataGenerator::applyDefaults() {
    customers_      = { "삼성전자 DS부문", "SK하이닉스", "DB하이텍", "키파운드리",
                        "매그나칩반도체", "LX세미콘", "네패스", "하나마이크론" };
    sampleTypes_    = { "Wafer", "Die", "Package", "Chip-on-Board", "Bare Die" };
    processSteps_   = { "Incoming Inspection", "Gate Oxidation", "Lithography",
                        "Etching", "Ion Implant", "CVD", "CMP", "Metal PVD",
                        "Anneal", "Final Electrical Test" };
    statuses_       = { "Draft", "Confirmed", "InProcess", "Completed", "OnHold", "Cancelled" };
    priorities_     = { "Critical", "High", "Normal", "Low" };
    productionLines_= { "Line-A", "Line-B", "Line-C", "Line-D", "Line-E" };

    quantityMin_   = 1;    quantityMax_   = 100;
    priceMin_      = 50000.0; priceMax_   = 5000000.0;
    thicknessMin_  = 500.0; thicknessMax_ = 900.0;
    defectMin_     = 0;    defectMax_     = 50;
    yieldMin_      = 60.0; yieldMax_      = 99.9;
}

// ── 템플릿 값으로 덮어쓰기 ───────────────────────────
void DummyDataGenerator::applyTemplate() {
    auto overwrite = [&](std::vector<std::string>& dst, const std::string& key) {
        auto v = tpl_.getStringArray(key);
        if (!v.empty()) dst = std::move(v);
    };
    overwrite(customers_,       "templates.customers");
    overwrite(sampleTypes_,     "templates.sample_types");
    overwrite(processSteps_,    "templates.process_steps");
    overwrite(statuses_,        "templates.statuses");
    overwrite(priorities_,      "templates.priorities");
    overwrite(productionLines_, "templates.production_lines");

    quantityMin_  = tpl_.getInt   ("ranges.quantity_min",    quantityMin_);
    quantityMax_  = tpl_.getInt   ("ranges.quantity_max",    quantityMax_);
    priceMin_     = tpl_.getNumber("ranges.unit_price_min",  priceMin_);
    priceMax_     = tpl_.getNumber("ranges.unit_price_max",  priceMax_);
    thicknessMin_ = tpl_.getNumber("ranges.thickness_min",  thicknessMin_);
    thicknessMax_ = tpl_.getNumber("ranges.thickness_max",  thicknessMax_);
    defectMin_    = tpl_.getInt   ("ranges.defect_min",      defectMin_);
    defectMax_    = tpl_.getInt   ("ranges.defect_max",      defectMax_);
    yieldMin_     = tpl_.getNumber("ranges.yield_min",       yieldMin_);
    yieldMax_     = tpl_.getNumber("ranges.yield_max",       yieldMax_);
}

// ════════════════════════════════════════════════════
// 공개 API
// ════════════════════════════════════════════════════
void DummyDataGenerator::loadTemplate(const std::string& path) {
    tplLoaded_ = tpl_.load(path);
    if (tplLoaded_) applyTemplate();
}

std::vector<ProductionOrder> DummyDataGenerator::generate(int count) {
    std::vector<ProductionOrder> result;
    result.reserve(count);
    for (int i = 0; i < count; ++i)
        result.push_back(makeOrder(i));
    return result;
}

// ════════════════════════════════════════════════════
// 주문 1건 생성
// ════════════════════════════════════════════════════
ProductionOrder DummyDataGenerator::makeOrder(int /*idx*/) {
    ProductionOrder o;

    // ── ID / 번호 ──
    std::string dateStr = today();
    std::ostringstream oid;
    oid << "ORD-" << dateStr.substr(0, 4) << dateStr.substr(5, 2) << dateStr.substr(8, 2)
        << "-" << std::setw(6) << std::setfill('0') << seq_;
    o.orderId = oid.str();

    int lotSeq = seq_++;
    std::ostringstream lot;
    lot << "LOT-" << dateStr.substr(0, 4)
        << "-" << std::setw(3) << std::setfill('0') << (lotSeq % 1000)
        << "-" << static_cast<char>('A' + randInt(0, 4))
        << static_cast<char>('A' + randInt(0, 4));
    o.lotNumber = lot.str();

    // ── 기본 필드 ──
    o.customerId     = pick(customers_);
    o.sampleType     = pick(sampleTypes_);
    o.processStep    = pick(processSteps_);
    o.status         = pick(statuses_);
    o.priority       = pick(priorities_);
    o.productionLine = pick(productionLines_);

    // ── 장비 / 작업자 / 원자재 ──
    static const std::vector<std::string> eqpTypes = { "DIF", "STR", "ETC", "CVD", "CMP", "IMP", "MET" };
    std::ostringstream eqp;
    eqp << pick(eqpTypes) << "-" << std::setw(3) << std::setfill('0') << randInt(1, 50);
    o.equipmentId = eqp.str();

    std::ostringstream opr;
    opr << "OPR-" << std::setw(3) << std::setfill('0') << randInt(1, 200);
    o.operatorId = opr.str();

    static const std::vector<std::string> matPfx = { "Si-W", "SiC-W", "GaN-W", "CHM", "GAS", "PR-" };
    std::ostringstream mat;
    mat << pick(matPfx) << std::setw(5) << std::setfill('0') << randInt(1, 99999);
    o.materialCode = mat.str();

    // ── 웨이퍼 사양 ──
    static const std::vector<int> diameters = { 150, 200, 300 };
    static const std::vector<int> nodes     = { 7, 10, 14, 28, 40, 65, 90, 130, 180 };
    o.waferSizeMm = diameters[randInt(0, (int)diameters.size() - 1)];
    o.nodeSizeNm  = nodes    [randInt(0, (int)nodes.size() - 1)];

    // ── 시료명 자동 조합 ──
    std::ostringstream sn;
    sn << o.sampleType << "-" << o.nodeSizeNm << "nm-"
       << o.processStep.substr(0, 3) << "-"
       << std::setw(4) << std::setfill('0') << randInt(1, 9999);
    o.sampleName = sn.str();

    // ── 수량 / 단가 ──
    o.quantity  = randInt(quantityMin_, quantityMax_);
    o.unitPrice = randDouble(priceMin_, priceMax_);

    // ── 날짜 ──
    o.orderDate = today();
    o.dueDate   = offsetDate(o.orderDate, randInt(14, 90));

    // ── 물성 / 품질 ──
    o.thicknessUm = randDouble(thicknessMin_, thicknessMax_);
    o.defectCount = randInt(defectMin_, defectMax_);
    o.yieldRate   = randDouble(yieldMin_, yieldMax_);

    return o;
}

// ════════════════════════════════════════════════════
// JSON 직렬화
// ════════════════════════════════════════════════════
static std::string jStr(const std::string& s) {
    std::string r = "\"";
    for (char c : s) {
        if (c == '"')  r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else r += c;
    }
    return r + "\"";
}

std::string DummyDataGenerator::toJson(const std::vector<ProductionOrder>& orders) {
    std::ostringstream o;
    o << std::fixed << std::setprecision(2);
    o << "{\n"
      << "  \"generatedAt\": " << jStr(today()) << ",\n"
      << "  \"count\": " << orders.size() << ",\n"
      << "  \"orders\": [\n";

    for (size_t i = 0; i < orders.size(); ++i) {
        const auto& p = orders[i];
        o << "    {\n"
          << "      \"orderId\":        " << jStr(p.orderId)        << ",\n"
          << "      \"customerId\":     " << jStr(p.customerId)     << ",\n"
          << "      \"sampleName\":     " << jStr(p.sampleName)     << ",\n"
          << "      \"sampleType\":     " << jStr(p.sampleType)     << ",\n"
          << "      \"lotNumber\":      " << jStr(p.lotNumber)      << ",\n"
          << "      \"processStep\":    " << jStr(p.processStep)    << ",\n"
          << "      \"productionLine\": " << jStr(p.productionLine) << ",\n"
          << "      \"equipmentId\":    " << jStr(p.equipmentId)    << ",\n"
          << "      \"operatorId\":     " << jStr(p.operatorId)     << ",\n"
          << "      \"materialCode\":   " << jStr(p.materialCode)   << ",\n"
          << "      \"waferSizeMm\":    " << p.waferSizeMm          << ",\n"
          << "      \"nodeSizeNm\":     " << p.nodeSizeNm           << ",\n"
          << "      \"status\":         " << jStr(p.status)         << ",\n"
          << "      \"priority\":       " << jStr(p.priority)       << ",\n"
          << "      \"quantity\":       " << p.quantity              << ",\n"
          << "      \"unitPrice\":      " << p.unitPrice             << ",\n"
          << "      \"orderDate\":      " << jStr(p.orderDate)      << ",\n"
          << "      \"dueDate\":        " << jStr(p.dueDate)        << ",\n"
          << "      \"thicknessUm\":    " << p.thicknessUm          << ",\n"
          << "      \"defectCount\":    " << p.defectCount           << ",\n"
          << "      \"yieldRate\":      " << p.yieldRate             << "\n"
          << "    }";
        if (i + 1 < orders.size()) o << ",";
        o << "\n";
    }
    o << "  ]\n}\n";
    return o.str();
}

void DummyDataGenerator::saveToFile(const std::string& path,
                                    const std::vector<ProductionOrder>& orders) {
    fs::path p(path);
    if (p.has_parent_path())
        fs::create_directories(p.parent_path());

    std::ofstream ofs(path, std::ios::out | std::ios::trunc);
    if (!ofs) throw std::runtime_error("Cannot open output file: " + path);
    ofs << toJson(orders);
}

// ════════════════════════════════════════════════════
// 내부 유틸
// ════════════════════════════════════════════════════
const std::string& DummyDataGenerator::pick(const std::vector<std::string>& v) {
    return v[std::uniform_int_distribution<int>(0, (int)v.size() - 1)(rng_)];
}
int DummyDataGenerator::randInt(int lo, int hi) {
    return std::uniform_int_distribution<int>(lo, hi)(rng_);
}
double DummyDataGenerator::randDouble(double lo, double hi) {
    return std::uniform_real_distribution<double>(lo, hi)(rng_);
}

std::string DummyDataGenerator::fmtDate(int y, int m, int d) {
    std::ostringstream oss;
    oss << y << '-' << std::setw(2) << std::setfill('0') << m
             << '-' << std::setw(2) << std::setfill('0') << d;
    return oss.str();
}

std::string DummyDataGenerator::today() {
    std::time_t t = std::time(nullptr);
    std::tm* tm   = std::localtime(&t);
    return fmtDate(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

std::string DummyDataGenerator::offsetDate(const std::string& base, int days) {
    int y = std::stoi(base.substr(0, 4));
    int m = std::stoi(base.substr(5, 2));
    int d = std::stoi(base.substr(8, 2)) + days;
    // 월 경계 단순 처리 (PoC 범위)
    while (d > 28) { d -= 28; if (++m > 12) { m = 1; ++y; } }
    return fmtDate(y, m, d);
}
