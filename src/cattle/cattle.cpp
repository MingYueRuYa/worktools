#include "cattle.h"
#include "common.h"
#include "dolproductinfo.h"

#include <algorithm>

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QSettings>

#include <Windows.h>

const QString kCSV_FILE_SUFFIX      = "csv";
const QString kPATCH_FILE_SUFFIX    = "patch";
const int kFONT_SIZE                = 3;

struct JudgeCommitID : public std::binary_function<QString, QString ,bool>
{
    bool operator()(const QString &rhs, const QString &lhs) {
        if (rhs.length() < lhs.length()) {
            return lhs.contains(rhs);
        } else {
            return rhs.contains(lhs);
        }
    }
};

Cattle::Cattle(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);

    mCsvFile = QFileInfo();

    mCommitVec.reserve(1024);
    mPatchVec.reserve(1024);

    InitTextBrowser();

    FindCSVFile();
    ReadCSVFile();

    FindPatchFile();
    ReadPatchFile();
}

bool Cattle::FindCSVFile() {
    TravserFile(qApp->applicationDirPath(), kCSV_FILE_SUFFIX);

    return mCsvFile.exists();
}

bool Cattle::FindPatchFile() {
    TravserFile(qApp->applicationDirPath(), kPATCH_FILE_SUFFIX);

    return mCsvFile.exists();
}

int Cattle::FindCommitIDRow(QTableWidget *tableWidget, 
                                const QString &commitID) {
    if (commitID.isEmpty() || nullptr == tableWidget) { return -1; }

    for (int i = 0; i < tableWidget->rowCount(); ++i) {
        if (JudgeCommitID()(commitID, tableWidget->item(i, 0)->text())) { 
            return i; 
        }
    }

    return -1;
}

void Cattle::TravserFile(const QString &filePath, const QString &fileSuffix) {
    QDir dir(filePath);
    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | 
                                            QDir::Files, 
                                            QDir::Name);
    if (dirs.isEmpty()) { return; }

    QFileInfoList::Iterator ibeg = dirs.begin();
    while(ibeg != dirs.end()) {
        if (ibeg->isDir()) {
            TravserFile(ibeg->absoluteFilePath(), fileSuffix);
        } else if (ibeg->isFile()) {
            if (fileSuffix == ibeg->suffix() && 
                kCSV_FILE_SUFFIX == fileSuffix) {
                mCsvFile = *ibeg;
                return;
            } else if (fileSuffix == ibeg->suffix() && 
                        kPATCH_FILE_SUFFIX == fileSuffix) {
                mPatchPathVec.push_back(ibeg->absoluteFilePath());
            }
        }
        ++ibeg;
    }
}

void Cattle::ReadCSVFile() {
    if (! mCsvFile.exists()) {
        ui.textbrowser->append(WrapHtmlStr("red", "csv file is invalide.", kFONT_SIZE));
        return;
    }

    QFile csv_file(mCsvFile.absoluteFilePath());
    if (! csv_file.open(QIODevice::ReadOnly)) {
        ui.textbrowser->append(WrapHtmlStr("red", 
                                    "Open file is invalide.", 
                                    kFONT_SIZE));
        return;
    }

    while (! csv_file.atEnd()) {
        QString content = csv_file.readLine();
        mCommitVec.emplace_back((CommitEntity(content)));
    }

    csv_file.close();

    InsertCommitTable();
}

void Cattle::ReadPatchFile()
{
    for (int i = 0; i < mPatchPathVec.size(); ++i) {
        QFile file(mPatchPathVec[i]); 
        if (! file.open(QIODevice::ReadOnly)) { continue; }

        QString content = file.readLine();
        QString id = content.split(" ")[1];

        content = file.readLine();
        QString author = content.split(" ")[1];

        mPatchVec.push_back(PatchEntity(id, author, mPatchPathVec[i]));
        file.close();
    }

    InsertPatchTable();
}

void Cattle::InsertCommitTable() {
    while (ui.table_commit_log->rowCount() > 0) {
        ui.table_commit_log->removeRow(0);
    }

    std::for_each(mCommitVec.begin(), mCommitVec.end(), 
        [this] (const CommitEntity &entity) {
        int row_count = ui.table_commit_log->rowCount();
        ui.table_commit_log->insertRow(row_count);
        ui.table_commit_log->setItem(row_count, 0, 
                                        new QTableWidgetItem(entity.id));
        ui.table_commit_log->setItem(row_count, 1, 
                                        new QTableWidgetItem(entity.author));
        ui.table_commit_log->setItem(row_count, 2, 
                                        new QTableWidgetItem(entity.time));
        ui.table_commit_log->setItem(row_count, 3, 
                                        new QTableWidgetItem(entity.comment));
    });

}

void Cattle::InsertPatchTable()
{
    while (ui.table_patch->rowCount() > 0) {
        ui.table_patch->removeRow(0);
    }

    std::for_each(mPatchVec.begin(), mPatchVec.end(), 
        [this] (const PatchEntity &entity) {
        int row_count = ui.table_patch->rowCount();
        ui.table_patch->insertRow(row_count);
        ui.table_patch->setItem(row_count, 0, 
                                        new QTableWidgetItem(entity.id));
        ui.table_patch->setItem(row_count, 1, 
                                 new QTableWidgetItem(entity.author));
    });
}

