#include "SimpleJsonParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

// ── 파일 로드 ────────────────────────────────────────
bool SimpleJsonParser::load(const std::string& filePath) {
    std::ifstream ifs(filePath);
    if (!ifs) return false;
    std::ostringstream oss;
    oss << ifs.rdbuf();
    content_ = oss.str();
    return !content_.empty();
}

// ── 균형 블록 추출 ({...} 또는 [...]) ────────────────
std::string SimpleJsonParser::extractBlock(const std::string& text, size_t pos) {
    if (pos >= text.size()) return {};
    char open  = text[pos];
    char close = (open == '{') ? '}' : ']';
    int  depth = 0;
    bool inStr = false;

    for (size_t i = pos; i < text.size(); ++i) {
        char c = text[i];
        if (c == '\\' && inStr) { ++i; continue; }
        if (c == '"')  { inStr = !inStr; continue; }
        if (inStr)     continue;
        if (c == open)  ++depth;
        else if (c == close) {
            if (--depth == 0)
                return text.substr(pos, i - pos + 1);
        }
    }
    return {};
}

// ── JSON 오브젝트에서 특정 키의 raw 값 추출 ──────────
std::string SimpleJsonParser::extractValueForKey(const std::string& json,
                                                 const std::string& key) {
    const std::string needle = "\"" + key + "\"";
    size_t pos = 0;

    while (pos < json.size()) {
        size_t found = json.find(needle, pos);
        if (found == std::string::npos) return {};

        // 콜론 확인 (값 키인지 검증)
        size_t after = found + needle.size();
        while (after < json.size() && (json[after] == ' ' || json[after] == '\t'))
            ++after;
        if (after >= json.size() || json[after] != ':') { pos = found + 1; continue; }

        // 값 시작 위치
        size_t vs = after + 1;
        while (vs < json.size() &&
               (json[vs] == ' ' || json[vs] == '\t' || json[vs] == '\n' || json[vs] == '\r'))
            ++vs;
        if (vs >= json.size()) return {};

        char ch = json[vs];
        if (ch == '{' || ch == '[') return extractBlock(json, vs);

        if (ch == '"') {
            size_t end = vs + 1;
            while (end < json.size()) {
                if (json[end] == '\\') { end += 2; continue; }
                if (json[end] == '"')  { ++end; break; }
                ++end;
            }
            return json.substr(vs, end - vs);
        }

        // 숫자 / true / false / null
        size_t end = vs;
        while (end < json.size() &&
               json[end] != ',' && json[end] != '}' && json[end] != ']' &&
               json[end] != '\n' && json[end] != '\r')
            ++end;
        while (end > vs && (json[end - 1] == ' ' || json[end - 1] == '\t'))
            --end;
        return json.substr(vs, end - vs);
    }
    return {};
}

// ── 도트 표기법 경로로 raw 값 조회 ───────────────────
std::string SimpleJsonParser::findRawValue(const std::string& dotKey) const {
    std::string context = content_;
    std::istringstream ss(dotKey);
    std::string part;
    while (std::getline(ss, part, '.')) {
        context = extractValueForKey(context, part);
        if (context.empty()) return {};
    }
    return context;
}

// ── 공개 API ─────────────────────────────────────────
std::vector<std::string> SimpleJsonParser::getStringArray(const std::string& dotKey) const {
    std::string raw = findRawValue(dotKey);
    if (raw.empty() || raw.front() != '[') return {};

    std::vector<std::string> result;
    size_t pos = 1; // '[' 건너뜀
    while (pos < raw.size()) {
        // 구분자·공백 건너뜀
        while (pos < raw.size() &&
               (raw[pos] == ' ' || raw[pos] == '\t' || raw[pos] == '\n' ||
                raw[pos] == '\r' || raw[pos] == ','))
            ++pos;
        if (pos >= raw.size() || raw[pos] == ']') break;

        if (raw[pos] == '"') {
            size_t end = pos + 1;
            while (end < raw.size()) {
                if (raw[end] == '\\') { end += 2; continue; }
                if (raw[end] == '"')  { ++end;    break;    }
                ++end;
            }
            result.push_back(raw.substr(pos + 1, end - pos - 2));
            pos = end;
        } else {
            ++pos;
        }
    }
    return result;
}

double SimpleJsonParser::getNumber(const std::string& dotKey, double fallback) const {
    std::string raw = findRawValue(dotKey);
    if (raw.empty()) return fallback;
    try { return std::stod(raw); }
    catch (...) { return fallback; }
}

int SimpleJsonParser::getInt(const std::string& dotKey, int fallback) const {
    return static_cast<int>(getNumber(dotKey, static_cast<double>(fallback)));
}
