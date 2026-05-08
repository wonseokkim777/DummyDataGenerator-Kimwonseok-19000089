#pragma once
#include <string>
#include <vector>

// ────────────────────────────────────────────────
// 고객사
// ────────────────────────────────────────────────
struct Customer {
    std::string customerId;
    std::string customerCode;
    std::string customerName;
    std::string contactPerson;
    std::string email;
    std::string phone;
    std::string country;
    std::string tier;           // Gold / Silver / Bronze
};

// ────────────────────────────────────────────────
// 웨이퍼 사양
// ────────────────────────────────────────────────
struct WaferSpec {
    int    diameter;            // mm: 150, 200, 300
    std::string material;       // Si, GaAs, SiC, GaN
    int    thickness;           // μm
    double resistivity;         // Ω·cm
    std::string orientation;    // <100>, <110>, <111>
    std::string dopingType;     // N-type, P-type
    std::string dopant;         // Boron, Phosphorus, Arsenic
};

// ────────────────────────────────────────────────
// 제품(시료)
// ────────────────────────────────────────────────
struct Product {
    std::string productId;
    std::string productCode;
    std::string productName;
    std::string productType;    // DRAM, NAND, Logic, Power, Sensor, RF
    int    processNode;         // nm
    WaferSpec waferSpec;
    double targetYield;         // %
    double unitPrice;           // USD per wafer
};

// ────────────────────────────────────────────────
// 공정 단계 (이력)
// ────────────────────────────────────────────────
struct ProcessStep {
    std::string stepId;
    int    stepOrder;
    std::string processName;
    std::string processType;    // Thermal, Litho, Etch, Deposition, CMP, Metrology, Implant, Clean
    std::string equipmentId;
    double durationHours;
    double temperature;         // °C
    double pressure;            // Torr
    std::string status;         // Pending, InProgress, Completed, Skipped
    std::string startedAt;
    std::string completedAt;
};

// ────────────────────────────────────────────────
// 주문 항목
// ────────────────────────────────────────────────
struct OrderItem {
    std::string itemId;
    std::string productId;
    int    quantity;            // 웨이퍼 매수
    double unitPrice;
    std::string specialRequirements;
    std::string requestedDeliveryDate;
};

// ────────────────────────────────────────────────
// 생산 주문
// ────────────────────────────────────────────────
struct ProductionOrder {
    std::string orderId;
    std::string orderNumber;
    std::string customerId;
    std::string orderDate;
    std::string dueDate;
    std::string priority;       // Critical, High, Normal, Low
    std::string status;         // Draft, Confirmed, InProcess, Completed, Cancelled, OnHold
    std::vector<OrderItem> items;
    double totalAmount;
    std::string currency;
    std::string salesRepId;
    std::string notes;
};

// ────────────────────────────────────────────────
// 설비
// ────────────────────────────────────────────────
struct Equipment {
    std::string equipmentId;
    std::string equipmentCode;
    std::string equipmentName;
    std::string equipmentType;  // DiffusionFurnace, Stepper, Etcher, CVD, PVD, CMP, Implanter, Metrology
    std::string model;
    std::string manufacturer;
    std::string location;       // e.g. "Bay-A / Chamber-3"
    std::string status;         // Available, InUse, Maintenance, Down
    double utilization;         // %
    std::string lastMaintenanceDate;
    std::string nextMaintenanceDate;
};

// ────────────────────────────────────────────────
// 품질 측정값
// ────────────────────────────────────────────────
struct QualityMeasurement {
    std::string parameter;
    double value;
    double lowerSpec;
    double upperSpec;
    std::string unit;
    bool   isPass;
};

// ────────────────────────────────────────────────
// 품질 검사
// ────────────────────────────────────────────────
struct QualityInspection {
    std::string inspectionId;
    std::string lotId;
    std::string inspectionType; // Incoming, In-line, Final
    std::string inspectedAt;
    std::string inspectorId;
    std::string inspectorName;
    std::vector<QualityMeasurement> measurements;
    std::string result;         // Pass, Fail, Marginal
    int    defectCount;
    double yield;               // %
    std::string disposition;    // Released, Scrapped, MRB, Rework
};

// ────────────────────────────────────────────────
// 로트
// ────────────────────────────────────────────────
struct Lot {
    std::string lotId;
    std::string lotNumber;
    std::string orderId;
    std::string orderItemId;
    std::string productId;
    int    waferCount;
    std::string status;         // Queued, InProcess, Completed, Scrapped, OnHold
    std::string currentProcessStep;
    std::string startDate;
    std::string completionDate;
    int    completedWafers;
    int    scrapWafers;
    double actualYield;         // %
    std::vector<ProcessStep> processHistory;
    std::vector<QualityInspection> inspections;
};

// ────────────────────────────────────────────────
// 원자재
// ────────────────────────────────────────────────
struct Material {
    std::string materialId;
    std::string materialCode;
    std::string materialType;   // Wafer, Chemical, Gas, Target, Photoresist, CMP-Slurry
    std::string materialName;
    std::string supplier;
    std::string supplierLotNumber;
    double quantity;
    std::string unit;
    std::string receivedDate;
    std::string expiryDate;
    std::string status;         // Available, InUse, Consumed, Quarantine, Rejected
    double unitCost;
    std::string currency;
};

// ────────────────────────────────────────────────
// 전체 데이터셋
// ────────────────────────────────────────────────
struct SemiDataset {
    std::vector<Customer>        customers;
    std::vector<Product>         products;
    std::vector<ProductionOrder> orders;
    std::vector<Equipment>       equipment;
    std::vector<Lot>             lots;
    std::vector<Material>        materials;
};
