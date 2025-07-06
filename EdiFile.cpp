#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

#include "EdiFile.h"

namespace edi {

    // ----------------------------------------------------------------------
    // Component
    // ----------------------------------------------------------------------
    Component::Component(const std::string& componentString) {
        value = componentString;
    }

    const std::string& Component::getValue() const {
        return value;
    }

    // ----------------------------------------------------------------------
    // Element
    // ----------------------------------------------------------------------
    Element::Element(const std::string& elementString) {
        rawElementString = elementString;
        createComponents();
    }

    void Element::createComponents() {
        size_t startIndex = 0;
        size_t currentPosition = 0;

        while ((currentPosition = rawElementString.find(EDI_COMPONENT_SEPARATOR, startIndex)) != std::string::npos) {
            components.emplace_back(rawElementString.substr(startIndex, currentPosition - startIndex));
            startIndex = currentPosition + 1;
        }
        components.emplace_back(rawElementString.substr(startIndex));
    }

    const std::string& Element::getRawElementString() const {
        return rawElementString;
    }

    void Element::setRawElementString(const std::string elementString) {
        rawElementString = elementString;
    }

    const std::vector<Component>& Element::getComponents() const {
        return components;
    }

    const bool Element::hasComponents() const {
        return components.size() > 0;
    }

    const bool Element::isSingleComponentElement() const {
        return components.size() == 1;
    }

    void Element::addComponent(std::string value) {
        components.emplace_back(value);
    }

    std::string Element::asString() {
        std::string returnValue;
        for(Component c: components) {
            returnValue += (c.getValue() + EDI_COMPONENT_SEPARATOR);
        }
        return returnValue.substr(0, returnValue.length() - 1);
    }

    // ----------------------------------------------------------------------
    // Segment
    // ----------------------------------------------------------------------
    Segment::Segment(const std::string& segmentString) {
        createContainedElements(segmentString);
    }

    void Segment::createContainedElements(const std::string& segmentString) {
        size_t startIndex = 0;
        size_t currentPosition = 0;

        while ((currentPosition = segmentString.find(EDI_ELEMENT_SEPARATOR, startIndex)) != std::string::npos) {
            elements.emplace_back(std::make_shared<Element>(segmentString.substr(startIndex, currentPosition - startIndex)));
            startIndex = currentPosition + 1;
        }
        elements.emplace_back(std::make_shared<Element>(segmentString.substr(startIndex, segmentString.length() - (startIndex))));
    }

    const std::vector<std::shared_ptr<Element>>& Segment::getElements() const {
        return elements;
    }

    std::optional<std::string> Segment::getName() {
        std::optional<std::string> returnValue = std::nullopt;
        if (hasElements())
        {
            returnValue = getElementByIndex(0)->getRawElementString();
        }
        return returnValue;
    }

    bool Segment::hasElements() const
    {
        return elements.size() > 0;
    }

    void Segment::setName(const std::string& segmentName) {
        if(!hasElements()) {
            elements.emplace_back(std::make_shared<Element>(segmentName));
        } else {
            getElementByIndex(0)->setRawElementString(segmentName);
        }
    }

    std::string Segment::getElementValue(const uint elementIndex) {
        std::string returnValue = "";
        if(elementIndex < elements.size()) {
            returnValue = getElementByIndex(elementIndex)->getRawElementString();
        }
        return returnValue;
    }

    std::string Segment::getElementValue(const uint elementIndex, const uint compositeIndex) {
        std::string returnValue = "";
        if(elementIndex < elements.size() && elements.at(elementIndex)->hasComponents() &&
            (compositeIndex < getElementByIndex(elementIndex)->getComponents().size())) {
                returnValue = getElementByIndex(elementIndex)->getComponents().at(compositeIndex).getValue();
        }
        return returnValue;
    }

    std::shared_ptr<Element> Segment::getElementByIndex(const uint elementIndex) {
        return elements.at(elementIndex);
    }

    std::shared_ptr<Element> Segment::newElement(std::string value) {
        elements.emplace_back(std::make_shared<Element>(value));
        return getElementByIndex(elements.size() - 1);
    }

    std::shared_ptr<Element> Segment::newElement() {
        elements.emplace_back(std::make_shared<Element>());
        return getElementByIndex(elements.size() - 1);
    }

    std::string Segment::asString() {
        std::string returnValue;
        for(std::shared_ptr<Element> e: elements) {
            returnValue += (e->asString() + EDI_ELEMENT_SEPARATOR);
        }
        return returnValue.substr(0, returnValue.length() - 1);
    }

    // ----------------------------------------------------------------------
    // EdiFile
    // ----------------------------------------------------------------------
    EdiFile::EdiFile() {
        fileLoadingSucceeded = false;
        readSegmentCursor = 0;
    }

    void EdiFile::loadFromFile(const std::string& fileName) {
        fileLoadingSucceeded = false;
        std::ifstream file(fileName);
        if(file) {
            std::string singleSegmentString;
            while(std::getline(file, singleSegmentString, EDI_SEGMENT_SEPARATOR)) {
                singleSegmentString = trim(singleSegmentString);
                if(singleSegmentString.length() > 0 && !singleSegmentString.empty())
                    segments.emplace_back(std::make_shared<Segment>(singleSegmentString));
            }
            fileLoadingSucceeded = true;
        } else {
            std::cerr << "failed to open file: " << fileName << std::endl;
        }
    }

    const std::vector<std::shared_ptr<Segment>>& EdiFile::getSegments() const {
        return segments;
    }

    std::optional<std::shared_ptr<Segment>> EdiFile::getSegment(int index) {
        std::optional<std::shared_ptr<Segment>> returnValue = std::nullopt;
        if(index < segments.size()) {
            returnValue = segments.at(index);
        }
        return returnValue;
    }

    std::optional<std::shared_ptr<Segment>> EdiFile::getCurrentSegment() {
        std::optional<std::shared_ptr<Segment>> returnValue = std::nullopt;
        if(readSegmentCursor < segments.size()) {
            returnValue = segments.at(readSegmentCursor);
        }
        return returnValue;
    }
    void EdiFile::gotoNextSegment() {
        readSegmentCursor++;
    }

    std::shared_ptr<Segment> EdiFile::newSegment(const std::string segmentName) {
        segments.emplace_back(std::make_shared<Segment>(segmentName));
        return segments.at(segments.size() - 1);
    }

    std::string EdiFile::asString() {
        std::string returnValue;
        for(std::shared_ptr<Segment> s: segments) {
            returnValue += (s->asString() + EDI_SEGMENT_SEPARATOR);
        }
        return returnValue;
    }


    std::string EdiFile::trim(const std::string& s) {
        std::string returnValue = "";
        auto start = std::find_if_not(s.begin(), s.end(), ::isspace);
        auto end = std::find_if_not(s.rbegin(), s.rend(), ::isspace).base();
    
        if (start < end)
            returnValue = std::string(start, end);
    
        return returnValue;
    }

    const bool EdiFile::fileLoaded() const {
        return fileLoadingSucceeded;
    }

} // edi