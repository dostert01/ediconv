#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "edi2XmlProcessor.h"

#define SCHEMA_TEST_FILE_01 "/pricatSchema01.xml"
#define SCHEMA_TEST_FILE_02 "/pricatSchema02.xml"
#define EDI_TEST_FILE_01 "/pricatTestFile01.edi"

namespace test_schemafile {
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


TEST(schemaFile, fileLoadedReturnsFalseIfNothingIsLoaded) {
    test_schemafile::configureTest();
    edi::SchemaFile schemaFile;
    EXPECT_EQ(false, schemaFile.fileLoaded());
}

TEST(schemaFile, canLoadSchemaFile) {
    test_schemafile::configureTest();
    edi::SchemaFile schemaFile;
    schemaFile.loadFromFile(test_schemafile::testFilesDir + SCHEMA_TEST_FILE_01);
    EXPECT_EQ(true, schemaFile.fileLoaded());
}

TEST(schemaFile, getMessageTypeWorks) {
    test_schemafile::configureTest();
    edi::SchemaFile schemaFile;
    schemaFile.loadFromFile(test_schemafile::testFilesDir + SCHEMA_TEST_FILE_01);
    EXPECT_EQ("PRICAT", schemaFile.getMessageType());
}

TEST(schemaFile, getVersionWorks) {
    test_schemafile::configureTest();
    edi::SchemaFile schemaFile;
    schemaFile.loadFromFile(test_schemafile::testFilesDir + SCHEMA_TEST_FILE_01);
    EXPECT_EQ("D96A", schemaFile.getMessageVersion());
}

TEST(schemaFile, processingWorks) {
    test_schemafile::configureTest();
    auto schemaFile = std::make_shared<edi::SchemaFile>();
    auto edifile = std::make_shared<edi::EdiFile>();
    edi::Edi2XmlProcessor processor;
    schemaFile->loadFromFile(test_schemafile::testFilesDir + SCHEMA_TEST_FILE_02);
    edifile->loadFromFile((test_schemafile::testFilesDir + EDI_TEST_FILE_01));
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
