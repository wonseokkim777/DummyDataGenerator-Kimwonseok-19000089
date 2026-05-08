#include "generator.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

// ════════════════════════════════════════════════════
// 참조 데이터 테이블
// ════════════════════════════════════════════════════
namespace ref {

const std::vector<std::string> CUSTOMER_NAMES = {
    "삼성전자 DS부문", "SK하이닉스", "DB하이텍", "키파운드리", "매그나칩반도체",
    "LX세미콘", "네패스", "테스나", "하나마이크론", "ASE Korea"
};
const std::vector<std::string> CUSTOMER_CODES = {
    "SEC", "SKH", "DBH", "KFY", "MXC", "LXS", "NPX", "TSN", "HNM", "ASK"
};
const std::vector<std::string> COUNTRIES = {
    "Korea", "Korea", "Korea", "Korea", "Korea",
    "Korea", "Korea", "Korea", "Korea", "Korea"
};
const std::vector<std::string> TIERS = { "Gold", "Gold", "Silver", "Silver", "Bronze" };

const std::vector<std::string> PRODUCT_TYPES = { "DRAM", "NAND", "Logic", "Power", "Sensor", "RF" };
const std::vector<int>         PROCESS_NODES  = { 7, 10, 14, 28, 40, 65, 90, 130, 180 };
const std::vector<std::string> WAFER_MATERIALS= { "Si", "SiC", "GaN", "GaAs" };
const std::vector<int>         DIAMETERS      = { 150, 200, 300 };
const std::vector<std::string> ORIENTATIONS   = { "<100>", "<110>", "<111>" };
const std::vector<std::string> DOPING_TYPES   = { "N-type", "P-type" };
const std::vector<std::string> DOPANTS_N      = { "Phosphorus", "Arsenic", "Antimony" };
const std::vector<std::string> DOPANTS_P      = { "Boron", "Aluminum", "Gallium" };

const std::vector<std::string> PRIORITIES = { "Critical", "High", "Normal", "Low" };
const std::vector<std::string> ORDER_STATUSES = {
    "Draft", "Confirmed", "InProcess", "Completed", "Cancelled", "OnHold"
};
const std::vector<std::string> LOT_STATUSES = {
    "Queued", "InProcess", "Completed", "Scrapped", "OnHold"
};

// 공정 플로우 (순서 중요)
struct ProcDef { std::string name; std::string type; double minHr; double maxHr;
                 double minTemp; double maxTemp; double minPres; double maxPres; };

const std::vector<ProcDef> PROCESS_FLOW = {
    {"Incoming Wafer Inspection", "Metrology",   0.5,  1.0,   22,  24,  760,  760},
    {"RCA Clean",                 "Clean",        1.0,  2.0,   22,  80,  760,  760},
    {"Gate Oxidation",            "Thermal",      2.0,  6.0,  900, 1100,  0.1,  760},
    {"Gate Nitride Deposition",   "Deposition",   3.0,  5.0,  700,  900,  0.1,    2},
    {"Photo Lithography",         "Litho",        0.5,  1.5,   22,  23,  760,  760},
    {"Gate Etch",                 "Etch",         0.5,  2.0,   50, 200, 1e-3, 0.05},
    {"Photoresist Strip",         "Clean",        0.5,  1.0,   80, 120,  760,  760},
    {"Source/Drain Implant",      "Implant",      1.0,  3.0,   22,  25, 1e-6, 1e-4},
    {"Anneal",                    "Thermal",      1.0,  4.0,  900, 1050,  0.1,  760},
    {"ILD CVD Deposition",        "Deposition",   2.0,  4.0,  350,  450,  0.1,   10},
    {"CMP Planarization",         "CMP",          1.0,  3.0,   22,  25,  760,  760},
    {"Contact Litho",             "Litho",        0.5,  1.5,   22,  23,  760,  760},
    {"Contact Etch",              "Etch",         0.5,  2.0,   50, 200, 1e-3, 0.05},
    {"Barrier Metal PVD",         "Deposition",   1.0,  2.0,   22,  25, 1e-6, 1e-4},
    {"W Plug CVD",                "Deposition",   2.0,  4.0,  400,  500,  0.1,   10},
    {"Metal 1 PVD",               "Deposition",   1.0,  2.0,   22,  25, 1e-6, 1e-4},
    {"Metal 1 Litho",             "Litho",        0.5,  1.5,   22,  23,  760,  760},
    {"Metal 1 Etch",              "Etch",         0.5,  2.0,   50, 200, 1e-3, 0.05},
    {"IMD CVD",                   "Deposition",   2.0,  4.0,  350,  450,  0.1,   10},
    {"Final Passivation",         "Deposition",   1.0,  2.0,  300,  400,  0.1,    5},
    {"Pad Open Litho",            "Litho",        0.5,  1.5,   22,  23,  760,  760},
    {"Pad Etch",                  "Etch",         0.5,  1.5,   50, 200, 1e-3, 0.05},
    {"Final Electrical Test",     "Metrology",    2.0,  8.0,   22,  25,  760,  760},
};

const std::vector<std::string> EQUIP_TYPES = {
    "DiffusionFurnace", "Stepper", "Etcher", "CVD", "PVD", "CMP",
    "Implanter", "Metrology", "CleanStation"
};
const std::vector<std::string> MANUFACTURERS = {
    "Applied Materials", "Tokyo Electron", "ASML", "Lam Research",
    "KLA Corporation", "Axcelis", "Novellus"
};
const std::vector<std::string> EQUIP_STATUSES = { "Available", "InUse", "Maintenance", "Down" };

const std::vector<std::string> MATERIAL_TYPES = {
    "Wafer", "Chemical", "Gas", "Target", "Photoresist", "CMP-Slurry"
};
const std::vector<std::string> MATERIAL_NAMES_WAFER = {
    "8인치 실리콘 웨이퍼 (P형)", "12인치 실리콘 웨이퍼 (N형)", "SiC 에피택시얼 웨이퍼"
};
const std::vector<std::string> MATERIAL_NAMES_CHEM = {
    "HF 49% 식각액", "H2SO4 98% 황산", "HCl 37% 염산", "SC1 세정액", "SC2 세정액"
};
const std::vector<std::string> MATERIAL_NAMES_GAS = {
    "SiH4 실란 가스", "NH3 암모니아", "N2O 아산화질소", "Cl2 염소 가스", "CHF3 가스"
};
const std::vector<std::string> MATERIAL_NAMES_PR = {
    "KrF 포토레지스트 248nm", "ArF 포토레지스트 193nm", "EUV 포토레지스트 13.5nm"
};
const std::vector<std::string> SUPPLIERS = {
    "신에츠화학", "SUMCO", "SK실트론", "Entegris", "CMC Materials",
    "Air Products", "린데코리아", "동진쎄미켐"
};
const std::vector<std::string> INSPECTORS = {
    "김지수", "이민준", "박서연", "최준혁", "정다은",
    "한승우", "오수진", "임태양", "강예림", "윤재호"
};

} // namespace ref

