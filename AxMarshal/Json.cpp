#include "Json.h"

namespace AxMarshal {
namespace Json {
void __toJson(Str &res, const uint32_t &data) { res += std::to_string(data); }
void __toJson(Str &res, const uint64_t &data) { res += std::to_string(data); }
void __toJson(Str &res, const int32_t &data) { res += std::to_string(data); }
void __toJson(Str &res, const int64_t &data) { res += std::to_string(data); }

void __fromJson(Iter &it, uint32_t &data) {
    int len;
    sscanf(&*it, "%u%n", &data, &len);
    it += len;
}
void __fromJson(Iter &it, uint64_t &data) {
    int len;
    sscanf(&*it, "%lu%n", &data, &len);
    it += len;
}
void __fromJson(Iter &it, int32_t &data) {
    int len;
    sscanf(&*it, "%d%n", &data, &len);
    it += len;
}
void __fromJson(Iter &it, int64_t &data) {
    int len;
    sscanf(&*it, "%ld%n", &data, &len);
    it += len;
}
void __jsonIgnoreSpace(Iter &it) {
    while (isspace(*it)) {
        ++it;
    }
}
char __jsonGetFirstChar(Iter &it) {
    __jsonIgnoreSpace(it);
    char res = *it;
    ++it;
    return res;
}
std::string __jsonGetFirstKey(Iter &it) {
    if (__jsonGetFirstChar(it) == '}') {
        --it;
        return std::string();
    }
    auto l = it;
    while (*it != '"') {
        ++it;
    }
    auto r = it;
    __jsonGetFirstChar(it);
    __jsonGetFirstChar(it);
    return std::string(l, r);
}
}  // namespace Json
}  // namespace AxMarshal