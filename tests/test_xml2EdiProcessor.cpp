#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "xml2EdiProcessor.h"
#include "SchemaFile.h"
#include "EdiFile.h"

#define XML_TEST_FILE_01 "/pricatTestFile01.xml"
#define SCHEMA_TEST_FILE_02 "/pricatSchema02.xml"
#define EDI_TEST_FILE_03 "/pricatTestFile03.edi"

namespace 
test_xml2EdiProcessor {
    std::string workingDir;
    std::string testFilesDir;

    void log(const std::string &msg) {
        std::cout << msg << std::endl;
    }

    void configureTest() {
        workingDir = std::filesystem::current_path();
        log("Running test in directory: " + workingDir);
        if(getenv("TEST_FILES_DIR") != nullptr) {
            testFilesDir = getenv("TEST_FILES_DIR");
            log("Testfiles expected to be present in directory: " + testFilesDir);
        } else {
            log("Environment variable TEST_FILES_DIR must be set to where the tests expect the testfiles to live!");
        }
    }
}

TEST(Xml2Edi, processingWorks) {
    test_xml2EdiProcessor::configureTest();
    auto schemaFile = std::make_shared<edi::SchemaFile>();
    auto edixmlFile = std::make_shared<pugi::xml_document>();
    auto ediFile = std::make_shared<edi::EdiFile>();
    edi::Xml2EdiProcessor processor;
    schemaFile->loadFromFile(test_xml2EdiProcessor::testFilesDir + SCHEMA_TEST_FILE_02);
    edixmlFile->load_file(std::string(test_xml2EdiProcessor::testFilesDir + XML_TEST_FILE_01).c_str());
    edixmlFile->save(std::cout, "  ");

    processor.process(edixmlFile, schemaFile, ediFile);
    std::string result = ediFile->asString();
    edi::EdiFile nominalValueEdiFile;
    nominalValueEdiFile.loadFromFile(test_xml2EdiProcessor::testFilesDir + EDI_TEST_FILE_03);
    
    EXPECT_EQ(nominalValueEdiFile.asString(), ediFile->asString());
    
}

