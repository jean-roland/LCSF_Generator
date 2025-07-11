/**
 * \file test_generator.cpp
 * \brief Code generator unit test
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
#include "codegenerator.h"
#include "test_data.h"

#define outputDir "gen_out/"

TEST(test_generator, gen_A_output) {
    CodeGenerator test_generator;
    CodeExtractor test_extractor;
    QString path = "gen_out/";
    QFile model_file, output_file;
    QStringList model_content, output_content;

    // Check raw A generation
    test_generator.generateMainHeader(protocol_name, cmd_list, test_extractor, path);
    test_generator.generateMain(protocol_name, cmd_list, test_extractor, true, path);
    test_generator.generateBridgeHeader(protocol_name, protocol_id, cmd_list, path);
    test_generator.generateBridge(protocol_name, cmd_list, true, path);
    test_generator.generateDescription(protocol_name, cmd_list, path);

    // Check Main
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_raw_main_a.c"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "Test_Main_a.c"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Main header
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_raw_main.h"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "Test_Main.h"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Bridge
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_bridge_a.c"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "LCSF_Bridge_Test_a.c"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Bridge header
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_bridge.h"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "LCSF_Bridge_Test.h"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Desc
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_desc.c"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "LCSF_Desc_Test.c"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check imported A generation
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_main_a.c"));
    QTextStream file_content(&model_file);
    test_extractor.extractFromSourceFile(protocol_name, &file_content, cmd_list);
    model_file.close();
    test_generator.generateMainHeader(protocol_name, cmd_list, test_extractor, path);
    test_generator.generateMain(protocol_name, cmd_list, test_extractor, true, path);
    test_generator.generateBridgeHeader(protocol_name, protocol_id, cmd_list, path);
    test_generator.generateBridge(protocol_name, cmd_list, true, path);
    test_generator.generateDescription(protocol_name, cmd_list, path);

    // Check Main
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_main_a.c"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "Test_Main_a.c"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Main header
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_main.h"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "Test_Main.h"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Bridge
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_bridge_a.c"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "LCSF_Bridge_Test_a.c"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Bridge header
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_bridge.h"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "LCSF_Bridge_Test.h"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Desc
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_desc.c"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "LCSF_Desc_Test.c"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();
}

TEST(test_generator, gen_B_output) {
    CodeGenerator test_generator;
    CodeExtractor test_extractor;
    QString path = "gen_out/";
    QFile model_file, output_file;
    QStringList model_content, output_content;

    // Check raw B generation
    test_generator.generateMainHeader(protocol_name, cmd_list, test_extractor, path);
    test_generator.generateMain(protocol_name, cmd_list, test_extractor, false, path);
    test_generator.generateBridgeHeader(protocol_name, protocol_id, cmd_list, path);
    test_generator.generateBridge(protocol_name, cmd_list, false, path);
    test_generator.generateDescription(protocol_name, cmd_list, path);

    // Check Main
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_raw_main_b.c"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "Test_Main_b.c"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Bridge
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_bridge_b.c"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "LCSF_Bridge_Test_b.c"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check imported B generation
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_main_b.c"));
    QTextStream file_content(&model_file);
    test_extractor.extractFromSourceFile(protocol_name, &file_content, cmd_list);
    model_file.close();
    test_generator.generateMainHeader(protocol_name, cmd_list, test_extractor, path);
    test_generator.generateMain(protocol_name, cmd_list, test_extractor, false, path);

    // Check Main
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_main_b.c"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "Test_Main_b.c"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < model_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Main header
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_main.h"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "Test_Main.h"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();
}