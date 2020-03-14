#include <gtest/gtest.h>
#include "attribute.h"

TEST(test_attribute, setget) {
    Attribute test_att;

    // Check default values
    ASSERT_EQ(test_att.getName(), "default");
    ASSERT_EQ(test_att.getId(), 0);
    ASSERT_TRUE(test_att.getIsOptional());
    ASSERT_EQ(test_att.getDataType(), NS_AttDataType::UNKNOWN);
    ASSERT_EQ(test_att.getDesc(), QString());
    ASSERT_EQ(test_att.getSubAttArray(), QList<Attribute *>());

    // Check setters/getters
    QString att_name("Attribute_Name");
    test_att.setName(att_name);
    ASSERT_EQ(test_att.getName(), att_name);

    short att_id = 12;
    test_att.setId(att_id);
    ASSERT_EQ(test_att.getId(), att_id);

    test_att.setIsOptional(false);
    ASSERT_FALSE(test_att.getIsOptional());
    test_att.setIsOptional(true);
    ASSERT_TRUE(test_att.getIsOptional());

    NS_AttDataType::T_AttDataType att_dt = NS_AttDataType::BYTE_ARRAY;
    test_att.setDataType(att_dt);
    ASSERT_EQ(test_att.getDataType(), att_dt);

    QString att_desc("Test description of an attribute.");
    test_att.setDesc(att_desc);
    ASSERT_EQ(test_att.getDesc(), att_desc);

    void *p_att_obj = (void *)0x12345678;
    test_att.setObjPtr(p_att_obj);
    ASSERT_EQ(test_att.getObjPtr(), p_att_obj);

    QList<Attribute *> att_subatt_array = {&test_att};
    test_att.setSubAttArray(att_subatt_array);
    ASSERT_EQ(test_att.getSubAttArray(), att_subatt_array);

    // Check edit params
    att_name = "Other_Name";
    att_id = 62;
    att_dt = NS_AttDataType::UINT16;
    att_desc = "Another description";
    test_att.editParams(att_name, att_id, true, att_dt, att_desc);
    ASSERT_EQ(test_att.getName(), att_name);
    ASSERT_EQ(test_att.getId(), att_id);
    ASSERT_TRUE(test_att.getIsOptional());
    ASSERT_EQ(test_att.getDataType(), att_dt);
    ASSERT_EQ(test_att.getDesc(), att_desc);
}

TEST(test_attribute, subatt) {
    Attribute test_att("Test_att", 01, false, NS_AttDataType::SUB_ATTRIBUTES, "This is a test attribute");
    Attribute *sub_att0 = new Attribute("sub_att0", 10, false, NS_AttDataType::SUB_ATTRIBUTES, "This is a sub-attribute");
    Attribute *sub_att1 = new Attribute("sub_att1", 11, true, NS_AttDataType::UINT8, "This is a sub-attribute");
    Attribute *sub_att2 = new Attribute("sub_att2", 12, false, NS_AttDataType::UINT16, "This is a sub-attribute");
    Attribute *sub_att3 = new Attribute("sub_att3", 13, false, NS_AttDataType::UINT32, "This is a sub-attribute");
    Attribute *sub_att4 = new Attribute("sub_att4", 14, true, NS_AttDataType::BYTE_ARRAY, "This is a sub-attribute");

    // Check add/remove att
    sub_att0->addSubAtt(sub_att4);
    ASSERT_EQ(sub_att0->getSubAttArray().first(), sub_att4);
    ASSERT_EQ(sub_att0->getSubAttArray().count(), 1);
    sub_att0->removeAtt(sub_att4->getName());
    ASSERT_EQ(sub_att0->getSubAttArray().count(), 0);

    // Check sortArrayById
    QList<Attribute *> subAttArray = {sub_att2, sub_att1, sub_att3, sub_att0};
    test_att.setSubAttArray(subAttArray);
    test_att.sortArrayById();
    subAttArray = {sub_att0, sub_att1, sub_att2, sub_att3};
    ASSERT_EQ(test_att.getSubAttArray(), subAttArray);

    // Check getSubAttNamesList
    QStringList subAttNames = {sub_att0->getName(), sub_att1->getName(),
     sub_att2->getName(), sub_att3->getName()};
    ASSERT_EQ(test_att.getSubAttNamesList(), subAttNames);

    // Check getSubAttPointer
    ASSERT_EQ(test_att.getSubAttPointer(sub_att3->getName()), sub_att3);

    // Check clearArray_rec
    test_att.clearArray_rec();
    ASSERT_EQ(test_att.getSubAttArray(), QList<Attribute *>());
}