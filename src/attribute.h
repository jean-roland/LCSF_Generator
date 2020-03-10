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
    QList<Attribute *> m_subAttArray;
    QString m_desc;
    void *pObj;

public:
    Attribute(void);
    Attribute(QString attName, short attId, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType, QString attDesc);
    Attribute(QString attName, short attId, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType, QList<Attribute *> attSubAttArray, QString attDesc);
    void setName(QString attName);
    QString getName(void);
    void setId(short attId);
    short getId(void);
    void setIsOptional(bool attIsOptional);
    bool getIsOptional(void);
    void setDataType(NS_AttDataType::T_AttDataType attDataType);
    NS_AttDataType::T_AttDataType getDataType(void);
    void setDesc(QString attDesc);
    QString getDesc(void);
    void setObjPtr(void *pObj);
    void *getObjPtr(void);
    void setSubAttArray(QList<Attribute *> attSubAttArray);
    void addSubAtt(Attribute *newAtt);
    QList<Attribute *> getSubAttArray(void);
    QStringList getSubAttNamesList(void);
    void sortArrayById(void);
    void editParams(QString attName, short attCode, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType, QString attDesc);
    void clearArray_rec(void);
    void removeAtt(QString attName);
    Attribute *getSubAttPointer(QString subAttName);
};

#endif // ATTRIBUTE_H