// ════════════════════════════════════════════════════
// 생성자
// ════════════════════════════════════════════════════
SemiDataGenerator::SemiDataGenerator(unsigned seed) : rng_(seed) {}

// ════════════════════════════════════════════════════
// 유틸
// ════════════════════════════════════════════════════
int    SemiDataGenerator::randInt(int lo, int hi) {
    return std::uniform_int_distribution<int>(lo, hi)(rng_);
}
double SemiDataGenerator::randDouble(double lo, double hi) {
    return std::uniform_real_distribution<double>(lo, hi)(rng_);
}
bool   SemiDataGenerator::randBool(double trueProb) {
    return std::bernoulli_distribution(trueProb)(rng_);
}
template<typename T>
const T& SemiDataGenerator::pick(const std::vector<T>& v) {
    return v[randInt(0, static_cast<int>(v.size()) - 1)];
}

std::string SemiDataGenerator::fmtDate(int y, int m, int d) {
    std::ostringstream oss;
    oss << y << '-' << std::setw(2) << std::setfill('0') << m
              << '-' << std::setw(2) << std::setfill('0') << d;
    return oss.str();
}

std::string SemiDataGenerator::randomDate(int yearFrom, int yearTo) {
    int y = randInt(yearFrom, yearTo);
    int m = randInt(1, 12);
    int dMax = (m == 2) ? 28 : (m == 4 || m == 6 || m == 9 || m == 11) ? 30 : 31;
    int d = randInt(1, dMax);
    return fmtDate(y, m, d);
}

std::string SemiDataGenerator::offsetDate(const std::string& base, int days) {
    int y = std::stoi(base.substr(0, 4));
    int m = std::stoi(base.substr(5, 2));
    int d = std::stoi(base.substr(8, 2));
    // 단순 날짜 오프셋 (월 경계 처리 생략 - PoC 수준)
    d += days;
    while (d > 28) { d -= 28; m++; }
    while (m > 12) { m -= 12; y++; }
    return fmtDate(y, m, d);
}

