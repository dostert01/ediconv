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

    const std::vector<Component>& Element::getComponents() const {
        return components;
    }

    const bool Element::hasComponents() const {
        return components.size() > 0;
    }

    const bool Element::isSingleComponentElement() const {
        return components.size() == 1;
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
            elements.emplace_back(segmentString.substr(startIndex, currentPosition - startIndex));
            startIndex = currentPosition + 1;
        }
        elements.emplace_back(segmentString.substr(startIndex, segmentString.length() - (startIndex)));
    }

    const std::vector<Element>& Segment::getElements() const {
        return elements;
    }

    std::optional<std::string> Segment::getName() const {
        std::optional<std::string> returnValue = std::nullopt;
        if (elements.size() > 0) {
            returnValue = elements.at(0).getRawElementString();  
        }
        return returnValue;
    }

    std::string Segment::getElementValue(const uint elementIndex) {
        std::string returnValue = "";
        if(elementIndex < elements.size()) {
            returnValue = elements.at(elementIndex).getRawElementString();
        }
        return returnValue;
    }

    std::string Segment::getElementValue(const uint elementIndex, const uint compositeIndex) {
        std::string returnValue = "";
        if(elementIndex < elements.size() && elements.at(elementIndex).hasComponents() && (compositeIndex < elements.at(elementIndex).getComponents().size())) {
            returnValue = elements.at(elementIndex).getComponents().at(compositeIndex).getValue();
        }
        return returnValue;
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
                    segments.emplace_back(Segment(singleSegmentString));
            }
            fileLoadingSucceeded = true;
        } else {
            std::cerr << "failed to open file: " << fileName << std::endl;
        }
    }

    const std::vector<Segment>& EdiFile::getSegments() const {
        return segments;
    }

    std::optional<Segment> EdiFile::getCurrentSegment() {
        std::optional<Segment> returnValue = std::nullopt;
        if(readSegmentCursor < segments.size()) {
            returnValue = segments.at(readSegmentCursor);
        }
        return returnValue;
    }
    void EdiFile::gotoNextSegment() {
        readSegmentCursor++;
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