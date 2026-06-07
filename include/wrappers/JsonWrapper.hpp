#ifndef JSON_WRAPPER_HPP
#define JSON_WRAPPER_HPP

#include <cjson/cJSON.h>
#include <memory>
#include <string>

namespace ahun {

struct cJSONDeleter {
    void operator()(cJSON* ptr) const {
        if (ptr) cJSON_Delete(ptr);
    }
};

using ScopedJson = std::unique_ptr<cJSON, cJSONDeleter>;

class JsonDocument {
public:
    static ScopedJson parse(const std::string& json_str) {
        return ScopedJson(cJSON_Parse(json_str.c_str()));
    }

    static ScopedJson createObject() {
        return ScopedJson(cJSON_CreateObject());
    }

    static ScopedJson createArray() {
        return ScopedJson(cJSON_CreateArray());
    }
};

} // namespace ahun

#endif // JSON_WRAPPER_HPP
