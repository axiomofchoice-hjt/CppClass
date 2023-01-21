#include "Json.h"

namespace CppClass {
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
std::string __jsonEnumGet(Iter &it) {
    while (*it != '.') {
        ++it;
    }
    ++it;
    auto l = it;
    while (*it != '"') {
        ++it;
    }
    auto r = it;
    it += 2;
    return std::string(l, r);
}
}  // namespace Json
}  // namespace CppClass