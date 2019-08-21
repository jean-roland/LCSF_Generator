#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include <QString>
#include <QList>
#include "enumtype.h"

class Attribute {
private:
    QString m_name;
    short m_id;
    bool m_isOptional;
    NS_AttDataType::T_AttDataType m_dataType;
    QList<Attribute*> m_subAttArray;
    QString m_desc;

public:
    Attribute();
    Attribute(QString attName, short attId, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType, QString attDesc);
    Attribute(QString attName, short attId, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType, QList<Attribute *> attSubAttArray, QString attDesc);
    void setName(QString attName);
    QString getName();
    void setId(short attId);
    short getId();
    void setIsOptional(bool attIsOptional);
    bool getIsOptional();
    void setDataType(NS_AttDataType::T_AttDataType attDataType);
    NS_AttDataType::T_AttDataType getDataType();
    void setDesc(QString attDesc);
    QString getDesc();
    void setSubAttArray(QList<Attribute*> attSubAttArray);
    void addSubAtt(Attribute * newAtt);
    QList<Attribute*> getSubAttArray();
    QStringList getSubtAttNamesList();
    void sortArrayById();
    void editParams(QString attName, short attCode, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType, QString attDesc);
    void clearArray_rec();
    void removeAtt(QString attName);
    Attribute * getSubAttPointer(QString subAttName);
};

#endif // ATTRIBUTE_H
