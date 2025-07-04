#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "EdiFile.h"

#define EDI_TEST_FILE_01 "/pricatTestFile01.edi"
#define EDI_TEST_FILE_02 "/pricatTestFile02.edi"

namespace test_common {
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

TEST(parseEdiSegment, parseToElements) {
    const std::string segmentString = R"(UNH+1+PRICAT:D:96A:UN)";
    edi::Segment segment = edi::Segment(segmentString);
    EXPECT_EQ(3,segment.getElements().size());
    EXPECT_EQ("UNH",segment.getElements().at(0).getRawElementString());
    EXPECT_EQ("1",segment.getElements().at(1).getRawElementString());
    EXPECT_EQ("PRICAT:D:96A:UN",segment.getElements().at(2).getRawElementString());
}

TEST(parseEdiSegment, segmentHasName) {
    const std::string segmentString = R"(NAD+BY+123456789::9)";
    edi::Segment segment = edi::Segment(segmentString);
    EXPECT_EQ("NAD", segment.getName());
}

TEST(parseEdiSegment, canParseSegementElements) {
    const std::string segmentString = R"(NAD+BY+123456789::9)";
    edi::Segment segment = edi::Segment(segmentString);
    edi::Element element = segment.getElements().at(2);
    EXPECT_EQ("123456789::9", element.getRawElementString());
    EXPECT_EQ(3, element.getComponents().size());
}

TEST(parseEdiSegment, hasComponentsWorks) {
    const std::string segmentString = R"(NAD+BY+123456789::9)";
    edi::Segment segment = edi::Segment(segmentString);
    EXPECT_EQ(true, segment.getElements().at(1).hasComponents());
    EXPECT_EQ(true, segment.getElements().at(2).hasComponents());
}

TEST(parseEdiSegment, isSingleComponentElement) {
    const std::string segmentString = R"(NAD+BY+123456789::9)";
    edi::Segment segment = edi::Segment(segmentString);
    EXPECT_EQ(true, segment.getElements().at(1).isSingleComponentElement());
    EXPECT_EQ(false, segment.getElements().at(2).isSingleComponentElement());
}

TEST(ediFile, canReadFromFile01) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile(test_common::testFilesDir + EDI_TEST_FILE_01);
    EXPECT_EQ(16, edifile.getSegments().size());
}

TEST(ediFile, canReadFromFile02) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile(test_common::testFilesDir + EDI_TEST_FILE_01);
    EXPECT_EQ("BGM", edifile.getSegments().at(1).getName());
}

TEST(ediFile, canReadFromFile03) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile(test_common::testFilesDir + EDI_TEST_FILE_02);
    EXPECT_EQ(16, edifile.getSegments().size());
}

TEST(ediFile, canReadFromFile04) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile(test_common::testFilesDir + EDI_TEST_FILE_02);
    EXPECT_EQ("BGM", edifile.getSegments().at(1).getName());
}

TEST(ediFile, fileLoadedRetrunTrueOnSuccess) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile(test_common::testFilesDir + EDI_TEST_FILE_02);
    EXPECT_EQ(true, edifile.fileLoaded());
}

TEST(ediFile, fileLoadedRetrunFalseOnFailure) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile("no file path");
    EXPECT_EQ(false, edifile.fileLoaded());
}

TEST(ediFile, canTraverseUsingSegmentCursor) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile(test_common::testFilesDir + EDI_TEST_FILE_01);
    std::optional<edi::Segment> currentSegment;
    currentSegment = edifile.getCurrentSegment();
    EXPECT_EQ("UNH", currentSegment.value().getName());
    currentSegment = edifile.getCurrentSegment();
    EXPECT_EQ("UNH", currentSegment.value().getName());
    for(int i=0; i<5; i++)
        edifile.gotoNextSegment();
    currentSegment = edifile.getCurrentSegment();
    EXPECT_EQ("LIN", currentSegment.value().getName());
    for(int i=0; i<10; i++)
        edifile.gotoNextSegment();
    currentSegment = edifile.getCurrentSegment();
    EXPECT_EQ("UNT", currentSegment.value().getName());
    edifile.gotoNextSegment();
    currentSegment = edifile.getCurrentSegment();
    EXPECT_EQ(false, currentSegment.has_value());
}

TEST(ediFile, getElementValue01) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile(test_common::testFilesDir + EDI_TEST_FILE_02);
    edi::Segment currentSegment;
    currentSegment = edifile.getCurrentSegment().value();
    currentSegment.getElementValue(1);
    EXPECT_EQ("PRICAT:D:96A:UN", currentSegment.getElementValue(2));
}

TEST(ediFile, getElementValue02) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile(test_common::testFilesDir + EDI_TEST_FILE_02);
    edi::Segment currentSegment;
    currentSegment = edifile.getCurrentSegment().value();
    currentSegment.getElementValue(1);
    EXPECT_EQ("96A", currentSegment.getElementValue(2, 2));
}

TEST(ediFile, getElementValueReturnsEmptyStringOnIndexOutOfBounds01) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile(test_common::testFilesDir + EDI_TEST_FILE_02);
    edi::Segment currentSegment;
    currentSegment = edifile.getCurrentSegment().value();
    currentSegment.getElementValue(1);
    EXPECT_EQ("", currentSegment.getElementValue(2, 4));
}

TEST(ediFile, getElementValueReturnsEmptyStringOnIndexOutOfBounds02) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile(test_common::testFilesDir + EDI_TEST_FILE_02);
    edi::Segment currentSegment;
    currentSegment = edifile.getCurrentSegment().value();
    currentSegment.getElementValue(1);
    EXPECT_EQ("", currentSegment.getElementValue(200, 1));
}

TEST(ediFile, getElementValueReturnsEmptyStringOnIndexOutOfBounds03) {
    test_common::configureTest();
    edi::EdiFile edifile;
    edifile.loadFromFile(test_common::testFilesDir + EDI_TEST_FILE_02);
    edi::Segment currentSegment;
    currentSegment = edifile.getCurrentSegment().value();
    currentSegment.getElementValue(1);
    EXPECT_EQ("", currentSegment.getElementValue(200));
}
