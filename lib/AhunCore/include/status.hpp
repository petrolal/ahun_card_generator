#ifndef STATUS_HPP
#define STATUS_HPP

namespace ahun {

enum class Status {
    OK = 0,
    ERROR_GENERAL,
    ERROR_SD_CARD,
    ERROR_FILE_NOT_FOUND,
    ERROR_OUT_OF_MEMORY,
    ERROR_XML_PARSE,
    ERROR_JSON_PARSE,
    ERROR_RENDERING
};

} // namespace ahun

#endif // STATUS_HPP
