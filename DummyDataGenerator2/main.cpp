#include <iostream>
#include <fstream>
#include <string>
#include "models.h"
#include "generator.h"
#include "json_builder.h"

// ════════════════════════════════════════════════════
// 직렬화 함수
// ════════════════════════════════════════════════════

static std::string serializeWaferSpec(const WaferSpec& ws) {
    return JObj()
        .f("diameter",    Jv::num(ws.diameter))
        .f("diameterUnit",Jv::str("mm"))
        .f("material",    Jv::str(ws.material))
        .f("thickness",   Jv::num(ws.thickness))
        .f("thicknessUnit",Jv::str("μm"))
        .f("resistivity", Jv::num(ws.resistivity, 4))
        .f("resistivityUnit",Jv::str("Ω·cm"))
        .f("orientation", Jv::str(ws.orientation))
        .f("dopingType",  Jv::str(ws.dopingType))
        .f("dopant",      Jv::str(ws.dopant))
        .build(2);
}

static std::string serializeCustomer(const Customer& c) {
    return JObj()
        .f("customerId",   Jv::str(c.customerId))
        .f("customerCode", Jv::str(c.customerCode))
        .f("customerName", Jv::str(c.customerName))
        .f("contactPerson",Jv::str(c.contactPerson))
        .f("email",        Jv::str(c.email))
        .f("phone",        Jv::str(c.phone))
        .f("country",      Jv::str(c.country))
        .f("tier",         Jv::str(c.tier))
        .build(2);
}

static std::string serializeProduct(const Product& p) {
    return JObj()
        .f("productId",    Jv::str(p.productId))
        .f("productCode",  Jv::str(p.productCode))
        .f("productName",  Jv::str(p.productName))
        .f("productType",  Jv::str(p.productType))
        .f("processNode",  Jv::num(p.processNode))
        .f("processNodeUnit",Jv::str("nm"))
        .f("waferSpec",    serializeWaferSpec(p.waferSpec))
        .f("targetYield",  Jv::num(p.targetYield))
        .f("targetYieldUnit",Jv::str("%"))
        .f("unitPrice",    Jv::num(p.unitPrice))
        .f("currency",     Jv::str("KRW"))
        .build(2);
}

static std::string serializeEquipment(const Equipment& e) {
    return JObj()
        .f("equipmentId",          Jv::str(e.equipmentId))
        .f("equipmentCode",        Jv::str(e.equipmentCode))
        .f("equipmentName",        Jv::str(e.equipmentName))
        .f("equipmentType",        Jv::str(e.equipmentType))
        .f("model",                Jv::str(e.model))
        .f("manufacturer",         Jv::str(e.manufacturer))
        .f("location",             Jv::str(e.location))
        .f("status",               Jv::str(e.status))
        .f("utilization",          Jv::num(e.utilization))
        .f("utilizationUnit",      Jv::str("%"))
        .f("lastMaintenanceDate",  Jv::str(e.lastMaintenanceDate))
        .f("nextMaintenanceDate",  Jv::str(e.nextMaintenanceDate))
        .build(2);
}

static std::string serializeMaterial(const Material& m) {
    return JObj()
        .f("materialId",         Jv::str(m.materialId))
        .f("materialCode",       Jv::str(m.materialCode))
        .f("materialType",       Jv::str(m.materialType))
        .f("materialName",       Jv::str(m.materialName))
        .f("supplier",           Jv::str(m.supplier))
        .f("supplierLotNumber",  Jv::str(m.supplierLotNumber))
        .f("quantity",           Jv::num(m.quantity))
        .f("unit",               Jv::str(m.unit))
        .f("receivedDate",       Jv::str(m.receivedDate))
        .f("expiryDate",         Jv::str(m.expiryDate))
        .f("status",             Jv::str(m.status))
        .f("unitCost",           Jv::num(m.unitCost))
        .f("currency",           Jv::str(m.currency))
        .build(2);
}

