#pragma once

#include "ui_cattle.h"

#include <vector>

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtWidgets/QWidget>

using std::vector;

typedef struct PatchEntity {
    QString id;
    QString author;
    QString time;
    QString filepath;

    PatchEntity(const QString &id, 
                const QString author, 
                const QString &filePath)
    {
        this->id        = id;
        this->author    = author;
        this->filepath  = filePath;
    }

} PatchEntity;

typedef struct CommitEntity {
    QString id;
    QString author;
    QString time;
    // 提交注释
    QString comment;

    CommitEntity(const QString &content)
    {
        QStringList content_list = content.split(",");

        if (content_list.size() >= 4) { 
            id       = content_list[0];
            author   = content_list[1];
            time     = content_list[2];
            comment  = content_list[3];
        }
    }

    void operator<<(const QString &content)
    {
        QStringList content_list = content.split(",");

        if (content_list.size() < 4) {
            // throw std::logic_error("content size error, must be over 4.");
            return;
        }

        id       = content_list[0];
        author   = content_list[1];
        time     = content_list[2];
        comment  = content_list[3];
    }

} CommitEntity;

typedef vector<CommitEntity>::iterator CommitIterator;
typedef vector<PatchEntity>::iterator PatchIterator;

class Cattle : public QWidget
{
    Q_OBJECT

public:
    Cattle(QWidget *parent = Q_NULLPTR);

private:
    bool FindCSVFile();
    bool FindPatchFile();
    int FindCommitIDRow(QTableWidget *tableWidget, const QString &commitID);
    void TravserFile(const QString &filePath, const QString &fileSuffix);
    void ReadCSVFile();
    void ReadPatchFile();
    void InsertCommitTable();
    void InsertPatchTable();
    // 1.先使用Tortoise Diff工具打开
    // 2.上面的不存在，使用notepad++ 打开
    // 3.上面的不存在，使用notepad打开
    void OpenPatchFile(const QString &patchFile);
    void OpenPatchFile(const QString &commitID, int);
    void InitTextBrowser();

protected slots:
    void on_table_commit_log_cellClicked(int row, int column);
    void on_table_commit_log_cellDoubleClicked(int row, int column);
    void on_table_patch_cellDoubleClicked(int row, int column);

private:
    Ui::cattleClass ui;

    QFileInfo mCsvFile;
    vector<CommitEntity> mCommitVec;
    vector<PatchEntity> mPatchVec;
    vector<QString> mPatchPathVec;
};
