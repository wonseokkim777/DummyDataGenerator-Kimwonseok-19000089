#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility>

// ── 원시값 인코딩 헬퍼 ──────────────────────────────
namespace Jv {

inline std::string escape(const std::string& s) {
    std::string r;
    r.reserve(s.size() + 4);
    for (unsigned char c : s) {
        switch (c) {
        case '"':  r += "\\\""; break;
        case '\\': r += "\\\\"; break;
        case '\n': r += "\\n";  break;
        case '\r': r += "\\r";  break;
        case '\t': r += "\\t";  break;
        default:   r += static_cast<char>(c); break;
        }
    }
    return r;
}

inline std::string str(const std::string& s) { return "\"" + escape(s) + "\""; }

inline std::string num(int v)    { return std::to_string(v); }
inline std::string num(double v, int prec = 2) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(prec) << v;
    return oss.str();
}

inline std::string boolean(bool v) { return v ? "true" : "false"; }

} // namespace Jv

// ── JSON 오브젝트 빌더 ──────────────────────────────
class JObj {
public:
    JObj& f(const std::string& key, const std::string& val) {
        fields_.push_back({key, val});
        return *this;
    }

    std::string build(int indent = 0) const {
        if (fields_.empty()) return "{}";
        std::string pad(indent * 2, ' ');
        std::string ipad((indent + 1) * 2, ' ');
        std::string r = "{\n";
        for (size_t i = 0; i < fields_.size(); ++i) {
            r += ipad + "\"" + fields_[i].first + "\": " + fields_[i].second;
            if (i + 1 < fields_.size()) r += ',';
            r += '\n';
        }
        r += pad + '}';
        return r;
    }

private:
    std::vector<std::pair<std::string, std::string>> fields_;
};

// ── JSON 배열 빌더 ──────────────────────────────────
class JArr {
public:
    JArr& add(const std::string& v) { items_.push_back(v); return *this; }
    bool  empty() const { return items_.empty(); }
    size_t size() const { return items_.size(); }

    std::string build(int indent = 0) const {
        if (items_.empty()) return "[]";
        std::string pad(indent * 2, ' ');
        std::string ipad((indent + 1) * 2, ' ');
        std::string r = "[\n";
        for (size_t i = 0; i < items_.size(); ++i) {
            r += ipad + items_[i];
            if (i + 1 < items_.size()) r += ',';
            r += '\n';
        }
        r += pad + ']';
        return r;
    }

private:
    std::vector<std::string> items_;
};
