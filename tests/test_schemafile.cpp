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

TEST(schemaQualification, getQualifiesEdiPath01) {
    test_schemafile::configureTest();
    edi::SchemaFile schemaFile;
    schemaFile.loadFromFile(test_schemafile::testFilesDir + SCHEMA_TEST_FILE_01);
    EXPECT_EQ("/PRICAT/UNH/Message_Reference_Number", schemaFile.getEdiPath("/EdifactMessage/Segment[@name='UNH']/Element[@name='Message Reference Number']"));
}

TEST(schemaQualification, getQualifiesEdiPath02) {
    test_schemafile::configureTest();
    edi::SchemaFile schemaFile;
    schemaFile.loadFromFile(test_schemafile::testFilesDir + SCHEMA_TEST_FILE_01);
    EXPECT_EQ("/PRICAT/UNH/S009_-_Message_Identifier/Controlling_Agency", schemaFile.getEdiPath("/EdifactMessage/Segment[@name='UNH']/Element[@name='Message Identifier']/Composite[@name='S009 - Message Identifier']/Component[@name='Controlling Agency']"));
}

TEST(schemaQualification, getQualifiesEdiPath03) {
    test_schemafile::configureTest();
    edi::SchemaFile schemaFile;
    schemaFile.loadFromFile(test_schemafile::testFilesDir + SCHEMA_TEST_FILE_01);
    EXPECT_EQ(
        "/PRICAT/SG22_ProductLineItem/SG23_ProductIDs/PIA/Product_ID_Function_Code",
        schemaFile.getEdiPath(
            "/EdifactMessage/SegmentGroup[@name='SG22_ProductLineItem']/SegmentGroup[@name='SG23_ProductIDs']/Segment[@name='PIA']/Element[@name='Product ID Function Code']"));
}
