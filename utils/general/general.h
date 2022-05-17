#ifndef GENERAL_H
#define GENERAL_H

#include <QString>
#include <QVector>


enum Tag{
    COMMAND,
    SIMPLEPERF,
    XTS,
    SSH,
};

enum CallbackState{
    Success,
    Error,
    Unknow
};

struct COLOR{
    QString RED = QString("<font color=\'#ee0000\'>%1 </font>");
    QString GREEN = QString("<font color=\"#00aa00\">%1 </font>");
    QString BLUE = QString("<font color=\"#0000ee\">%1 </font>");
    QString BLACK = QString("<font color=\"#000000\">%1 </font>");
};

struct REPO{
    QString repoCmd;
};

struct BRANCH{
    QString branchName;
    REPO    repo;
};

typedef QString buildVer;
typedef QString buildCmd;
typedef QMap<buildVer,buildCmd> BUILD;


struct PROJECT{
    QString             projectName;
    QVector<BRANCH>     branch;
    QVector<BUILD>      build;
};

typedef void (*ptrFunc)(CallbackState);

#endif // GENERAL_H