static std::string serializeOrderItem(const OrderItem& item) {
    return JObj()
        .f("itemId",                  Jv::str(item.itemId))
        .f("productId",               Jv::str(item.productId))
        .f("quantity",                Jv::num(item.quantity))
        .f("unitPrice",               Jv::num(item.unitPrice))
        .f("specialRequirements",     Jv::str(item.specialRequirements))
        .f("requestedDeliveryDate",   Jv::str(item.requestedDeliveryDate))
        .build(3);
}

static std::string serializeOrder(const ProductionOrder& o) {
    JArr itemArr;
    for (const auto& item : o.items)
        itemArr.add(serializeOrderItem(item));

    return JObj()
        .f("orderId",      Jv::str(o.orderId))
        .f("orderNumber",  Jv::str(o.orderNumber))
        .f("customerId",   Jv::str(o.customerId))
        .f("orderDate",    Jv::str(o.orderDate))
        .f("dueDate",      Jv::str(o.dueDate))
        .f("priority",     Jv::str(o.priority))
        .f("status",       Jv::str(o.status))
        .f("items",        itemArr.build(2))
        .f("totalAmount",  Jv::num(o.totalAmount, 0))
        .f("currency",     Jv::str(o.currency))
        .f("salesRepId",   Jv::str(o.salesRepId))
        .f("notes",        Jv::str(o.notes))
        .build(2);
}

static std::string serializeProcessStep(const ProcessStep& s) {
    return JObj()
        .f("stepId",        Jv::str(s.stepId))
        .f("stepOrder",     Jv::num(s.stepOrder))
        .f("processName",   Jv::str(s.processName))
        .f("processType",   Jv::str(s.processType))
        .f("equipmentId",   Jv::str(s.equipmentId))
        .f("durationHours", Jv::num(s.durationHours))
        .f("temperature",   Jv::num(s.temperature))
        .f("temperatureUnit",Jv::str("°C"))
        .f("pressure",      Jv::num(s.pressure, 6))
        .f("pressureUnit",  Jv::str("Torr"))
        .f("status",        Jv::str(s.status))
        .f("startedAt",     s.startedAt.empty()   ? "null" : Jv::str(s.startedAt))
        .f("completedAt",   s.completedAt.empty() ? "null" : Jv::str(s.completedAt))
        .build(4);
}

static std::string serializeMeasurement(const QualityMeasurement& meas) {
    return JObj()
        .f("parameter",  Jv::str(meas.parameter))
        .f("value",      Jv::num(meas.value, 4))
        .f("lowerSpec",  Jv::num(meas.lowerSpec, 4))
        .f("upperSpec",  Jv::num(meas.upperSpec, 4))
        .f("unit",       Jv::str(meas.unit))
        .f("isPass",     Jv::boolean(meas.isPass))
        .build(5);
}

static std::string serializeInspection(const QualityInspection& insp) {
    JArr measArr;
    for (const auto& m : insp.measurements)
        measArr.add(serializeMeasurement(m));

    return JObj()
        .f("inspectionId",   Jv::str(insp.inspectionId))
        .f("lotId",          Jv::str(insp.lotId))
        .f("inspectionType", Jv::str(insp.inspectionType))
        .f("inspectedAt",    Jv::str(insp.inspectedAt))
        .f("inspectorId",    Jv::str(insp.inspectorId))
        .f("inspectorName",  Jv::str(insp.inspectorName))
        .f("measurements",   measArr.build(4))
        .f("result",         Jv::str(insp.result))
        .f("defectCount",    Jv::num(insp.defectCount))
        .f("yield",          Jv::num(insp.yield))
        .f("yieldUnit",      Jv::str("%"))
        .f("disposition",    Jv::str(insp.disposition))
        .build(3);
}

