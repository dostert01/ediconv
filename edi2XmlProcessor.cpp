#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <ranges>

#include "edi2XmlProcessor.h"
#include "common.h"

namespace edi {

    // ----------------------------------------------------------------------
    // Edi2XmlProcessor
    // ----------------------------------------------------------------------
    void Edi2XmlProcessor::process(std::shared_ptr<EdiFile> edi, std::shared_ptr<SchemaFile> schema,  std::shared_ptr<pugi::xml_document> targetXml) {
        ediXlm = targetXml;
        ediFile = edi;
        schemaFile = schema;
        std::optional<pugi::xml_node> messageRootNode = schema->getSchemaRootNode();
        if(messageRootNode.has_value()) {
            std::string messageTypeName = messageRootNode.value().attribute("type").as_string();
            pugi::xml_node ediXmlRootNode = targetXml->append_child(messageTypeName);
            processAllChildSegments(messageRootNode.value(), ediXmlRootNode);
        }
    }

    void Edi2XmlProcessor::processAllChildSegments(pugi::xml_node& currentSchemaParentNode, pugi::xml_node currentEdiParentNode) {
        for (pugi::xml_node segmentNode: currentSchemaParentNode.children()) {
            
            std::string schemaXmlNodeNameName = std::string(segmentNode.name());
            std::string segmentName = segmentNode.attribute("name").as_string();
    
            if(schemaXmlNodeNameName == "Segment") {
                pugi::xml_node targetSegmentNode = appendNode(currentEdiParentNode, segmentName);
                processDataSegment(segmentNode, targetSegmentNode);
            } else if (schemaXmlNodeNameName == "SegmentGroup") {
                processAllSegmentGroutRepetitions(segmentNode, currentEdiParentNode, segmentName);
            }
            
        }
    }

