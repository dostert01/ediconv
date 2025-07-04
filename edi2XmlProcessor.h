#pragma once
#include <string>
#include <optional>
#include <memory>
#include "pugixml.hpp"
#include "EdiFile.h"
#include "SchemaFile.h"

namespace edi
{

    class Edi2XmlProcessor {
        public:
            Edi2XmlProcessor() = default;
            void process(std::shared_ptr<EdiFile> edi, std::shared_ptr<SchemaFile> schema,  std::shared_ptr<pugi::xml_document> targetXml);
        private:
            std::shared_ptr<EdiFile> ediFile;
            std::shared_ptr<SchemaFile> schemaFile;
            std::shared_ptr<pugi::xml_document> ediXlm;
            const std::optional<std::string> getRootNodeAttributeValue(std::string attributeName) const;
            std::optional<pugi::xml_node> getSchemaRootNode() const;
            void processAllChildSegments(pugi::xml_node &currentParentNode, pugi::xml_node currentEdiParentNode);
            void processSegmentGroup(pugi::xml_node &segmentNode, pugi::xml_node &currentEdiParentNode);
            void processDataSegment(pugi::xml_node &segmentNode, pugi::xml_node currentEdiParentNode);
            void processAllMatchingEdiSegments(std::optional<edi::Segment> &currentEdiSegment, std::string &schemaSegmentName, int maxOccurs, pugi::xml_node &segmentNode, pugi::xml_node currentEdiParentNode);
            void findMatchingDataSegmentInEdi(std::optional<edi::Segment> &currentEdiSegment, std::string &schemaSegmentName);
            void processElements(pugi::xml_node segmentNode, pugi::xml_node currentEdiParentNode);
            void addAsQualifierToParentNode(pugi::xml_node &elementNode, pugi::xml_node &currentEdiParentNode, std::string &elementName, std::string &value);
            pugi::xml_node appendPCDataNode(pugi::xml_node &currentEdiParentNode, std::string &elementName, std::string &value);
            pugi::xml_node appendNode(pugi::xml_node &currentEdiParentNode, std::string &elementName);
            void appendAttribute(pugi::xml_node &currentEdiParentNode, std::string &attributeName, std::string &value);
            void processCompositeElement(pugi::xml_node segmentNode, const int elementIndex, pugi::xml_node currentEdiParentNode);
            bool ediSegmentMatchesSchemaSegment(std::optional<Segment>& currentEdiSegment, const std::string& schemaSegmentName);
            std::string replaceWhitespaces(std::string& s, std::string& r);
            std::string replaceWhitespaces(std::string& s);
    };


} // edi