std::string SemiDataGenerator::newId(const std::string& prefix) {
    std::ostringstream oss;
    oss << prefix << '-' << std::setw(6) << std::setfill('0') << idSeq_++;
    return oss.str();
}

// ════════════════════════════════════════════════════
// 고객사 생성
// ════════════════════════════════════════════════════
Customer SemiDataGenerator::makeCustomer(int idx) {
    int i = idx % static_cast<int>(ref::CUSTOMER_NAMES.size());
    Customer c;
    c.customerId    = newId("CUST");
    c.customerCode  = ref::CUSTOMER_CODES[i];
    c.customerName  = ref::CUSTOMER_NAMES[i];
    c.contactPerson = pick(ref::INSPECTORS); // 담당자 이름 재활용
    c.email         = ref::CUSTOMER_CODES[i] + ".rd@" +
                      (i % 2 == 0 ? "samsung.com" : "example.co.kr");
    c.phone         = "02-" + std::to_string(randInt(1000,9999))
                    + "-" + std::to_string(randInt(1000,9999));
    c.country       = ref::COUNTRIES[i];
    c.tier          = pick(ref::TIERS);
    return c;
}

// ════════════════════════════════════════════════════
// 제품 생성
// ════════════════════════════════════════════════════
Product SemiDataGenerator::makeProduct(int idx) {
    Product p;
    p.productId   = newId("PROD");
    p.productType = pick(ref::PRODUCT_TYPES);
    p.processNode = pick(ref::PROCESS_NODES);

    // 코드: TYPE-NODE-SEQ
    std::string typeAbbr = p.productType.substr(0, 3);
    p.productCode = typeAbbr + std::to_string(p.processNode) + "N-"
                  + std::to_string(1000 + idx);
    p.productName = p.productType + " " + std::to_string(p.processNode) + "nm 시료 #"
                  + std::to_string(idx + 1);

    // 웨이퍼 사양
    WaferSpec& ws = p.waferSpec;
    ws.material    = (p.productType == "Power" || p.productType == "RF")
                     ? pick(std::vector<std::string>{"SiC","GaN"})
                     : "Si";
    ws.diameter    = (p.processNode <= 14) ? 300 :
                     (p.processNode <= 65) ? 200 : 150;
    ws.thickness   = (ws.diameter == 300) ? randInt(750, 780) :
                     (ws.diameter == 200) ? randInt(700, 730) : randInt(650, 680);
    ws.resistivity = randDouble(0.001, 100.0);
    ws.orientation = pick(ref::ORIENTATIONS);
    ws.dopingType  = pick(ref::DOPING_TYPES);
    ws.dopant      = (ws.dopingType == "N-type") ? pick(ref::DOPANTS_N)
                                                  : pick(ref::DOPANTS_P);

    p.targetYield = randDouble(85.0, 98.0);
    p.unitPrice   = randDouble(500.0, 5000.0);
    return p;
}

// ════════════════════════════════════════════════════
// 생산 주문 생성
// ════════════════════════════════════════════════════
ProductionOrder SemiDataGenerator::makeOrder(
        int idx,
        const std::vector<Customer>& customers,
        const std::vector<Product>&  products)
{
    ProductionOrder o;
    o.orderId     = newId("ORD");
    int year = randInt(2025, 2026);
    int week = randInt(1, 52);
    std::ostringstream oss;
    oss << "PO-" << year << "-W" << std::setw(2) << std::setfill('0') << week
        << "-" << std::setw(4) << std::setfill('0') << (idx + 1);
    o.orderNumber = oss.str();

    o.customerId  = pick(customers).customerId;
    o.orderDate   = randomDate(2025, 2026);
    o.dueDate     = offsetDate(o.orderDate, randInt(30, 120));
    o.priority    = pick(ref::PRIORITIES);
    o.status      = pick(ref::ORDER_STATUSES);
    o.currency    = "KRW";

    // 주문 항목 1~3개
    int itemCount = randInt(1, 3);
    double total  = 0.0;
    for (int i = 0; i < itemCount; ++i) {
        const Product& prod = pick(products);
        OrderItem item;
        item.itemId       = newId("ITEM");
        item.productId    = prod.productId;
        item.quantity     = randInt(5, 100);
        item.unitPrice    = prod.unitPrice * randDouble(0.9, 1.1);
        item.requestedDeliveryDate = offsetDate(o.orderDate, randInt(14, 90));
        static const std::vector<std::string> reqs = {
            "ESD 포장 요청", "인증서(CoC) 첨부", "박스 단위 포장",
            "특수 세정 처리", "해당 없음", "추가 메트롤로지 데이터 요청"
        };
        item.specialRequirements = pick(reqs);
        total += item.quantity * item.unitPrice;
        o.items.push_back(std::move(item));
    }
    o.totalAmount = total;
    o.salesRepId  = newId("EMP");
    static const std::vector<std::string> notes = {
        "긴급 납기 요청 건", "고객 방문 시 샘플 전달 예정", "NDA 체결 완료",
        "특수 공정 조건 별도 협의 필요", ""
    };
    o.notes = pick(notes);
    return o;
}

