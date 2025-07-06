#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <ranges>

#include "xml2EdiProcessor.h"
#include "common.h"

namespace edi {

    // ----------------------------------------------------------------------
    // Xml2EdiProcessor
    // ----------------------------------------------------------------------
    void Xml2EdiProcessor::process(std::shared_ptr<pugi::xml_document> ediXml, std::shared_ptr<SchemaFile>& schema, std::shared_ptr<EdiFile>& ediOutputFile) {
        initMemberVariables(ediXml, schema, ediOutputFile);
        if(schema->getSchemaRootNode()->hash_value()){
            pugi::xml_node schemaRootNode = schema->getSchemaRootNode().value();
            pugi::xml_node ediXmlRootNode = ediXml->root().first_child();
            processSegmentsAndSegmentGroups(schemaRootNode, ediXmlRootNode);
        }
    }

    void Xml2EdiProcessor::initMemberVariables(std::shared_ptr<pugi::xml_document> &ediXml, std::shared_ptr<edi::SchemaFile> &schema, std::shared_ptr<EdiFile> ediOutputFile)
    {
        ediXmlFile = ediXml;
        schemaFile = schema;
        ediFile = ediOutputFile;
    }

    void Xml2EdiProcessor::processSegmentsAndSegmentGroups(const pugi::xml_node& schemaNode, const pugi::xml_node& ediXmlNode) {
        for(pugi::xml_node currentSegmentOrGroup: schemaNode.children()) {
            if(std::string(currentSegmentOrGroup.name()) == "Segment") {
                processSegment(currentSegmentOrGroup, ediXmlNode);
            } else if(std::string(currentSegmentOrGroup.name()) == "SegmentGroup") {
                processSegmentGroups(currentSegmentOrGroup, ediXmlNode);
            }
        }
    }

    void Xml2EdiProcessor::processSegment(pugi::xml_node schemaNode, const pugi::xml_node& ediXmlNode) {
        std::string segmentName = schemaNode.attribute("name").as_string();

        int counter = 0;
        for (auto currentXmlEdiSegment: ediXmlNode.children(segmentName.c_str())) {
            counter++;
            std::cout << "processing segment '" << currentXmlEdiSegment.name() << "'" << std::endl;
            auto newEdiSegment = ediFile->newSegment(segmentName);

            for(pugi::xml_node schemaElement: schemaNode.children()) {
                if(std::string(schemaElement.name()) == "Element") {
                    std::string elementName = schemaElement.attribute("name").as_string();
                    if(elementIsCompositeElement(schemaElement)){
                        std::cout << "processing composite element '" << elementName << "'" << std::endl;
                    } else {
                        auto xmlEdiDataElement = currentXmlEdiSegment.child(edi::replaceWhitespaces(elementName));
                        std::cout << "processing element '" << elementName << "'" << std::endl;
                        newEdiSegment->newElement(xmlEdiDataElement.child_value());
                    }
                }
            }
        }

        if(counter == 0) {
            std::cout << "skipped segment '" << segmentName << "', that was not found in edixml." << std::endl;
        }
    }

    bool Xml2EdiProcessor::elementIsCompositeElement(pugi::xml_node &elementNode)
    {
        return elementNode.child("Composite") != nullptr;
    }

    void Xml2EdiProcessor::processSegmentGroups(pugi::xml_node node, const pugi::xml_node& ediXmlNode) {
        std::string segmentGroupName = node.attribute("name").as_string();
        std::cout << "processing all segment groups ' " << segmentGroupName << "'" << std::endl;

        int counter = 0;
        for (auto currentEdiSegment: ediXmlNode.children(segmentGroupName.c_str())) {
            counter++;
            processSegmentGroup(node, currentEdiSegment);
        }
        std::cout << "processed " << counter << " segment groups '" << segmentGroupName << "'." << std::endl;
    }

    void Xml2EdiProcessor::processSegmentGroup(pugi::xml_node node, const pugi::xml_node& ediXmlNode) {
        std::string segmentGroupName = node.attribute("name").as_string();
        std::cout << "entering segment group' " << segmentGroupName << "'" << std::endl;
        processSegmentsAndSegmentGroups(node, ediXmlNode);
        std::cout << "leaving segment group' " << segmentGroupName << "'" << std::endl;
    }

} // edi