void Cattle::OpenPatchFile(const QString &patchFile) {
    // 1.使用Tortoise diff工具
    // 2.notepad++ 
    do {
        const QString register_path = R"(HKEY_LOCAL_MACHINE\SOFTWARE)";

        // tortoise git diff
        QSettings git_settings(register_path + R"(\TortoiseGit)",
                                QSettings::NativeFormat);
        QString exe_path = git_settings.value("Directory", "").toString();
        if (! exe_path.isEmpty()) {
            exe_path += R"(bin\TortoiseGitUDiff.exe)";

            ShellExecute(NULL, L"open", exe_path.toStdWString().c_str(), 
                patchFile.toStdWString().c_str(), NULL, SW_SHOW);
            break;
        }

        // tortoise svn diff
        QSettings svn_settings(register_path + R"(\TortoiseSvn)",
                                QSettings::NativeFormat);
        exe_path = svn_settings.value("Directory", "").toString();
        if (! exe_path.isEmpty()) {
            exe_path += R"(bin\TortoiseUDiff.exe)";

            ShellExecute(NULL, L"open", exe_path.toStdWString().c_str(), 
                patchFile.toStdWString().c_str(), NULL, SW_SHOW);
            break;
        } 

        // notepad++
        QSettings notepadpp_settings(register_path + R"(\Microsoft\Windows\CurrentVersion\Uninstall\notepad++)",
                                QSettings::NativeFormat);
        exe_path = notepadpp_settings.value("DisplayIcon", "").toString();
        if (! exe_path.isEmpty()) {
            ShellExecute(NULL, L"open", exe_path.toStdWString().c_str(), 
                patchFile.toStdWString().c_str(), NULL, SW_SHOW);
            break;
        }

        ShellExecute(NULL, L"open", L"notepad.exe", 
                        patchFile.toStdWString().c_str(), NULL, SW_SHOW);

        ui.textbrowser->append(
        WrapHtmlStr("red", 
        "Not find tortoise and notepad++ tools, please install tortoise tools.", 
                            kFONT_SIZE));

    } while(0);
}

void Cattle::OpenPatchFile(const QString &commitID, int) {
    PatchIterator ifind = std::find_if(mPatchVec.begin(), mPatchVec.end(), 
                    [&commitID] (const PatchEntity &entity) {
                    return JudgeCommitID()(commitID, entity.id);
                });
    
    if (ifind != mPatchVec.end()) {
        OpenPatchFile(ifind->filepath);
    } else {
        ui.textbrowser->append(
                    WrapHtmlStr("red", "Not find this log<br>", kFONT_SIZE));
    }
}

void Cattle::InitTextBrowser() {
    QString exe_path    = qApp->applicationFilePath();
    QString exe_version = 
        QString::fromStdString(dolProductInfo(exe_path.toStdString(), true).GetFileVersion());

    QString copy_right = QString("author: %1<br>"
                                 "contact: wechat 635672377<br>"
                                 "version: %2<br>").
                                    arg(QString::fromLocal8Bit("刘世雄")).
                                    arg(exe_version);
    QString use_age     = QString(
        QString::fromLocal8Bit("双击任何的表格的单元格，就会使用tortoise diff工具，如果没有安装tortoise，就会使用notepad++工具打开。"));

    ui.textbrowser->append(WrapHtmlStr("blue", copy_right, kFONT_SIZE));
    ui.textbrowser->append(WrapHtmlStr("blue", use_age, kFONT_SIZE));
}

void Cattle::on_table_commit_log_cellClicked(int row, int column) {
    QString commit_id   = ui.table_commit_log->item(row, 0)->text();
    QString comment     = ui.table_commit_log->item(row, 3)->text();

    PatchIterator ifind = std::find_if(mPatchVec.begin(), mPatchVec.end(), 
                    [&commit_id] (const PatchEntity &entity) {
                    return JudgeCommitID()(commit_id, entity.id);
                });
    
    if (ifind != mPatchVec.end()) {
        ui.textbrowser->append(WrapHtmlStr("blue", 
                                    comment + " " + ifind->filepath,
                                    kFONT_SIZE));

        int row = FindCommitIDRow(ui.table_patch, commit_id);

        if (nullptr == ui.table_patch->item(row, 2)) {
            ui.table_patch->setItem(row, 2, new QTableWidgetItem(comment));

            // rename file
            QFile file(ifind->filepath);
            QString order_number = (ifind->filepath).split("-")[0];
            QString new_file_name = QString("%1-%2.%3").
                        arg(order_number).arg(comment.replace("\n","")).
                        arg(kPATCH_FILE_SUFFIX);

            if (file.rename(new_file_name)) { 
                QString msg = 
                    QString("%1-->%2").arg(ifind->filepath).arg(new_file_name);
                ifind->filepath = new_file_name;
                ui.textbrowser->append(WrapHtmlStr("blue", msg, kFONT_SIZE));
            } else {
                QString msg = "origin file name:" + 
                               ifind->filepath + " rename error"; 

                ui.textbrowser->append(WrapHtmlStr("red", msg, kFONT_SIZE));
            }
        }

        ui.table_patch->selectRow(row);

    } else {
        ui.textbrowser->append(WrapHtmlStr("red", " Not find this log.", kFONT_SIZE));
    }
}

void Cattle::on_table_commit_log_cellDoubleClicked(int row, int column) {
    QString commit_id   = ui.table_commit_log->item(row, 0)->text();

    OpenPatchFile(commit_id, 0);
}

void Cattle::on_table_patch_cellDoubleClicked(int row, int column) {
    QString commit_id   = ui.table_patch->item(row, 0)->text();

    OpenPatchFile(commit_id, 0);
}
