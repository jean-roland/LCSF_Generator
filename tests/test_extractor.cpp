/**
 * \file test_extractor.cpp
 * \brief Code extractor unit test
 * \author Jean-Roland Gosse

    This file is part of LCSF Generator.

    LCSF Generator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LCSF Generator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this project. If not, see <https://www.gnu.org/licenses/>
 */

#include <gtest/gtest.h>
#include <QFile>
#include "codeextractor.h"
#include "test_data.h"

TEST(test_extractor, a_extraction) {
    // Get model file data
    QFile file(MODEL_MAIN_A_PATH);

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
    ASSERT_TRUE(test_extractor.getUnknownIncludes() == a_unk_inc);
    ASSERT_TRUE(test_extractor.getUnknownDefinitions() == a_unk_def);
    ASSERT_TRUE(test_extractor.getUnknownPrivateFunctions() == a_unk_pri_fct);
    ASSERT_TRUE(test_extractor.getUnknownPublicFunctions() == a_unk_pub_fct);
    ASSERT_TRUE(test_extractor.getUnknownPublicFunctionsHeaders() == a_unk_pub_hdr);
    ASSERT_TRUE(test_extractor.getDefaultCommandHandler() == a_def_cmd_hdl);
    ASSERT_TRUE(test_extractor.getCommandFunctions() == a_cmd_fct);
}

TEST(test_extractor, b_extraction) {
    // Get model file data
    QFile file(MODEL_MAIN_B_PATH); // Note: Test should be run from build/ for this path to work

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
    ASSERT_TRUE(test_extractor.getUnknownIncludes() == b_unk_inc);
    ASSERT_TRUE(test_extractor.getUnknownDefinitions() == b_unk_def);
    ASSERT_TRUE(test_extractor.getUnknownPrivateFunctions() == b_unk_pri_fct);
    ASSERT_TRUE(test_extractor.getUnknownPublicFunctions() == b_unk_pub_fct);
    ASSERT_TRUE(test_extractor.getUnknownPublicFunctionsHeaders() == b_unk_pub_hdr);
    ASSERT_TRUE(test_extractor.getDefaultCommandHandler() == b_def_cmd_hdl);
    ASSERT_TRUE(test_extractor.getCommandFunctions() == b_cmd_fct);
}