    void Edi2XmlProcessor::processAllSegmentGroutRepetitions(pugi::xml_node &segmentNode, pugi::xml_node &currentEdiParentNode, std::string &segmentName)
    {
        std::string schemaSegmentGroupName = segmentNode.attribute("name").value();
        std::string schemaSegmentGroupStart = segmentNode.attribute("start").value();
        auto currentEdiSegment = ediFile->getCurrentSegment();
        while (ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentGroupStart))
        {
            pugi::xml_node targetSegmentNode = appendNode(currentEdiParentNode, segmentName);
            processSegmentGroup(segmentNode, targetSegmentNode);
            currentEdiSegment = ediFile->getCurrentSegment();
        }
    }

    void Edi2XmlProcessor::processSegmentGroup(pugi::xml_node &segmentNode, pugi::xml_node& currentEdiParentNode)
    {
        std::string schemaSegmentGroupName = segmentNode.attribute("name").value();
        std::string schemaSegmentGroupStart = segmentNode.attribute("start").value();
        auto currentEdiSegment = ediFile->getCurrentSegment();
        std::cout << "processing schema group segment " << schemaSegmentGroupName << " starts with " << schemaSegmentGroupStart << std::endl;
        processAllChildSegments(segmentNode, currentEdiParentNode);
        std::cout << "leaving schema group segment " << schemaSegmentGroupName << " starts with " << schemaSegmentGroupStart << std::endl;
    }

    void Edi2XmlProcessor::processDataSegment(pugi::xml_node &segmentNode, pugi::xml_node currentEdiParentNode)
    {
        std::string schemaSegmentName = segmentNode.attribute("name").value();
        bool isMandatorySegment = std::string(segmentNode.attribute("required").value()) == "true";
        int maxOccurs = segmentNode.attribute("maxOccurs").as_int();
        std::cout << "processing schema segment " << schemaSegmentName << std::endl;
        auto currentEdiSegment = ediFile->getCurrentSegment();
        if (isMandatorySegment) {
            findMatchingDataSegmentInEdi(currentEdiSegment, schemaSegmentName);
        }
        if (ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentName)) {
            processAllMatchingEdiSegments(currentEdiSegment, schemaSegmentName, maxOccurs, segmentNode, currentEdiParentNode);
        }
    }

    void Edi2XmlProcessor::processAllMatchingEdiSegments(std::optional<std::shared_ptr<edi::Segment>> &currentEdiSegment, std::string &schemaSegmentName, int maxOccurs, pugi::xml_node &segmentNode, pugi::xml_node currentEdiParentNode)
    {
        std::shared_ptr<Segment> ediSegment = currentEdiSegment.value();
        int currentOccurrences = 0;
        while (ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentName) && (currentOccurrences < maxOccurs))
        {
            std::cout << "now processing edi segment " << ediSegment->getName().value_or("") << std::endl;
            processElements(segmentNode, currentEdiParentNode);
            ediFile->gotoNextSegment();
            currentEdiSegment = ediFile->getCurrentSegment();
            currentOccurrences++;
        }
    }

    void Edi2XmlProcessor::findMatchingDataSegmentInEdi(std::optional<std::shared_ptr<edi::Segment>> &currentEdiSegment, std::string &schemaSegmentName)
    {
        while (!ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentName))
        {
            std::cout << "skipping edi segment " << currentEdiSegment.value()->getName().value_or("") << std::endl;
            ediFile->gotoNextSegment();
            currentEdiSegment = ediFile->getCurrentSegment();
        }
    }

    bool Edi2XmlProcessor::ediSegmentMatchesSchemaSegment(std::optional<std::shared_ptr<Segment>>& currentEdiSegment, const std::string& schemaXmlNodeNameName) {
        return currentEdiSegment.has_value() && (currentEdiSegment.value()->getName() == schemaXmlNodeNameName);
    }

    void Edi2XmlProcessor::processElements(pugi::xml_node segmentNode, pugi::xml_node currentEdiParentNode) {
        int elementIndex = 1;
        for (pugi::xml_node elementNode: segmentNode.children()) {
            std::string schemaXmlNodeNameName = std::string(elementNode.name());
            if(schemaXmlNodeNameName == "Element") {
                std::string elementName = elementNode.attribute("name").value();
                std::cout << "processing element " << elementName << std::endl;
                if (elementIsCompositeElement(elementNode))
                    processCompositeElement(elementNode, elementIndex, currentEdiParentNode);
                else {
                    std::string value = ediFile->getCurrentSegment().value()->getElementValue(elementIndex);
                    std::cout << "element value: " << value << std::endl;
                    appendPCDataNode(currentEdiParentNode, elementName, value);
                    addAsQualifierToParentNode(elementNode, currentEdiParentNode, elementName, value);
                }
            }
            elementIndex++;
        }
    }

    bool Edi2XmlProcessor::elementIsCompositeElement(pugi::xml_node &elementNode)
    {
        return elementNode.child("Composite") != nullptr;
    }

    void Edi2XmlProcessor::addAsQualifierToParentNode(pugi::xml_node &elementNode, pugi::xml_node &currentEdiParentNode, std::string &elementName, std::string &value)    {
        std::string isQualifier = elementNode.attribute("isQualifier").as_string();
        if (isQualifier == "true") {
            appendAttribute(currentEdiParentNode, elementName, value);
        }
    }

    void Edi2XmlProcessor::appendAttribute(pugi::xml_node &currentEdiParentNode, std::string &attributeName, std::string &value) {
        currentEdiParentNode.append_attribute(replaceWhitespaces(attributeName)) = value;
    }

    pugi::xml_node Edi2XmlProcessor::appendPCDataNode(pugi::xml_node &currentEdiParentNode, std::string &elementName, std::string &value)
    {
        pugi::xml_node node = appendNode(currentEdiParentNode, elementName);
        node.append_child(pugi::node_pcdata).set_value(value);
        return node;
    }

    pugi::xml_node Edi2XmlProcessor::appendNode(pugi::xml_node &currentEdiParentNode, std::string &elementName)
    {
        pugi::xml_node node = currentEdiParentNode.append_child(replaceWhitespaces(elementName));
        return node;
    }

    void Edi2XmlProcessor::processCompositeElement(pugi::xml_node elementNode, const int elementIndex, pugi::xml_node currentEdiParentNode) {
        pugi::xml_node compositeNode = elementNode.child("Composite");
        if(compositeNode != nullptr) {
            std::string compositeNodeName = compositeNode.attribute("name").as_string();
            std::cout << "processing composite " << compositeNodeName << std::endl;
            pugi::xml_node compositeTargetNode = currentEdiParentNode.append_child(replaceWhitespaces(compositeNodeName));
            int componentIndex = 0;
            for (pugi::xml_node componentNode: compositeNode.children()) {
                std::string schemaXmlNodeNameName = std::string(componentNode.name());
                if(schemaXmlNodeNameName == "Component") {
                    std::string componentName = componentNode.attribute("name").value();
                    std::string value = ediFile->getCurrentSegment().value()->getElementValue(elementIndex, componentIndex);
                    appendPCDataNode(compositeTargetNode, componentName, value);
                    addAsQualifierToParentNode(componentNode, compositeTargetNode, componentName, value);
                    std::cout << "processing element name: '" << componentName << 
                        "' value: '" << value << "'" << std::endl;
                }
                componentIndex++;
           }
        }
    }

} // edi