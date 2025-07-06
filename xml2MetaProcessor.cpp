#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <ranges>

#include "xml2MetaProcessor.h"

namespace edi {

    // ----------------------------------------------------------------------
    // MetaFile
    // ----------------------------------------------------------------------
    MetaFile::MetaFile() {
        fileLoadingSucceeded = false;
    }

    void MetaFile::loadFromXmlFile(const std::string& fileName) {
        pugi::xml_parse_result result = xmlFile.load_file(fileName.c_str());
        fileLoadingSucceeded = result != -1;    
    }

    bool MetaFile::isXmlFileLoaded() {
        return fileLoadingSucceeded;
    }

    std::vector<MetaFileLine>& MetaFile::getMetafileLines() {
        return metafileLines;
    }

    // ----------------------------------------------------------------------
    // Xml2MetaProcessor
    // ----------------------------------------------------------------------
    void Xml2MetaProcessor::process(std::shared_ptr<pugi::xml_document> ediXml) {
        xmlFile = ediXml;
    }

} // edi