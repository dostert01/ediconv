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
            std::string messageTypeName = messageRootNode.value().attribute("type").as_string();
            pugi::xml_node ediXmlRootNode = targetXml->append_child(messageTypeName);
            processAllChildSegments(messageRootNode.value(), edi, ediXmlRootNode);
        }
    }

    void SchemaFile::processAllChildSegments(pugi::xml_node& currentSchemaParentNode, EdiFile& edi, pugi::xml_node currentEdiParentNode) {
        for (pugi::xml_node segmentNode: currentSchemaParentNode.children()) {
            std::string schemaXmlNodeNameName = std::string(segmentNode.name());
            std::string segmentName = segmentNode.attribute("name").as_string();
            pugi::xml_node targetSegmentNode = appendNode(currentEdiParentNode, segmentName);
            if(schemaXmlNodeNameName == "Segment") {
                processDataSegment(segmentNode, edi, targetSegmentNode);
            } else if (schemaXmlNodeNameName == "SegmentGroup") {
                processSegmentGroup(segmentNode, edi, targetSegmentNode);
            }
        }
    }

    void SchemaFile::processSegmentGroup(pugi::xml_node &segmentNode, edi::EdiFile &edi, pugi::xml_node& currentEdiParentNode)
    {
        std::string schemaSegmentGroupName = segmentNode.attribute("name").value();
        std::string schemaSegmentGroupStart = segmentNode.attribute("start").value();
        std::optional<Segment> currentEdiSegment = edi.getCurrentSegment();
        while (ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentGroupStart))
        {
            std::cout << "processing schema group segment " << schemaSegmentGroupName << " starts with " << schemaSegmentGroupStart << std::endl;
            processAllChildSegments(segmentNode, edi, currentEdiParentNode);
            std::cout << "leaving schema group segment " << schemaSegmentGroupName << " starts with " << schemaSegmentGroupStart << std::endl;
            currentEdiSegment = edi.getCurrentSegment();
        }
    }

    void SchemaFile::processDataSegment(pugi::xml_node &segmentNode, edi::EdiFile &edi, pugi::xml_node currentEdiParentNode)
    {
        std::string schemaSegmentName = segmentNode.attribute("name").value();
        bool isMandatorySegment = std::string(segmentNode.attribute("required").value()) == "true";
        int maxOccurs = segmentNode.attribute("maxOccurs").as_int();
        std::cout << "processing schema segment " << schemaSegmentName << std::endl;
        std::optional<Segment> currentEdiSegment = edi.getCurrentSegment();
        if (isMandatorySegment) {
            findMatchingDataSegmentInEdi(currentEdiSegment, schemaSegmentName, edi);
        }
        if (ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentName)) {
            processAllMatchingEdiSegments(currentEdiSegment, schemaSegmentName, maxOccurs, segmentNode, edi, currentEdiParentNode);
        }
    }

    void SchemaFile::processAllMatchingEdiSegments(std::optional<edi::Segment> &currentEdiSegment, std::string &schemaSegmentName, int maxOccurs, pugi::xml_node &segmentNode, edi::EdiFile &edi, pugi::xml_node currentEdiParentNode)
    {
        Segment ediSegment = currentEdiSegment.value();
        int currentOccurrences = 0;
        while (ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentName) && (currentOccurrences < maxOccurs))
        {
            std::cout << "now processing edi segment " << ediSegment.getName().value_or("") << std::endl;
            processElements(segmentNode, edi, currentEdiParentNode);
            edi.gotoNextSegment();
            currentEdiSegment = edi.getCurrentSegment();
            currentOccurrences++;
        }
    }

    void SchemaFile::findMatchingDataSegmentInEdi(std::optional<edi::Segment> &currentEdiSegment, std::string &schemaSegmentName, edi::EdiFile &edi)
    {
        while (!ediSegmentMatchesSchemaSegment(currentEdiSegment, schemaSegmentName))
        {
            std::cout << "skipping edi segment " << currentEdiSegment.value().getName().value_or("") << std::endl;
            edi.gotoNextSegment();
            currentEdiSegment = edi.getCurrentSegment();
        }
    }

    bool SchemaFile::ediSegmentMatchesSchemaSegment(std::optional<Segment>& currentEdiSegment, const std::string& schemaXmlNodeNameName) {
        return currentEdiSegment.has_value() && (currentEdiSegment.value().getName() == schemaXmlNodeNameName);
    }

    void SchemaFile::processElements(pugi::xml_node segmentNode, EdiFile& edi, pugi::xml_node currentEdiParentNode) {
        int elementIndex = 1;
        for (pugi::xml_node elementNode: segmentNode.children()) {
            std::string schemaXmlNodeNameName = std::string(elementNode.name());
            if(schemaXmlNodeNameName == "Element") {
                std::string elementName = elementNode.attribute("name").value();
                std::cout << "processing element " << elementName << std::endl;
                if(elementNode.child("Composite") != nullptr)
                    processCompositeElement(elementNode, edi, elementIndex, currentEdiParentNode);
                else {
                    std::string value = edi.getCurrentSegment().value().getElementValue(elementIndex);
                    std::cout << "element value: " << value << std::endl;
                    appendPCDataNode(currentEdiParentNode, elementName, value);
                    addAsQualifierToParentNode(elementNode, currentEdiParentNode, elementName, value);
                }
            }
            elementIndex++;
        }
    }

    void SchemaFile::addAsQualifierToParentNode(pugi::xml_node &elementNode, pugi::xml_node &currentEdiParentNode, std::string &elementName, std::string &value)    {
        std::string isQualifier = elementNode.attribute("isQualifier").as_string();
        if (isQualifier == "true") {
            appendAttribute(currentEdiParentNode, elementName, value);
        }
    }

    void SchemaFile::appendAttribute(pugi::xml_node &currentEdiParentNode, std::string &attributeName, std::string &value) {
        currentEdiParentNode.append_attribute(replaceWhitespaces(attributeName)) = value;
    }

    pugi::xml_node SchemaFile::appendPCDataNode(pugi::xml_node &currentEdiParentNode, std::string &elementName, std::string &value)
    {
        pugi::xml_node node = appendNode(currentEdiParentNode, elementName);
        node.append_child(pugi::node_pcdata).set_value(value);
        return node;
    }

    pugi::xml_node SchemaFile::appendNode(pugi::xml_node &currentEdiParentNode, std::string &elementName)
    {
        pugi::xml_node node = currentEdiParentNode.append_child(replaceWhitespaces(elementName));
        return node;
    }

    void SchemaFile::processCompositeElement(pugi::xml_node elementNode, EdiFile& edi, const int elementIndex, pugi::xml_node currentEdiParentNode) {
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
                    std::string value = edi.getCurrentSegment().value().getElementValue(elementIndex, componentIndex);
                    appendPCDataNode(compositeTargetNode, componentName, value);
                    addAsQualifierToParentNode(componentNode, compositeTargetNode, componentName, value);
                    std::cout << "processing element name: '" << componentName << 
                        "' value: '" << value << "'" << std::endl;
                }
                componentIndex++;
           }
        }
    }

    std::string SchemaFile::replaceWhitespaces(std::string& s) {
        std::string r = "_";
        return replaceWhitespaces(s, r);
    }

    std::string SchemaFile::replaceWhitespaces(std::string& s, std::string& r) {
        std::string toReplace = " ";
        size_t start = 0;
        while((start = s.find(toReplace, start)) != std::string::npos) {
            s.replace(start, toReplace.length(), r);
            start += r.length();
        }
        return s;
    }

} // edi