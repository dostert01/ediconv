#pragma once
#include <string>
#include <optional>
#include <memory>
#include "pugixml.hpp"
#include "EdiFile.h"

namespace edi
{

    class SchemaFile {
        public:
            SchemaFile();
            void loadFromFile(const std::string& fileName);
            const bool fileLoaded() const;
            const std::optional<std::string> getMessageType() const;
            const std::optional<std::string> getMessageVersion() const;
            std::optional<pugi::xml_node> getSchemaRootNode() const;
        private:
            bool fileLoadingSucceeded;
            pugi::xml_document schemaXml;
            std::shared_ptr<pugi::xml_document> ediXlm;
            const std::optional<std::string> getRootNodeAttributeValue(std::string attributeName) const;
    };


} // edi
