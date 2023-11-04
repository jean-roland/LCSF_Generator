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

#include "docgenerator.h"
#include "test_data.h"

#define outputDir "gen_out/"

TEST(test_docgenerator, doc_output) {
    DocGenerator test_generator;
    QString path = "gen_out/";
    QFile model_file, output_file;
    QStringList model_content, output_content;

    // Generate doc
    test_generator.generateWikiTable(protocol_name, protocol_id, cmd_list, path);
    test_generator.generateDokuWikiTable(protocol_name, protocol_id, cmd_list, path);
    test_generator.generateMkdownTable(protocol_name, protocol_id, cmd_list, path);

    // Check wiki
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_wiki.txt"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "LCSF_Test_WikiTables.txt"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check dokuwiki
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_dokuwiki.txt"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "LCSF_Test_DokuWikiTables.txt"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check mkdown
    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, "model_mkdown.md"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "LCSF_Test_MkdownTables.md"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();
}