#include <gtest/gtest.h>
#include <QFile>

#include "codeextractor.h"
#include "codegenerator.h"
#include "data/generator_data.h"

#define modelDir "../tests/data/"
#define outputDir "gen_out/"

// Note: Test should be run from build/ for file paths to work

void openFile(QFile *pFile, QString filedir, QString fileName) {
    pFile->setFileName(filedir + fileName);

    if(!pFile->open(QIODevice::ReadOnly)) {
        FAIL() << " Couldn't open " + fileName.toStdString() + ": " + pFile->errorString().toStdString();
    }
}

QStringList readFileContent(QFile *pFile) {
    QTextStream file_content(pFile);
    QStringList output;

    while (!file_content.atEnd()) {
        QString line = file_content.readLine();
        output.append(line);
    }
    return output;
}

TEST(test_extractor, gen_raw_main) {
    CodeGenerator test_generator;
    CodeExtractor test_extractor;
    QString path = "gen_out/";
    QFile model_file, output_file;
    QStringList model_content, output_content;

    // Check raw generation
    test_generator.generateMainHeader(protocol_name, cmd_list, test_extractor, path);
    test_generator.generateMain(protocol_name, cmd_list, test_extractor, true, path);
    test_generator.generateBridgeHeader(protocol_name, protocol_id, cmd_list, path);
    test_generator.generateBridge(protocol_name, cmd_list, true, path);
    test_generator.generateDescription(protocol_name, cmd_list, path);

    // Check Main
    openFile(&model_file, modelDir, "model_raw_main.c");
    openFile(&output_file, outputDir, "Test_Main.c");
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    ASSERT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Main header
    openFile(&model_file, modelDir, "model_raw_main.h");
    openFile(&output_file, outputDir, "Test_Main.h");
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    ASSERT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Bridge
    openFile(&model_file, modelDir, "model_bridge.c");
    openFile(&output_file, outputDir, "LCSF_Bridge_Test.c");
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    ASSERT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Bridge header
    openFile(&model_file, modelDir, "model_bridge.h");
    openFile(&output_file, outputDir, "LCSF_Bridge_Test.h");
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    ASSERT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Desc
    openFile(&model_file, modelDir, "model_desc.h");
    openFile(&output_file, outputDir, "LCSF_Desc_Test.h");
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    ASSERT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check imported generation
    openFile(&model_file, modelDir, "model_main.c");
    QTextStream file_content(&model_file);
    test_extractor.extractFromSourceFile(protocol_name, &file_content, cmd_list);
    model_file.close();
    test_generator.generateMainHeader(protocol_name, cmd_list, test_extractor, path);
    test_generator.generateMain(protocol_name, cmd_list, test_extractor, true, path);
    test_generator.generateBridgeHeader(protocol_name, protocol_id, cmd_list, path);
    test_generator.generateBridge(protocol_name, cmd_list, true, path);
    test_generator.generateDescription(protocol_name, cmd_list, path);

    // Check Main
    openFile(&model_file, modelDir, "model_main.c");
    openFile(&output_file, outputDir, "Test_Main.c");
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    ASSERT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Main header
    openFile(&model_file, modelDir, "model_main.h");
    openFile(&output_file, outputDir, "Test_Main.h");
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    ASSERT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Bridge
    openFile(&model_file, modelDir, "model_bridge.c");
    openFile(&output_file, outputDir, "LCSF_Bridge_Test.c");
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    ASSERT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Bridge header
    openFile(&model_file, modelDir, "model_bridge.h");
    openFile(&output_file, outputDir, "LCSF_Bridge_Test.h");
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    ASSERT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();

    // Check Desc
    openFile(&model_file, modelDir, "model_desc.h");
    openFile(&output_file, outputDir, "LCSF_Desc_Test.h");
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    ASSERT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();
}