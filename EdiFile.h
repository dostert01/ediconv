#pragma once
#include <vector>
#include <string>
#include <optional>

namespace edi
{

#define EDI_SEGMENT_SEPARATOR '\''
#define EDI_ELEMENT_SEPARATOR '+'
#define EDI_COMPONENT_SEPARATOR ':'

    class Component {
    private:
        std::string value;

    public:
        Component(const std::string &componentString);
        const std::string &getValue() const;
    };

    class Element {
    private:
        std::string rawElementString;
        std::vector<Component> components;
        void createComponents();

    public:
        Element(const std::string &elementString);
        const std::string &getRawElementString() const;
        const std::vector<Component> &getComponents() const;
        const bool hasComponents() const;
        const bool isSingleComponentElement() const;
    };

    class Segment {
    private:
        std::vector<Element> elements;
        void createContainedElements(const std::string &segmentString);
    public:
        Segment() = default;
        Segment(const std::string &segmentString);
        const std::vector<Element> &getElements() const;
        std::optional<std::string> getName() const;
        std::string getElementValue(const uint elementIndex);
        std::string getElementValue(const uint elementIndex, const uint compositeIndex);
    };

    class EdiFile {
        public:
            EdiFile();
            void loadFromFile(const std::string& fileName);
            const std::vector<Segment>& getSegments() const;
            const bool fileLoaded() const;
            std::optional<Segment> getCurrentSegment();
            void gotoNextSegment();
        private:
            std::vector<Segment> segments;
            std::string trim(const std::string& s);
            bool fileLoadingSucceeded;
            int readSegmentCursor;
    };

} // edi
