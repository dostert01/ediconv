#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "xml2MetaProcessor.h"

#define XML_TEST_FILE_01 "/pricatTestFile01.xml"

namespace test_xml2MetaProcessor {
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

TEST(MetaFile, processingWorks) {
    test_xml2MetaProcessor::configureTest();
    edi::MetaFile metafile;
    metafile.loadFromXmlFile(test_xml2MetaProcessor::testFilesDir + XML_TEST_FILE_01);
    EXPECT_EQ(true, metafile.isXmlFileLoaded());
}

TEST(MetaFile, getMetaFileLines) {
    test_xml2MetaProcessor::configureTest();
    edi::MetaFile metafile;
    metafile.loadFromXmlFile(test_xml2MetaProcessor::testFilesDir + XML_TEST_FILE_01);
    EXPECT_EQ(123, metafile.getMetafileLines().size());
}
