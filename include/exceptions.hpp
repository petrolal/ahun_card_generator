#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

namespace ahun {

class AhunException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class ConfigException : public AhunException {
public:
    using AhunException::AhunException;
};

class TemplateException : public AhunException {
public:
    using AhunException::AhunException;
};

class RenderingException : public AhunException {
public:
    using AhunException::AhunException;
};

class CalendarException : public AhunException {
public:
    using AhunException::AhunException;
};

} // namespace ahun

#endif // EXCEPTIONS_HPP
