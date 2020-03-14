#include <gtest/gtest.h>
#include "command.h"

TEST(test_command, setget) {
    Command test_cmd;

    // Check default values
    ASSERT_EQ(test_cmd.getName(), "default");
    ASSERT_EQ(test_cmd.getId(), 0);
    ASSERT_FALSE(test_cmd.getHasAtt());
    ASSERT_EQ(test_cmd.getDesc(), QString());
    ASSERT_EQ(test_cmd.getAttArray(), QList<Attribute *>());

    // Check setters/getters
    QString cmd_name("Command_Name");
    test_cmd.setName(cmd_name);
    ASSERT_EQ(test_cmd.getName(), cmd_name);

    short cmd_id = 12;
    test_cmd.setId(cmd_id);
    ASSERT_EQ(test_cmd.getId(), cmd_id);

    test_cmd.setHasAtt(true);
    ASSERT_TRUE(test_cmd.getHasAtt());
    test_cmd.setHasAtt(false);
    ASSERT_FALSE(test_cmd.getHasAtt());

    NS_DirectionType::T_DirectionType cmd_dir = NS_DirectionType::BIDIRECTIONAL;
    test_cmd.setDirection(cmd_dir);
    ASSERT_EQ(test_cmd.getDirection(), cmd_dir);

    QString cmd_desc("Test description of a Command.");
    test_cmd.setDesc(cmd_desc);
    ASSERT_EQ(test_cmd.getDesc(), cmd_desc);

    Attribute test_att;
    QList<Attribute *> cmd_array = {&test_att};
    test_cmd.setAttArray(cmd_array);
    ASSERT_EQ(test_cmd.getAttArray(), cmd_array);

    // Check edit params
    cmd_name = "Another_name";
    cmd_id = 28;
    cmd_dir = NS_DirectionType::UNKNOWN;
    cmd_desc = "Another description";
    test_cmd.editParameters(cmd_name, cmd_id, true, cmd_dir, cmd_desc);
    ASSERT_EQ(test_cmd.getName(), cmd_name);
    ASSERT_EQ(test_cmd.getId(), cmd_id);
    ASSERT_TRUE(test_cmd.getHasAtt());
    ASSERT_EQ(test_cmd.getDirection(), cmd_dir);
    ASSERT_EQ(test_cmd.getDesc(), cmd_desc);
}

TEST(test_command, cmd_directions) {
    Command test_cmd;

    // Check bidirectional
    test_cmd.setDirection(NS_DirectionType::BIDIRECTIONAL);

    ASSERT_TRUE(test_cmd.isTransmittable(true));
    ASSERT_TRUE(test_cmd.isReceivable(true));
    ASSERT_TRUE(test_cmd.isTransmittable(false));
    ASSERT_TRUE(test_cmd.isReceivable(false));

    // Check A->B
    test_cmd.setDirection(NS_DirectionType::A_TO_B);

    ASSERT_TRUE(test_cmd.isTransmittable(true));
    ASSERT_FALSE(test_cmd.isReceivable(true));
    ASSERT_FALSE(test_cmd.isTransmittable(false));
    ASSERT_TRUE(test_cmd.isReceivable(false));

    // Check B->A
    test_cmd.setDirection(NS_DirectionType::B_TO_A);

    ASSERT_FALSE(test_cmd.isTransmittable(true));
    ASSERT_TRUE(test_cmd.isReceivable(true));
    ASSERT_TRUE(test_cmd.isTransmittable(false));
    ASSERT_FALSE(test_cmd.isReceivable(false));
}

TEST(test_command, cmd_att) {
    Command test_cmd;

    // Check add attribute
    Attribute *test_att0 = new Attribute("sub_att0", 10, false, NS_AttDataType::SUB_ATTRIBUTES, "This is a sub-attribute");
    Attribute *test_att1 = new Attribute("sub_att1", 11, false, NS_AttDataType::UINT8, "This is a sub-attribute");
    Attribute *test_att2 = new Attribute("sub_att2", 12, false, NS_AttDataType::UINT16, "This is a sub-attribute");
    Attribute *test_att3 = new Attribute("sub_att3", 13, false, NS_AttDataType::UINT32, "This is a sub-attribute");
    Attribute *test_att4 = new Attribute("sub_att4", 14, false, NS_AttDataType::STRING, "This is a sub-attribute");
    test_att0->addSubAtt(test_att4);
    QList<Attribute *> att_array = {test_att1, test_att0, test_att3, test_att2};
    test_cmd.addAttribute(test_att1);
    test_cmd.addAttribute(test_att0);
    test_cmd.addAttribute(test_att3);
    test_cmd.addAttribute(test_att2);
    ASSERT_EQ(test_cmd.getAttArray(), att_array);

    // Check getAttNamesList
    QStringList att_names = {test_att1->getName(), test_att0->getName(),
     test_att3->getName(), test_att2->getName()};
    ASSERT_EQ(test_cmd.getAttNamesList(), att_names);

    // Check sortAttArrayById
    att_array = {test_att0, test_att1, test_att2, test_att3};
    test_cmd.sortAttArrayById();
    ASSERT_EQ(test_cmd.getAttArray(), att_array);

    // Check getAttByName
    ASSERT_EQ(test_cmd.getAttByName(test_att2->getName()), test_att2);

    // Check removeAttByName
    att_array = {test_att0, test_att1, test_att3};
    test_cmd.removeAttByName(test_att2->getName());
    ASSERT_EQ(test_cmd.getAttArray(), att_array);

    // Check clearAttArray
    test_cmd.clearAttArray();
    ASSERT_EQ(test_cmd.getAttArray(), QList<Attribute *>());
}

TEST(test_command, cmd_static) {
    Command test_cmd0("cmd0", 10, false, NS_DirectionType::BIDIRECTIONAL, "Test description");
    Command test_cmd1("cmd1", 11, false, NS_DirectionType::BIDIRECTIONAL, "Test description");
    Command test_cmd2("cmd2", 12, false, NS_DirectionType::BIDIRECTIONAL, "Test description");
    Command test_cmd3("cmd3", 13, false, NS_DirectionType::BIDIRECTIONAL, "Test description");
    Command test_cmd4("cmd4", 14, false, NS_DirectionType::BIDIRECTIONAL, "Test description");
    QList<Command *> cmd_array = {&test_cmd1, &test_cmd3, &test_cmd0, &test_cmd4, &test_cmd2};

    // Check findCmdAddr
    ASSERT_EQ(Command::findCmdAddr(test_cmd2.getName(), cmd_array), &test_cmd2);

    // Check findCmdIdx
    ASSERT_EQ(Command::findCmdIdx(test_cmd2.getName(), cmd_array), cmd_array.indexOf(&test_cmd2));

    // Check getListCmdNames
    QStringList cmd_names = {test_cmd1.getName(), test_cmd3.getName(),
     test_cmd0.getName(), test_cmd4.getName(), test_cmd2.getName()};
    ASSERT_EQ(Command::getListCmdNames(cmd_array), cmd_names);

    // Check sortListById
    QList<Command *> sorted_array = {&test_cmd0, &test_cmd1, &test_cmd2, &test_cmd3, &test_cmd4};
    ASSERT_EQ(Command::sortListById(cmd_array), sorted_array);
}