// ════════════════════════════════════════════════════
// 설비 생성
// ════════════════════════════════════════════════════
Equipment SemiDataGenerator::makeEquipment(int idx) {
    Equipment e;
    e.equipmentId   = newId("EQP");
    e.equipmentType = pick(ref::EQUIP_TYPES);

    // 타입 약어 → 코드
    std::string abbr = e.equipmentType.substr(0, 3);
    std::transform(abbr.begin(), abbr.end(), abbr.begin(), ::toupper);
    e.equipmentCode = abbr + std::to_string(100 + idx);
    e.equipmentName = e.equipmentType + " #" + std::to_string(idx + 1);
    e.manufacturer  = pick(ref::MANUFACTURERS);

    std::ostringstream mdl;
    mdl << abbr << "-" << randInt(1000, 9999);
    e.model         = mdl.str();

    std::ostringstream loc;
    loc << "Bay-" << static_cast<char>('A' + randInt(0, 3))
        << " / Chamber-" << randInt(1, 6);
    e.location      = loc.str();
    e.status        = pick(ref::EQUIP_STATUSES);
    e.utilization   = randDouble(20.0, 95.0);
    e.lastMaintenanceDate = randomDate(2025, 2026);
    e.nextMaintenanceDate = offsetDate(e.lastMaintenanceDate, 90);
    return e;
}

// ════════════════════════════════════════════════════
// 공정 이력 생성
// ════════════════════════════════════════════════════
std::vector<ProcessStep> SemiDataGenerator::makeProcessFlow(
        const Product& prod,
        const std::vector<Equipment>& equip,
        const std::string& lotStartDate,
        bool completed)
{
    std::vector<ProcessStep> steps;
    // 공정 노드에 따라 단계 수 축소
    int totalSteps = static_cast<int>(ref::PROCESS_FLOW.size());
    int numSteps   = (prod.processNode <= 14) ? totalSteps :
                     (prod.processNode <= 65) ? totalSteps - 4 : totalSteps - 8;
    numSteps = std::max(numSteps, 8);

    std::string currentDate = lotStartDate;
    for (int i = 0; i < numSteps; ++i) {
        const auto& def = ref::PROCESS_FLOW[i];
        ProcessStep s;
        s.stepId        = newId("STEP");
        s.stepOrder     = i + 1;
        s.processName   = def.name;
        s.processType   = def.type;
        s.durationHours = randDouble(def.minHr, def.maxHr);
        s.temperature   = randDouble(def.minTemp, def.maxTemp);
        s.pressure      = randDouble(def.minPres, def.maxPres);

        // 관련 타입 설비 매핑 (없으면 랜덤)
        s.equipmentId = pick(equip).equipmentId;
        for (const auto& eq : equip) {
            if (eq.equipmentType.find(def.type.substr(0, 3)) != std::string::npos) {
                s.equipmentId = eq.equipmentId;
                break;
            }
        }

        if (completed) {
            s.status      = "Completed";
            s.startedAt   = currentDate + "T" + std::to_string(randInt(7,18)) + ":00:00";
            currentDate   = offsetDate(currentDate, 1);
            s.completedAt = currentDate + "T" + std::to_string(randInt(7,18)) + ":30:00";
        } else if (i < numSteps / 2) {
            s.status      = "Completed";
            s.startedAt   = currentDate + "T08:00:00";
            currentDate   = offsetDate(currentDate, 1);
            s.completedAt = currentDate + "T16:30:00";
        } else if (i == numSteps / 2) {
            s.status    = "InProgress";
            s.startedAt = currentDate + "T09:00:00";
            s.completedAt = "";
        } else {
            s.status    = "Pending";
            s.startedAt = "";
            s.completedAt = "";
        }
        steps.push_back(std::move(s));
    }
    return steps;
}

