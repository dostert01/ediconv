include(CMakePrintHelpers)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/_deps/googletest/googletest/include/
)

# set variables
set(TEST_FILES_DIR "${CMAKE_SOURCE_DIR}/tests/testdata/" CACHE STRING "Directory where test data is expected by the tests")
cmake_print_variables(TEST_FILES_DIR)

# Adding GTest und GTest Main
link_libraries(
        gtest
        gtest_main
        pugixml
)

add_executable(
        test_edifile
        test_edifile.cpp
        ../EdiFile.cpp
)
add_test(NAME test_edifile COMMAND test_edifile)
set_tests_properties(test_edifile PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_executable(
        test_schemafile
        test_schemafile.cpp
        ../SchemaFile.cpp
        ../EdiFile.cpp
        ../common.cpp
)
add_test(NAME test_schemafile COMMAND test_schemafile)
set_tests_properties(test_schemafile PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_executable(
        test_edi2xmlprocessor
        test_edi2xmlprocessor.cpp
        ../SchemaFile.cpp
        ../EdiFile.cpp
        ../edi2XmlProcessor.cpp
        ../common.cpp
)
add_test(NAME test_edi2xmlprocessor COMMAND test_edi2xmlprocessor)
set_tests_properties(test_edi2xmlprocessor PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_executable(
        test_xml2metaprocessor
        test_xml2MetaProcessor.cpp
        ../xml2MetaProcessor.cpp
)
add_test(NAME test_xml2metaprocessor COMMAND test_xml2metaprocessor)
set_tests_properties(test_xml2metaprocessor PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_executable(
        test_xml2EdiProcessor
        test_xml2EdiProcessor.cpp
        ../xml2EdiProcessor.cpp
        ../SchemaFile.cpp
        ../EdiFile.cpp
        ../common.cpp
)
add_test(NAME test_xml2EdiProcessor COMMAND test_xml2EdiProcessor)
set_tests_properties(test_xml2EdiProcessor PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")

add_executable(
        test_common
        test_common.cpp
        ../common.cpp
)
add_test(NAME test_common COMMAND test_common)
set_tests_properties(test_common PROPERTIES ENVIRONMENT "TEST_FILES_DIR=${TEST_FILES_DIR}")
