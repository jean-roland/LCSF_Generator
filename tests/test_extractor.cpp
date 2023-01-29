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
    ASSERT_EQ(test_extractor.getUnknownIncludes().toStdString(), a_unk_inc.toStdString());
    ASSERT_EQ(test_extractor.getUnknownDefinitions().toStdString(), a_unk_def.toStdString());
    ASSERT_EQ(test_extractor.getUnknownPrivateFunctions().toStdString(),a_unk_pri_fct.toStdString());
    ASSERT_EQ(test_extractor.getUnknownPublicFunctions().toStdString(), a_unk_pub_fct.toStdString());
    ASSERT_EQ(test_extractor.getUnknownPublicFunctionsHeaders().toStdString(), a_unk_pub_hdr.toStdString());
    ASSERT_EQ(test_extractor.getDefaultCommandHandler().toStdString(), a_def_cmd_hdl.toStdString());

    QStringList cmdFcnList = test_extractor.getCommandFunctions();
    for(int idx = 0; idx < b_cmd_fct.count(); idx++) {
        ASSERT_EQ(cmdFcnList.at(idx).toStdString(), a_cmd_fct.at(idx).toStdString());
    }
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
    ASSERT_EQ(test_extractor.getUnknownIncludes().toStdString(), b_unk_inc.toStdString());
    ASSERT_EQ(test_extractor.getUnknownDefinitions().toStdString(), b_unk_def.toStdString());
    ASSERT_EQ(test_extractor.getUnknownPrivateFunctions().toStdString(), b_unk_pri_fct.toStdString());
    ASSERT_EQ(test_extractor.getUnknownPublicFunctions().toStdString(), b_unk_pub_fct.toStdString());
    ASSERT_EQ(test_extractor.getUnknownPublicFunctionsHeaders().toStdString(), b_unk_pub_hdr.toStdString());
    ASSERT_EQ(test_extractor.getDefaultCommandHandler().toStdString(), b_def_cmd_hdl.toStdString());

    QStringList cmdFcnList = test_extractor.getCommandFunctions();
    for(int idx = 0; idx < b_cmd_fct.count(); idx++) {
        ASSERT_EQ(cmdFcnList.at(idx).toStdString(), b_cmd_fct.at(idx).toStdString());
    }
}