#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "common.h"


namespace 
test_common {
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

TEST(common, replaceWhitespaceWithUnderscore) {
    std::string s = " Hallo Welt ";
    EXPECT_EQ("_Hallo_Welt_", edi::replaceWhitespaces(s));
}

TEST(common, replaceWhitespaceWithSomethingElse) {
    std::string s = " Hallo Welt ";
    std::string r = "##";
    EXPECT_EQ("##Hallo##Welt##", edi::replaceWhitespaces(s, r));
}