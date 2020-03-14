#ifndef COMMAND_H
#define COMMAND_H
#include <QString>
#include "attribute.h"

class Command {
private:
    QString m_name;
    short m_id;
    bool m_hasAttributes;
    NS_DirectionType::T_DirectionType m_direction;
    QList<Attribute *> m_attArray;
    QString m_desc;

public:
    Command(void);
    Command(QString cmdName, short cmdCode, bool cmdHasAtt, NS_DirectionType::T_DirectionType cmdDirection, QString commandDesc);
    Command(QString cmdName, short cmdCode, bool cmdHasAtt, NS_DirectionType::T_DirectionType cmdDirection, QList<Attribute *> attArray, QString commandDesc);
    void setName(QString cmdName);
    QString getName(void);
    void setId(short cmdCode);
    short getId(void);
    void setHasAtt(bool cmdHasAtt);
    bool getHasAtt(void);
    void setDesc(QString cmdDesc);
    QString getDesc(void);
    void setDirection(NS_DirectionType::T_DirectionType cmdDirection);
    NS_DirectionType::T_DirectionType getDirection();
    bool isTransmittable(bool isA);
    bool isReceivable(bool isA);
    void setAttArray(QList<Attribute *> attArray);
    void addAttribute(Attribute *newAtt);
    QList<Attribute *> getAttArray(void);
    QStringList getAttNamesList(void);
    void sortAttArrayById(void);
    void editParameters(QString cmdName, short cmdCode, bool cmdHasAtt, NS_DirectionType::T_DirectionType cmdDirection , QString cmdDesc);
    void clearAttArray(void);
    void removeAttByName(QString attName);
    Attribute *getAttByName(QString cmdAttName);

    static Command *findCmdAddr(QString cmdName, QList<Command *> cmdList);
    static QList<Command *> sortListById(QList<Command *> cmdList);
    static int findCmdIdx(QString cmdName, QList<Command *> cmdList);
    static QStringList getListCmdNames(QList<Command *> cmdList);
};

#endif // COMMAND_H
