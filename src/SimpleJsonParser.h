#pragma once
#include <string>
#include <vector>

// 외부 라이브러리 없이 구현한 경량 JSON 파서.
// 도트 표기법으로 중첩 값 조회 (예: "templates.customers", "ranges.quantity_min").
// 제약: 동일 키가 문자열 값 안에 중복 등장하는 특수 JSON은 미지원 (PoC 범위).
class SimpleJsonParser {
public:
    bool load(const std::string& filePath);

    std::vector<std::string> getStringArray(const std::string& dotKey) const;
    double getNumber(const std::string& dotKey, double fallback = 0.0) const;
    int    getInt   (const std::string& dotKey, int    fallback = 0)   const;

private:
    std::string content_;

    std::string findRawValue(const std::string& dotKey) const;
    static std::string extractValueForKey(const std::string& json, const std::string& key);
    static std::string extractBlock(const std::string& text, size_t pos);
};
