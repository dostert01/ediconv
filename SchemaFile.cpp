#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <ranges>

#include "SchemaFile.h"

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

} // edi