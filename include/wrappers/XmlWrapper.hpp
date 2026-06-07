#ifndef XML_WRAPPER_HPP
#define XML_WRAPPER_HPP

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <stdexcept>

namespace ahun {

class XmlEngine {
public:
    XmlEngine() {
        xmlInitParser();
    }
    ~XmlEngine() {
        xmlCleanupParser();
    }
    XmlEngine(const XmlEngine&) = delete;
    XmlEngine& operator=(const XmlEngine&) = delete;
};

class XmlDocument {
public:
    XmlDocument(const std::string& path) : doc_(xmlReadFile(path.c_str(), NULL, 0)) {
        if (!doc_) throw std::runtime_error("Failed to parse XML: " + path);
    }
    ~XmlDocument() {
        if (doc_) xmlFreeDoc(doc_);
    }

    xmlDocPtr get() const { return doc_; }
    xmlNodePtr getRootElement() const { return xmlDocGetRootElement(doc_); }

private:
    xmlDocPtr doc_;
    XmlDocument(const XmlDocument&) = delete;
    XmlDocument& operator=(const XmlDocument&) = delete;
};

} // namespace ahun

#endif // XML_WRAPPER_HPP