static std::string serializeLot(const Lot& lot) {
    JArr stepArr;
    for (const auto& s : lot.processHistory)
        stepArr.add(serializeProcessStep(s));

    JArr inspArr;
    for (const auto& insp : lot.inspections)
        inspArr.add(serializeInspection(insp));

    return JObj()
        .f("lotId",              Jv::str(lot.lotId))
        .f("lotNumber",          Jv::str(lot.lotNumber))
        .f("orderId",            Jv::str(lot.orderId))
        .f("orderItemId",        Jv::str(lot.orderItemId))
        .f("productId",          Jv::str(lot.productId))
        .f("waferCount",         Jv::num(lot.waferCount))
        .f("status",             Jv::str(lot.status))
        .f("currentProcessStep", Jv::str(lot.currentProcessStep))
        .f("startDate",          Jv::str(lot.startDate))
        .f("completionDate",     lot.completionDate.empty() ? "null" : Jv::str(lot.completionDate))
        .f("completedWafers",    Jv::num(lot.completedWafers))
        .f("scrapWafers",        Jv::num(lot.scrapWafers))
        .f("actualYield",        Jv::num(lot.actualYield))
        .f("actualYieldUnit",    Jv::str("%"))
        .f("processHistory",     stepArr.build(2))
        .f("inspections",        inspArr.build(2))
        .build(2);
}

// ════════════════════════════════════════════════════
// 전체 데이터셋 → JSON 문자열
// ════════════════════════════════════════════════════
static std::string datasetToJson(const SemiDataset& ds) {
    JArr custArr, prodArr, orderArr, equipArr, lotArr, matArr;

    for (const auto& c : ds.customers)  custArr.add(serializeCustomer(c));
    for (const auto& p : ds.products)   prodArr.add(serializeProduct(p));
    for (const auto& o : ds.orders)     orderArr.add(serializeOrder(o));
    for (const auto& e : ds.equipment)  equipArr.add(serializeEquipment(e));
    for (const auto& l : ds.lots)       lotArr.add(serializeLot(l));
    for (const auto& m : ds.materials)  matArr.add(serializeMaterial(m));

    return JObj()
        .f("domain",       Jv::str("Semiconductor Sample Production Order Management"))
        .f("generatedAt",  Jv::str("2026-05-08T00:00:00"))
        .f("version",      Jv::str("1.0.0"))
        .f("summary", JObj()
            .f("customerCount",  Jv::num((int)ds.customers.size()))
            .f("productCount",   Jv::num((int)ds.products.size()))
            .f("orderCount",     Jv::num((int)ds.orders.size()))
            .f("equipmentCount", Jv::num((int)ds.equipment.size()))
            .f("lotCount",       Jv::num((int)ds.lots.size()))
            .f("materialCount",  Jv::num((int)ds.materials.size()))
            .build(1))
        .f("customers",  custArr.build(1))
        .f("products",   prodArr.build(1))
        .f("orders",     orderArr.build(1))
        .f("equipment",  equipArr.build(1))
        .f("lots",       lotArr.build(1))
        .f("materials",  matArr.build(1))
        .build(0);
}

// ════════════════════════════════════════════════════
// 엔트리 포인트
// ════════════════════════════════════════════════════
int main() {
    std::cout << "[반도체 시료 생산주문 더미 데이터 생성기]\n";
    std::cout << "Generating data...\n";

    SemiDataGenerator gen(42); // 시드 고정 → 재현 가능
    SemiDataset ds = gen.generate(
        /*customers*/  5,
        /*products*/  12,
        /*orders*/    20,
        /*equipment*/ 15,
        /*materials*/ 30
    );

    std::cout << "  Customers : " << ds.customers.size() << "\n";
    std::cout << "  Products  : " << ds.products.size()  << "\n";
    std::cout << "  Orders    : " << ds.orders.size()    << "\n";
    std::cout << "  Equipment : " << ds.equipment.size() << "\n";
    std::cout << "  Lots      : " << ds.lots.size()      << "\n";
    std::cout << "  Materials : " << ds.materials.size() << "\n";

    std::string json = datasetToJson(ds);

    const std::string outFile = "semiconductor_sample_orders.json";
    std::ofstream ofs(outFile, std::ios::out | std::ios::trunc);
    if (!ofs) {
        std::cerr << "ERROR: cannot open output file: " << outFile << "\n";
        return 1;
    }
    ofs << json;
    ofs.close();

    std::cout << "\nOutput written to: " << outFile << "\n";
    std::cout << "File size: " << json.size() << " bytes\n";
    return 0;
}
