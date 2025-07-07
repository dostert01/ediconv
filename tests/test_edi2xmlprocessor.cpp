#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "edi2XmlProcessor.h"

#define SCHEMA_TEST_FILE_01 "/pricatSchema01.xml"
#define SCHEMA_TEST_FILE_02 "/pricatSchema02.xml"
#define EDI_TEST_FILE_01 "/pricatTestFile01.edi"
#define EDI_TEST_FILE_03 "/pricatTestFile03.edi"

namespace test_edi2XmlProcessor {
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

TEST(schemaFile, processingWorks01) {
    test_edi2XmlProcessor::configureTest();
    auto schemaFile = std::make_shared<edi::SchemaFile>();
    auto edifile = std::make_shared<edi::EdiFile>();
    edi::Edi2XmlProcessor processor;
    schemaFile->loadFromFile(test_edi2XmlProcessor::testFilesDir + SCHEMA_TEST_FILE_02);
    edifile->loadFromFile((test_edi2XmlProcessor::testFilesDir + EDI_TEST_FILE_01));
    std::shared_ptr<pugi::xml_document> ediXml = std::make_shared<pugi::xml_document>();
    processor.process(edifile, schemaFile, ediXml);
    ediXml->save(std::cout, "  ");
}

TEST(schemaFile, processingWorks02) {
    test_edi2XmlProcessor::configureTest();
    auto schemaFile = std::make_shared<edi::SchemaFile>();
    auto edifile = std::make_shared<edi::EdiFile>();
    edi::Edi2XmlProcessor processor;
    schemaFile->loadFromFile(test_edi2XmlProcessor::testFilesDir + SCHEMA_TEST_FILE_02);
    edifile->loadFromFile((test_edi2XmlProcessor::testFilesDir + EDI_TEST_FILE_03));
    std::shared_ptr<pugi::xml_document> ediXml = std::make_shared<pugi::xml_document>();
    processor.process(edifile, schemaFile, ediXml);
    ediXml->save(std::cout, "  ");
}

TEST(creatingXmlDoc, canCreateANode) {
    pugi::xml_document doc;
    pugi::xml_node node = doc.append_child("rootElement");
    doc.save(std::cout, "  ");
    EXPECT_EQ("rootElement", std::string(node.name()));
}
