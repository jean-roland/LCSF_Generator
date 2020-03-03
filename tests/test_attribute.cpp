#include <gtest/gtest.h>
#include "attribute.h"

TEST(test_attribute, setget) {
    Attribute test_att;

    QString att_name("Attribute_Name");
    test_att.setName(att_name);
    ASSERT_EQ(test_att.getName(), att_name);

    short att_id = 12;
    test_att.setId(att_id);
    ASSERT_EQ(test_att.getId(), att_id);

    test_att.setIsOptional(true);
    ASSERT_TRUE(test_att.getIsOptional());
    test_att.setIsOptional(false);
    ASSERT_FALSE(test_att.getIsOptional());

    NS_AttDataType::T_AttDataType att_dt = NS_AttDataType::BYTE_ARRAY;
    test_att.setDataType(att_dt);
    ASSERT_EQ(test_att.getDataType(), att_dt);

    QString att_desc("Test description of an attribute.");
    test_att.setDesc(att_desc);
    ASSERT_EQ(test_att.getDesc(), att_desc);

    void *p_att_obj = (void *)0x12345678;
    test_att.setObjPtr(p_att_obj);
    ASSERT_EQ(test_att.getObjPtr(), p_att_obj);

    QList<Attribute *> att_subatt_array;
    test_att.setSubAttArray(att_subatt_array);
    ASSERT_EQ(test_att.getSubAttArray(), att_subatt_array);
}