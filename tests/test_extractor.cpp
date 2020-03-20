#include <gtest/gtest.h>
#include <QFile>
#include "codeextractor.h"
#include "test_data.h"

TEST(test_extractor, extraction) {
    // Get model file data
    QFile file("../tests/data/model_main.c"); // Note: Test should be run from build/ for this path to work

    if(!file.open(QIODevice::ReadOnly)) {
        FAIL() << " Couldn't open file: " + file.errorString().toStdString();
    }
    QTextStream file_content(&file);
    CodeExtractor test_extractor;

    // Check extraction and extraction status
    ASSERT_FALSE(test_extractor.getExtractionComplete());
    test_extractor.extractFromSourceFile(protocol_name, &file_content, cmd_list);
    ASSERT_TRUE(test_extractor.getExtractionComplete());
    file.close();

    // Check output
    ASSERT_TRUE(test_extractor.getUnknownIncludes() == unk_inc);
    ASSERT_TRUE(test_extractor.getUnknownDefinitions() == unk_def);
    ASSERT_TRUE(test_extractor.getUnknownPrivateFunctions() == unk_pri_fct);
    ASSERT_TRUE(test_extractor.getUnknownPublicFunctions() == unk_pub_fct);
    ASSERT_TRUE(test_extractor.getUnknownPublicFunctionsHeaders() == unk_pub_hdr);
    ASSERT_TRUE(test_extractor.getDefaultCommandHandler() == def_cmd_hdl);
    ASSERT_TRUE(test_extractor.getCommandFunctions() == cmd_fct);
}