// ════════════════════════════════════════════════════
// 품질 검사 생성
// ════════════════════════════════════════════════════
std::vector<QualityInspection> SemiDataGenerator::makeInspections(
        const std::string& lotId,
        const Product& prod,
        bool lotPass)
{
    struct MeasDef { std::string param; double lo; double hi; double spread; std::string unit; };
    std::vector<MeasDef> mdefs = {
        {"CD (Critical Dimension)",   (double)prod.processNode * 0.9,
                                      (double)prod.processNode * 1.1,  3.0, "nm"},
        {"Overlay",                    0.0,   5.0,  1.5, "nm"},
        {"Film Thickness",           100.0, 120.0,  5.0, "Å"},
        {"Sheet Resistance",           4.5,   5.5,  0.2, "Ω/sq"},
        {"Particle Count (≥0.2μm)",    0.0,  15.0,  5.0, "ea/wafer"},
    };

    std::vector<std::string> inspTypes = {"Incoming", "In-line", "Final"};
    std::vector<QualityInspection> result;

    for (const auto& itype : inspTypes) {
        QualityInspection insp;
        insp.inspectionId  = newId("INSP");
        insp.lotId         = lotId;
        insp.inspectionType= itype;
        insp.inspectedAt   = randomDate(2025, 2026) + "T" +
                             std::to_string(randInt(8,17)) + ":00:00";
        int inspIdx        = randInt(0, (int)ref::INSPECTORS.size() - 1);
        insp.inspectorId   = "EMP-" + std::to_string(100 + inspIdx);
        insp.inspectorName = ref::INSPECTORS[inspIdx];

        bool inspPass = lotPass ? randBool(0.9) : randBool(0.3);
        int failCount = 0;

        for (const auto& md : mdefs) {
            QualityMeasurement meas;
            meas.parameter  = md.param;
            meas.lowerSpec  = md.lo;
            meas.upperSpec  = md.hi;
            meas.unit       = md.unit;
            double range    = md.hi - md.lo;
            double margin   = range * 0.1 + 0.5; // 스펙을 벗어난 범위 (최소 0.5)
            if (!inspPass && randBool(0.4)) {
                // 스펙 상한 초과 값
                meas.value  = md.hi + randDouble(margin * 0.5, margin * 2.0);
                meas.isPass = false;
                ++failCount;
            } else {
                double halfRange = range * 0.4;
                double center    = (md.lo + md.hi) / 2.0;
                meas.value  = center + randDouble(-halfRange, halfRange);
                meas.isPass = (meas.value >= meas.lowerSpec && meas.value <= meas.upperSpec);
                if (!meas.isPass) ++failCount;
            }
            insp.measurements.push_back(meas);
        }

        insp.result      = (failCount == 0) ? "Pass" :
                           (failCount <= 1)  ? "Marginal" : "Fail";
        insp.defectCount = randInt(0, inspPass ? 5 : 30);
        insp.yield       = inspPass ? randDouble(90.0, 99.5) : randDouble(40.0, 85.0);
        insp.disposition = (insp.result == "Pass")     ? "Released" :
                           (insp.result == "Marginal") ? "MRB"      : "Scrapped";
        result.push_back(std::move(insp));
    }
    return result;
}

