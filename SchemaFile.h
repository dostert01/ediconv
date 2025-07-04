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
            void process(EdiFile& edi, std::shared_ptr<pugi::xml_document> targetXml);
        private:
            bool fileLoadingSucceeded;
            pugi::xml_document schemaXml;
            std::shared_ptr<pugi::xml_document> ediXlm;
            const std::optional<std::string> getRootNodeAttributeValue(std::string attributeName) const;
            std::optional<pugi::xml_node> getSchemaRootNode() const;
            void processAllChildSegments(pugi::xml_node &currentParentNode, EdiFile &edi, pugi::xml_node currentEdiParentNode);
            void processSegmentGroup(pugi::xml_node &segmentNode, edi::EdiFile &edi, const pugi::xml_node &currentEdiParentNode);
            void processDataSegment(pugi::xml_node &segmentNode, edi::EdiFile &edi);
            void processAllMatchingEdiSegments(std::optional<edi::Segment> &currentEdiSegment, std::string &schemaSegmentName, int maxOccurs, pugi::xml_node &segmentNode, edi::EdiFile &edi);
            void findMatchingDataSegmentInEdi(std::optional<edi::Segment> &currentEdiSegment, std::string &schemaSegmentName, edi::EdiFile &edi);
            void processElements(pugi::xml_node segmentNode, EdiFile& edi);
            void processCompositeElement(pugi::xml_node segmentNode, EdiFile& edi, const int elementIndex);
            bool ediSegmentMatchesSchemaSegment(std::optional<Segment>& currentEdiSegment, const std::string& schemaSegmentName);
    };


} // edi
