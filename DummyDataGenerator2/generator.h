#pragma once
#include <random>
#include <string>
#include "models.h"

class SemiDataGenerator {
public:
    explicit SemiDataGenerator(unsigned seed = 42);

    SemiDataset generate(int numCustomers, int numProducts,
                         int numOrders,   int numEquipment,
                         int numMaterials);

private:
    // ── 엔티티 생성 ──
    Customer        makeCustomer(int idx);
    Product         makeProduct(int idx);
    ProductionOrder makeOrder(int idx,
                              const std::vector<Customer>& customers,
                              const std::vector<Product>&  products);
    Equipment       makeEquipment(int idx);
    Lot             makeLot(int idx,
                            const ProductionOrder& order,
                            const OrderItem&       item,
                            const std::vector<Equipment>& equip);
    Material        makeMaterial(int idx);

    // ── 하위 요소 ──
    std::vector<ProcessStep>      makeProcessFlow(const Product& prod,
                                                  const std::vector<Equipment>& equip,
                                                  const std::string& lotStartDate,
                                                  bool completed);
    std::vector<QualityInspection> makeInspections(const std::string& lotId,
                                                   const Product& prod,
                                                   bool lotPass);

    // ── 날짜/ID 유틸 ──
    std::string randomDate(int yearFrom, int yearTo);
    std::string offsetDate(const std::string& base, int days);
    std::string newId(const std::string& prefix);
    std::string fmtDate(int y, int m, int d);

    // ── 선택 유틸 ──
    template<typename T>
    const T& pick(const std::vector<T>& v);
    int  randInt(int lo, int hi);
    double randDouble(double lo, double hi);
    bool   randBool(double trueProb = 0.5);

    std::mt19937 rng_;
    int          idSeq_ = 1;
};
