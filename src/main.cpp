#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "DummyDataGenerator.h"

// ── 기본 출력 파일명: output/dummy_YYYYMMDD_HHMMSS.json ──
static std::string defaultOutputPath() {
    std::time_t t  = std::time(nullptr);
    std::tm*    tm = std::localtime(&t);
    std::ostringstream oss;
    oss << "output/dummy_"
        << (tm->tm_year + 1900)
        << std::setw(2) << std::setfill('0') << (tm->tm_mon + 1)
        << std::setw(2) << std::setfill('0') <<  tm->tm_mday
        << "_"
        << std::setw(2) << std::setfill('0') <<  tm->tm_hour
        << std::setw(2) << std::setfill('0') <<  tm->tm_min
        << std::setw(2) << std::setfill('0') <<  tm->tm_sec
        << ".json";
    return oss.str();
}

static void printUsage(const char* prog) {
    std::cout << "Usage: " << prog << " [options]\n"
              << "  -n <count>   Number of records to generate  (default: 10)\n"
              << "  -s <seed>    RNG seed                        (default: random)\n"
              << "  -o <path>    Output file path                (default: output/dummy_YYYYMMDD_HHMMSS.json)\n"
              << "  -t <path>    JSON template file path         (default: data/templates/production_order_template.json)\n"
              << "  -p           Print generated records to console\n"
              << "  -h           Show this help message\n";
}

// ── 화면 출력 ──────────────────────────────────────────────
static std::string fmtPrice(double v) {
    // 천단위 쉼표
    long long iv = static_cast<long long>(v);
    std::string s = std::to_string(iv);
    int insert = static_cast<int>(s.size()) - 3;
    while (insert > 0) { s.insert(insert, ","); insert -= 3; }
    return s;
}

static void printOrders(const std::vector<ProductionOrder>& orders) {
    const std::string bar(72, '=');
    const std::string dash(72, '-');

    std::cout << "\n" << bar << "\n"
              << " Generated Orders (" << orders.size() << " records)\n"
              << bar << "\n";

    for (size_t i = 0; i < orders.size(); ++i) {
        const auto& o = orders[i];

        std::cout << "\n [" << std::right << std::setw(3) << (i + 1) << "] "
                  << o.orderId << "\n"
                  << dash << "\n";

        std::cout << std::left
                  << "  Customer   : " << o.customerId << "\n"
                  << "  Sample     : " << o.sampleName
                  << "  (" << o.sampleType << ", " << o.nodeSizeNm << "nm, "
                  << o.waferSizeMm << "mm wafer)\n"
                  << "  Lot        : " << o.lotNumber << "\n"
                  << "  Process    : " << o.processStep
                  << "  [" << o.productionLine << " / " << o.equipmentId
                  << " / " << o.operatorId << "]\n"
                  << "  Material   : " << o.materialCode << "\n"
                  << "  Schedule   : " << o.orderDate << " -> " << o.dueDate << "\n";

        std::cout << "  Status     : " << std::setw(12) << o.status
                  << "  Priority: " << o.priority << "\n";

        std::cout << std::fixed << std::setprecision(2)
                  << "  Qty        : " << o.quantity
                  << "   Price: " << fmtPrice(o.unitPrice) << " KRW"
                  << "   Thickness: " << o.thicknessUm << " um\n"
                  << "  Defects    : " << o.defectCount
                  << "   Yield: " << o.yieldRate << "%\n";
    }

    std::cout << "\n" << bar << "\n"
              << " Total: " << orders.size() << " records\n"
              << bar << "\n\n";
}

// ── 진입점 ────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    int         count         = 10;
    unsigned    seed          = 0;   // 0 = random
    std::string outputPath    = defaultOutputPath();
    std::string tplPath       = "data/templates/production_order_template.json";
    bool        printToScreen = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if      (arg == "-n" && i + 1 < argc) { count         = std::stoi(argv[++i]); }
        else if (arg == "-s" && i + 1 < argc) { seed          = static_cast<unsigned>(std::stoul(argv[++i])); }
        else if (arg == "-o" && i + 1 < argc) { outputPath    = argv[++i]; }
        else if (arg == "-t" && i + 1 < argc) { tplPath       = argv[++i]; }
        else if (arg == "-p")                 { printToScreen  = true; }
        else if (arg == "-h") { printUsage(argv[0]); return 0; }
        else { std::cerr << "Unknown option: " << arg << "\n"; printUsage(argv[0]); return 1; }
    }

    if (count <= 0) { std::cerr << "Count must be 1 or greater.\n"; return 1; }

    std::cout << "[Semiconductor Sample Production Order - Dummy Data Generator]\n";

    DummyDataGenerator gen(seed);

    bool tplOk = false;
    try   { gen.loadTemplate(tplPath); tplOk = true;
            std::cout << "Template loaded: " << tplPath << "\n"; }
    catch (...) {}
    if (!tplOk) std::cout << "No template found -- using built-in defaults\n";

    std::cout << "Generating... (" << count << " records)\n";
    auto orders = gen.generate(count);

    if (printToScreen)
        printOrders(orders);

    try {
        gen.saveToFile(outputPath, orders);
        std::cout << "Saved: " << outputPath << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Save failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