// ════════════════════════════════════════════════════
// 로트 생성
// ════════════════════════════════════════════════════
Lot SemiDataGenerator::makeLot(
        int idx,
        const ProductionOrder& order,
        const OrderItem&       item,
        const std::vector<Equipment>& equip)
{
    Lot lot;
    lot.lotId       = newId("LOT");

    // 로트 번호: LOT-YEAR-Www-NNNN
    std::ostringstream ln;
    ln << "LOT-" << order.orderDate.substr(0,4)
       << "-W" << std::setw(2) << std::setfill('0') << randInt(1,52)
       << "-" << std::setw(4) << std::setfill('0') << (idx+1);
    lot.lotNumber       = ln.str();
    lot.orderId         = order.orderId;
    lot.orderItemId     = item.itemId;
    lot.productId       = item.productId;
    lot.waferCount      = randInt(5, 25);
    lot.status          = pick(ref::LOT_STATUSES);
    lot.startDate       = order.orderDate;

    bool completed = (lot.status == "Completed" || lot.status == "Scrapped");
    lot.completionDate  = completed ? offsetDate(lot.startDate, randInt(20, 60)) : "";

    // 수율 계산
    bool lotPass        = (lot.status != "Scrapped");
    lot.scrapWafers     = lotPass ? randInt(0, 2) : randInt(3, lot.waferCount);
    lot.completedWafers = lot.waferCount - lot.scrapWafers;
    lot.actualYield     = 100.0 * lot.completedWafers / lot.waferCount;

    // 제품 조회용 더미 (processNode 불필요 → 더미 Product 생성)
    Product dummyProd;
    dummyProd.processNode = 28;
    dummyProd.productType = "Logic";

    lot.processHistory  = makeProcessFlow(dummyProd, equip, lot.startDate, completed);
    if (!lot.processHistory.empty()) {
        // currentProcessStep = 현재 진행 중인 단계 이름
        for (const auto& s : lot.processHistory) {
            if (s.status == "InProgress") { lot.currentProcessStep = s.processName; break; }
        }
        if (lot.currentProcessStep.empty())
            lot.currentProcessStep = completed ? "완료" : lot.processHistory.front().processName;
    }

    lot.inspections = makeInspections(lot.lotId, dummyProd, lotPass);
    return lot;
}

// ════════════════════════════════════════════════════
// 원자재 생성
// ════════════════════════════════════════════════════
Material SemiDataGenerator::makeMaterial(int idx) {
    Material m;
    m.materialId   = newId("MAT");
    m.materialType = pick(ref::MATERIAL_TYPES);
    m.supplier     = pick(ref::SUPPLIERS);

    if      (m.materialType == "Wafer")      m.materialName = pick(ref::MATERIAL_NAMES_WAFER);
    else if (m.materialType == "Chemical")   m.materialName = pick(ref::MATERIAL_NAMES_CHEM);
    else if (m.materialType == "Gas")        m.materialName = pick(ref::MATERIAL_NAMES_GAS);
    else if (m.materialType == "Photoresist")m.materialName = pick(ref::MATERIAL_NAMES_PR);
    else                                     m.materialName = m.materialType + " #" + std::to_string(idx+1);

    std::ostringstream code;
    code << m.materialType.substr(0,3) << "-" << std::setw(5) << std::setfill('0') << (idx+1);
    m.materialCode = code.str();

    std::ostringstream sln;
    sln << m.supplier.substr(0,3) << "-" << randInt(100000, 999999);
    m.supplierLotNumber = sln.str();

    m.quantity     = randDouble(10.0, 500.0);
    m.unit         = (m.materialType == "Wafer") ? "매" :
                     (m.materialType == "Gas")   ? "kg" :
                     (m.materialType == "Chemical") ? "L" : "kg";
    m.receivedDate = randomDate(2025, 2026);
    m.expiryDate   = offsetDate(m.receivedDate, randInt(90, 365));
    static const std::vector<std::string> matStat = {
        "Available", "Available", "InUse", "Consumed", "Quarantine", "Rejected"
    };
    m.status   = pick(matStat);
    m.unitCost = randDouble(50.0, 5000.0);
    m.currency = "KRW";
    return m;
}

// ════════════════════════════════════════════════════
// 전체 데이터셋 생성
// ════════════════════════════════════════════════════
SemiDataset SemiDataGenerator::generate(
        int numCustomers, int numProducts,
        int numOrders,    int numEquipment,
        int numMaterials)
{
    SemiDataset ds;

    for (int i = 0; i < numCustomers; ++i)
        ds.customers.push_back(makeCustomer(i));

    for (int i = 0; i < numProducts; ++i)
        ds.products.push_back(makeProduct(i));

    for (int i = 0; i < numEquipment; ++i)
        ds.equipment.push_back(makeEquipment(i));

    for (int i = 0; i < numOrders; ++i)
        ds.orders.push_back(makeOrder(i, ds.customers, ds.products));

    // 각 주문 항목당 1~2개 로트 생성
    int lotIdx = 0;
    for (const auto& order : ds.orders) {
        for (const auto& item : order.items) {
            int lotsForItem = randInt(1, 2);
            for (int l = 0; l < lotsForItem; ++l) {
                ds.lots.push_back(makeLot(lotIdx++, order, item, ds.equipment));
            }
        }
    }

    for (int i = 0; i < numMaterials; ++i)
        ds.materials.push_back(makeMaterial(i));

    return ds;
}
