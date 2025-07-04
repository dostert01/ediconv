#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

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

    const std::optional<std::string> SchemaFile::getRootNodeAttributeValue(std::string attributeName) const {
        std::optional<std::string> returnValue = std::nullopt;
        if(fileLoadingSucceeded && getSchemaRootNode().has_value()) {
            pugi::xml_node rootNode = getSchemaRootNode().value();
            if(rootNode.attribute(attributeName).hash_value())
                returnValue = rootNode.attribute(attributeName).as_string();
        }
        return returnValue;
    }


    std::optional<pugi::xml_node> SchemaFile::getSchemaRootNode() const {
        std::optional<pugi::xml_node> returnValue = std::nullopt;
        pugi::xml_node rootNode = schemaXml.root().first_child();
        if(std::string(rootNode.name()) == std::string("EdifactMessage"))
            returnValue = rootNode;
        return returnValue;
    }

    void SchemaFile::process(EdiFile& edi, std::shared_ptr<pugi::xml_document> targetXml) {
        ediXlm = targetXml;
        std::optional<pugi::xml_node> messageRootNode = getSchemaRootNode();
        if(messageRootNode.has_value()) {
            pugi::xml_node ediXmlRootNode = ediXlm->append_child("EdifactMessage");
            for(auto attribute : messageRootNode.value().attributes()){
                ediXmlRootNode.append_attribute(attribute.name()) = attribute.as_string();
            }
            processAllChildSegments(messageRootNode.value(), edi, ediXmlRootNode);
        }
    }

    void SchemaFile::processAllChildSegments(pugi::xml_node& currentSchemaParentNode, EdiFile& edi, pugi::xml_node currentEdiParentNode) {
        for (pugi::xml_node segmentNode: currentSchemaParentNode.children()) {
            std::string schemaXmlNodeNameName = std::string(segmentNode.name());
            if(schemaXmlNodeNameName == "Segment") {
                std::string schemaSegmentName = segmentNode.attribute("name").value();
                bool isMandatorySegment = std::string(segmentNode.attribute("required").value()) == "true";
                int maxOccurs = segmentNode.attribute("maxOccurs").as_int();
                std::cout << "processing schema segment " << schemaSegmentName << std::endl;
                std::optional<Segment> currentEdiSegment = edi.getCurrentSegment();
                if(isMandatorySegment) {
                    while(!ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentName)) {
                        std::cout << "skipping edi segment " << currentEdiSegment.value().getName().value_or("") << std::endl;    
                        edi.gotoNextSegment();
                        currentEdiSegment = edi.getCurrentSegment();
                    }
                }
                if(ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentName)) {
                    Segment ediSegement = currentEdiSegment.value();
                    int currentOccurrences = 0;
                    while(ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentName) &&
                        (currentOccurrences < maxOccurs)) {
                        std::cout << "now processing edi segment " << ediSegement.getName().value_or("") << std::endl;
                        processElements(segmentNode, edi);
                        edi.gotoNextSegment();
                        currentEdiSegment = edi.getCurrentSegment();
                        currentOccurrences++;
                    }
                }
                
            } else if (schemaXmlNodeNameName == "SegmentGroup") {
                std::string schemaSegmentGroupName = segmentNode.attribute("name").value();
                std::string schemaSegmentGroupStart = segmentNode.attribute("start").value();
                std::optional<Segment> currentEdiSegment = edi.getCurrentSegment();
                while(ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentGroupStart)) {
                    std::cout << "processing schema group segment " << schemaSegmentGroupName << " starts with " << schemaSegmentGroupStart << std::endl;
                    processAllChildSegments(segmentNode, edi, currentEdiParentNode);
                    std::cout << "leaving schema group segment " << schemaSegmentGroupName << " starts with " << schemaSegmentGroupStart << std::endl;
                    currentEdiSegment = edi.getCurrentSegment();
                }
            }
        }
    }

    bool SchemaFile::ediSegmentMatchesSchemaSegment(std::optional<Segment>& currentEdiSegment, const std::string& schemaXmlNodeNameName) {
        return currentEdiSegment.has_value() && (currentEdiSegment.value().getName() == schemaXmlNodeNameName);
    }

    void SchemaFile::processElements(pugi::xml_node segmentNode, EdiFile& edi) {
        int elementIndex = 1;
        for (pugi::xml_node elementNode: segmentNode.children()) {
            std::string schemaXmlNodeNameName = std::string(elementNode.name());
            if(schemaXmlNodeNameName == "Element") {
                std::string elementName = elementNode.attribute("name").value();
                std::cout << "processing element " << elementName << std::endl;
                if(elementNode.child("Composite") != nullptr)
                    processCompositeElement(elementNode, edi, elementIndex);
                else {
                    std::cout << "element value: " << edi.getCurrentSegment().value().getElementValue(elementIndex) << std::endl;
                }
            }
            elementIndex++;
        }
    }

    void SchemaFile::processCompositeElement(pugi::xml_node elementNode, EdiFile& edi, const int elementIndex) {
        pugi::xml_node compositeNode = elementNode.child("Composite");
        if(compositeNode != nullptr) {
            std::string compositeNodeName = compositeNode.attribute("name").as_string();
            std::cout << "processing composite " << compositeNodeName << std::endl;
            int componentIndex = 0;
            for (pugi::xml_node componentNode: compositeNode.children()) {
                std::string schemaXmlNodeNameName = std::string(componentNode.name());
                if(schemaXmlNodeNameName == "Component") {
                    std::string componentName = componentNode.attribute("name").value();
                    std::cout << "processing element name: '" << componentName << 
                        "' value: '" << edi.getCurrentSegment().value().getElementValue(elementIndex, componentIndex) << "'" << std::endl;
                }
                componentIndex++;
           }
        }
    }

} // edi