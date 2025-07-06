#pragma once
#include <vector>
#include <string>
#include <optional>
#include <memory>

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

    public:
        Element() = default;
        Element(const std::string &elementString);
        const std::string &getRawElementString() const;
        void setRawElementString(const std::string elementString);
        const std::vector<Component> &getComponents() const;
        const bool hasComponents() const;
        const bool isSingleComponentElement() const;
        void addComponent(std::string value);
        std::string asString();

    private:
        std::string rawElementString;
        std::vector<Component> components;
        void createComponents();

    };

    class Segment {
    public:
        Segment() = default;
        Segment(const std::string &segmentString);
        const std::vector<std::shared_ptr<Element>> &getElements() const;
        void setName(const std::string& segmentName);
        std::optional<std::string> getName();
        bool hasElements() const;
        std::shared_ptr<Element> newElement();
        std::shared_ptr<Element> newElement(std::string value);
        std::string getElementValue(const uint elementIndex);
        std::string getElementValue(const uint elementIndex, const uint compositeIndex);
        std::string asString();

    private:
        std::vector<std::shared_ptr<Element>> elements;
        void createContainedElements(const std::string &segmentString);
        std::shared_ptr<Element> getElementByIndex(const uint elementIndex);
    };

    class EdiFile {
        public:
            EdiFile();
            void loadFromFile(const std::string& fileName);
            const std::vector<std::shared_ptr<Segment>>& getSegments() const;
            std::optional<std::shared_ptr<Segment>> getSegment(int index);
            const bool fileLoaded() const;
            std::optional<std::shared_ptr<Segment>> getCurrentSegment();
            void gotoNextSegment();
            std::shared_ptr<Segment> newSegment(const std::string segmentName);
            std::string asString();
        private:
            std::vector<std::shared_ptr<Segment>> segments;
            std::string trim(const std::string& s);
            bool fileLoadingSucceeded;
            int readSegmentCursor;
    };

} // edi
