#ifndef GENERAL_H
#define GENERAL_H

#include <QMainWindow>
#include <QProcess>
#include <QString>
#include <QVector>

#include <functional>

typedef QProcess::ProcessState  ProcessState;
typedef QProcess::ExitStatus ExitStatus;

enum Tag{
    COMMAND,
    SIMPLEPERF,
    XTS,
    SSH,
};

/**
 * @brief The ProcessTag enum
 * <TAG>_<Process>
 * 每个tag下预留3个可处理的空间
 */
enum ProcessTag{
    COMMAND_    = 0,        //0
    COMMAND_SCREEN_RECORD,  //1
    COMMAND_2,              //2
    COMMAND_3,              //3
    SIMPLEPERF_ = 1<<2,     //4
    XTS_        = 2<<2,     //8
    XTS_FINDCTS,
    SSH_        = 3<<2,     //12
    SSH_LOGIN,              //13
    SSH_LOGOUT,
    SSH_FNINJA,
};

struct METADATA{
    ProcessTag      tag;
    QString         output;
    QString         error;
    ProcessState    state;
    ExitStatus      exitStatus;
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

#endif // GENERAL_H
