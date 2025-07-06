#pragma once
#include <string>
#include <optional>
#include <memory>
#include <vector>
#include "pugixml.hpp"
#include "SchemaFile.h"
#include "EdiFile.h"

namespace edi
{
    class Xml2EdiProcessor {
        public:
            Xml2EdiProcessor() = default;
            void process(std::shared_ptr<pugi::xml_document> ediXml, std::shared_ptr<SchemaFile>& schema, std::shared_ptr<EdiFile>& ediOutputFile);

        private:
            std::shared_ptr<pugi::xml_document> ediXmlFile;
            std::shared_ptr<SchemaFile> schemaFile;
            std::shared_ptr<EdiFile> ediFile;
            void initMemberVariables(std::shared_ptr<pugi::xml_document> &ediXml, std::shared_ptr<edi::SchemaFile> &schema, std::shared_ptr<EdiFile> ediOutputFile);
            void processSegmentsAndSegmentGroups(const pugi::xml_node& schemaNode, const pugi::xml_node& ediXmlNode);
            void processSegment(pugi::xml_node node, const pugi::xml_node& ediXmlNode);
            void processSegmentGroups(pugi::xml_node node, const pugi::xml_node& ediXmlNode);
            void processSegmentGroup(pugi::xml_node node, const pugi::xml_node& ediXmlNode);
            bool elementIsCompositeElement(pugi::xml_node &elementNode);
    };

} // edi
