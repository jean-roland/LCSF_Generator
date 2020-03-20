#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <QFile>
#include <QTextStream>

#include "deschandler.h"
#include "test_data.h"

// Note: Test should be run from build/ for file paths to work
#define modelDir "../tests/data/"
#define outputDir "gen_out/"

static QList<Command *> load_cmd_list;
static QString load_prot_name;
static QString load_prot_id;

static bool compare_cmd_list(QList<Command *> model_list, QList<Command *> out_list) {
    int err_val;
    bool val = Command::compareRefCmdList(model_list, out_list, err_val);

    if (!val) {
        if (err_val < 0) {
             std::cout << "Command lists sizes are different" << endl;
        } else {
            std::cout << "Commands are different at index: " << std::to_string(err_val) << "\n" << endl;
        }
    }
    return val;
}

TEST(test_deschandler, load_desc) {
    QFile desc_file;

    // Load model
    ASSERT_TRUE(openFile(&desc_file, modelDir, "model_desc.json"));
    DescHandler::load_desc(desc_file, load_cmd_list, load_prot_name, load_prot_id);
    desc_file.close();

    // Compared loaded data with model data
    ASSERT_EQ(protocol_name, load_prot_name);
    ASSERT_EQ(protocol_id, load_prot_id);
    ASSERT_TRUE(compare_cmd_list(cmd_list, load_cmd_list));
}

TEST(test_deschandler, save_desc) {
    QString path = "gen_out/";
    QString filename = path + "Test_desc";
    QFile model_file, output_file;
    QStringList model_content, output_content;

    // Generate desc
    ASSERT_TRUE(DescHandler::save_desc(filename, cmd_list, protocol_name, protocol_id));

    // Check desc file
    ASSERT_TRUE(openFile(&model_file, modelDir, "model_desc.json"));
    ASSERT_TRUE(openFile(&output_file, outputDir, "Test_desc.json"));
    model_content = readFileContent(&model_file);
    output_content = readFileContent(&output_file);
    ASSERT_EQ(output_content.count(), model_content.count());
    for(int idx = 0; idx < output_content.count(); idx++) {
        ASSERT_EQ(model_content.at(idx).toStdString(), output_content.at(idx).toStdString()) << idx;
    }
    model_file.close();
    output_file.close();
}