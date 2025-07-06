#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <ranges>

#include "SchemaFile.h"
#include "common.h"

#define PATH_SEPARATOR std::string("/")

namespace edi {

    // ----------------------------------------------------------------------
    // SchemaFile
    // ----------------------------------------------------------------------
    SchemaFile::SchemaFile() {
        fileLoadingSucceeded = false;
    }

    void SchemaFile::loadFromFile(const std::string& fileName) {
        pugi::xml_parse_result result = schemaXml.load_file(fileName.c_str());
        fileLoadingSucceeded = result != -1;
    }

    const bool SchemaFile::fileLoaded() const {
        return fileLoadingSucceeded;
    }

    const std::optional<std::string> SchemaFile::getMessageType() const {
        return getRootNodeAttributeValue("type");
    }

    const std::optional<std::string> SchemaFile::getMessageVersion() const {
        return getRootNodeAttributeValue("version");
    }

    std::optional<pugi::xml_node> SchemaFile::getSchemaRootNode() const {
        std::optional<pugi::xml_node> returnValue = std::nullopt;
        pugi::xml_node rootNode = schemaXml.root().first_child();
        if(std::string(rootNode.name()) == std::string("EdifactMessage"))
            returnValue = rootNode;
        return returnValue;
    }

    const std::optional<std::string> SchemaFile::getRootNodeAttributeValue(std::string attributeName) const {
        std::optional<std::string> returnValue = std::nullopt;
        if(fileLoadingSucceeded && getSchemaRootNode().has_value()) {
            pugi::xml_node rootNode = getSchemaRootNode().value();
            if(rootNode.attribute(attributeName).hash_value())
                returnValue = rootNode.attribute(attributeName).as_string();
        }
        return returnValue;
    }

    std::string SchemaFile::getEdiPath(const std::string& xPath) {
        std::string returnValue = "";
        pugi::xpath_node node = schemaXml.select_node(xPath.c_str(), 0);
        if(node) {
            std::string fullPath = PATH_SEPARATOR + node.node().attribute("name").as_string();
            prependParentPath(node.node(), fullPath);
            returnValue = fullPath;
        }
        return returnValue;
    }

    void SchemaFile::prependParentPath(const pugi::xml_node& node, std::string& fullPath) {
        pugi::xml_node parentNode = node.parent();
        if(parentNode && !isSchemaRootNode(parentNode)) {
            if(!isCompositeElement(node))
                fullPath = PATH_SEPARATOR + parentNode.attribute("name").as_string() + fullPath;
            prependParentPath(parentNode, fullPath);
        } else {
            fullPath = PATH_SEPARATOR + getMessageType().value_or("") + fullPath;
            replaceWhitespaces(fullPath);
        }
    }

    bool SchemaFile::isCompositeElement(const pugi::xml_node& node) {
        return node.name() == std::string("Composite");
    }

    bool SchemaFile::isSchemaRootNode(const pugi::xml_node& node) {
        return node.name() == std::string("EdifactMessage");
    }

} // edi