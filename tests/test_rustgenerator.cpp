/**
 * \file test_rustgen.cpp
 * \brief Rust code generator unit test
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

#include "rustgenerator.h"
#include "test_data.h"

#define outputDir "gen_out/"

TEST(test_rustgen, gen_A_output) {
    RustGenerator test_rustgen;
    QString path = "gen_out/";
    QFile model_file, output_file;
    QStringList model_content, output_content;

    // Check raw A generation
    test_rustgen.generateMain(protocol_name, cmd_list, true, path);
    test_rustgen.generateBridge(protocol_name, protocol_id, cmd_list, true, path);

    // Check Main
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_main_a.rs"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "protocol_test.rs"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Bridge
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_bridge_a.rs"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "lcsf_protocol_test.rs"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();
}

TEST(test_rustgen, gen_B_output) {
    RustGenerator test_rustgen;
    QString path = "gen_out/";
    QFile model_file, output_file;
    QStringList model_content, output_content;

    // Check raw B generation
    test_rustgen.generateMain(protocol_name, cmd_list, false, path);
    test_rustgen.generateBridge(protocol_name, protocol_id, cmd_list, false, path);

    // Check Main
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_main_b.rs"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "protocol_test.rs"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Bridge
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_bridge_b.rs"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "lcsf_protocol_test.rs"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();
}