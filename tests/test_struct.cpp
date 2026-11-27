/**
 * \file test_struct.cpp
 * \brief Struct attribute unit test
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

#include <QDir>
#include <QFile>
#include <gtest/gtest.h>

#include "codeextractor.h"
#include "codegenerator.h"
#include "deschandler.h"
#include "docgenerator.h"
#include "rustgenerator.h"
#include "test_data.h"

#define outputDir "gen_out/"

static QList<Command *> struct_cmd_list;
static QList<Attribute::T_structType *> struct_list;
static QString struct_prot_name, struct_prot_id, struct_prot_version, struct_prot_desc;

static void loadStructDesc(void) {
    QFile desc_file;

    if (!struct_cmd_list.isEmpty()) {
        return;
    }
    ASSERT_TRUE(openFile(&desc_file, MODEL_DIR_PATH, "model_struct_desc.json"));
    DescHandler::load_desc(desc_file, struct_cmd_list, struct_list, struct_prot_name, struct_prot_id,
        struct_prot_version, struct_prot_desc);
    desc_file.close();
}

static void compareFiles(QString modelName, QString outputName) {
    QFile model_file, output_file;

    ASSERT_TRUE(openFile(&model_file, MODEL_DIR_PATH, modelName));
    ASSERT_TRUE(openFile(&output_file, outputDir, outputName));
    QStringList model_content = readFileContent(&model_file);
    QStringList output_content = readFileContent(&output_file);
    EXPECT_EQ(output_content.count(), model_content.count()) << outputName.toStdString();
    for (int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString())
            << outputName.toStdString() << ":" << idx;
    }
    model_file.close();
    output_file.close();
}

TEST(test_struct, desc_round_trip) {
    loadStructDesc();
    ASSERT_EQ(struct_list.size(), 3);
    ASSERT_TRUE(Command::checkStructs(struct_cmd_list).isEmpty());
    ASSERT_EQ(Attribute::getStructSize(struct_list.at(0)), 38);
    ASSERT_EQ(Attribute::getStructSize(struct_list.at(2)), 37); // Variable-length field maximums
    ASSERT_EQ(Command::getMaxStructBuffSize(struct_cmd_list), 113);
    EXPECT_EQ(Attribute::getStructFieldOffsets(struct_list.at(2)).join(" ").toStdString(), "0 - - - - - -");
    EXPECT_EQ(Command::getStructAttNames(struct_cmd_list, struct_list.at(0)).join(", ").toStdString(),
        "get_status.status, get_status.opt_status, get_status.ext_status, cfg.opt_status");
    // Test order is not defined, a generator test may not have created it yet
    ASSERT_TRUE(QDir().mkpath(outputDir));
    ASSERT_TRUE(DescHandler::save_desc(outputDir "StructTest_desc.json", struct_cmd_list, struct_list, struct_prot_name,
        struct_prot_id, struct_prot_version, struct_prot_desc));
    compareFiles("model_struct_desc.json", "StructTest_desc.json");
}

TEST(test_struct, check_structs) {
    Attribute::T_structType empty = {"empty", "", true, {}};
    Attribute::T_structType big = {"big", "", true, {}};
    for (int idx = 0; idx < 32; idx++) {
        big.fields.append({"f" + QString::number(idx), NS_FieldType::UINT64, ""});
    }
    Attribute att("att", 0, false, NS_AttDataType::STRUCT, "");
    Command cmd("cmd", 0, true, NS_DirectionType::A_TO_B, "");
    cmd.addAttribute(&att);

    EXPECT_FALSE(Command::checkStructs({&cmd}).isEmpty()); // no struct type
    att.setStruct(&empty);
    EXPECT_FALSE(Command::checkStructs({&cmd}).isEmpty()); // no field
    empty.fields.append({"f", NS_FieldType::UNKNOWN, ""});
    EXPECT_FALSE(Command::checkStructs({&cmd}).isEmpty()); // unknown field type
    Attribute::T_structType var = {"var", "", false, {{"v", NS_FieldType::VARUINT32, ""}}};
    att.setStruct(&var);
    att.setIsArray(true);
    EXPECT_FALSE(Command::checkStructs({&cmd}).isEmpty()); // variable-length field in a non extensible struct
    var.isExtensible = true;
    EXPECT_TRUE(Command::checkStructs({&cmd}).isEmpty());
    att.setStruct(&big);
    EXPECT_FALSE(Command::checkStructs({&cmd}).isEmpty()); // extensible over 255 bytes
    big.isExtensible = false;
    EXPECT_TRUE(Command::checkStructs({&cmd}).isEmpty());

    // Generated names are lower case
    big.fields.append({"F0", NS_FieldType::UINT8, ""});
    EXPECT_FALSE(Command::checkStructs({&cmd}).isEmpty()); // field name differs by case only
    big.fields.removeLast();
    Attribute::T_structType bigUpper = {"BIG", "", false, big.fields};
    Attribute att2("att2", 1, false, NS_AttDataType::STRUCT, "");
    att2.setStruct(&bigUpper);
    cmd.addAttribute(&att2);
    EXPECT_FALSE(Command::checkStructs({&cmd}).isEmpty()); // struct name differs by case only
}

TEST(test_struct, gen_output) {
    CodeGenerator code_gen;
    RustGenerator rust_gen;
    DocGenerator doc_gen;
    QString path = outputDir;

    loadStructDesc();
    code_gen.generateMainHeader(struct_prot_name, struct_cmd_list, CodeExtractor(), path);
    code_gen.generateBridgeHeader(struct_prot_name, struct_prot_id, struct_prot_version, struct_cmd_list, path);
    code_gen.generateDescription(struct_prot_name, struct_cmd_list, path);
    for (bool isA : {true, false}) {
        QString side = isA ? "a" : "b";
        code_gen.generateMain(struct_prot_name, struct_cmd_list, CodeExtractor(), isA, path);
        code_gen.generateBridge(struct_prot_name, struct_cmd_list, isA, path);
        rust_gen.generateMain(struct_prot_name, struct_cmd_list, isA, path);
        rust_gen.generateBridge(struct_prot_name, struct_prot_id, struct_prot_version, struct_cmd_list, isA, path);
        compareFiles("model_struct_main_" + side + ".c", "StructTest_Main_" + side + ".c");
        compareFiles("model_struct_bridge_" + side + ".c", "LCSF_Bridge_StructTest_" + side + ".c");
        compareFiles("model_struct_main_" + side + ".rs", "protocol_structtest_" + side + ".rs");
        compareFiles("model_struct_bridge_" + side + ".rs", "lcsf_protocol_structtest_" + side + ".rs");
    }
    doc_gen.generateWikiTable(struct_prot_name, struct_prot_id, struct_prot_desc, struct_cmd_list, path);
    doc_gen.generateDokuWikiTable(struct_prot_name, struct_prot_id, struct_prot_desc, struct_cmd_list, path);
    doc_gen.generateMkdownTable(struct_prot_name, struct_prot_id, struct_prot_desc, struct_cmd_list, path);
    compareFiles("model_struct_main.h", "StructTest_Main.h");
    compareFiles("model_struct_bridge.h", "LCSF_Bridge_StructTest.h");
    compareFiles("model_struct_desc.c", "LCSF_Desc_StructTest.c");
    compareFiles("model_struct_wiki.txt", "LCSF_StructTest_WikiTables.txt");
    compareFiles("model_struct_dokuwiki.txt", "LCSF_StructTest_DokuWikiTables.txt");
    compareFiles("model_struct_mkdown.md", "LCSF_StructTest_MkdownTables.md");
}
