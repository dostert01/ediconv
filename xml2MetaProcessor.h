#pragma once
#include <string>
#include <optional>
#include <memory>
#include <vector>
#include "pugixml.hpp"

namespace edi
{

    class MetaFileLine {
        public:
            MetaFileLine() = default;
    };

    class MetaFile {
        public:
            MetaFile();
            void loadFromXmlFile(const std::string& fileName);
            bool isXmlFileLoaded();
            std::vector<MetaFileLine>& getMetafileLines();
        private:
            pugi::xml_document xmlFile;
            bool fileLoadingSucceeded;
            std::vector<MetaFileLine> metafileLines;
    };

    class Xml2MetaProcessor {
        public:
            Xml2MetaProcessor() = default;
            void process(std::shared_ptr<pugi::xml_document> ediXml);
        private:
            std::shared_ptr<pugi::xml_document> xmlFile;
    };


} // edi
