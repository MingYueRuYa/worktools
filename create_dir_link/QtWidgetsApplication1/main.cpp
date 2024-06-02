#include "QtWidgetsApplication1.h"
#include <QtWidgets/QApplication>


#include <QApplication>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QAbstractItemView>
#include <QStyledItemDelegate>
#include <QCheckBox>
#include <QFileSystemModel>
#include <QDir>
#include <QMouseEvent>
#include <unordered_map>


class CheckBoxDelegate : public QStyledItemDelegate
{
public:
    CheckBoxDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyledItemDelegate::paint(painter, option, index);
        const QModelIndex &par_idx = index.parent();
        if (index.column() == 0 && par_idx.isValid()) {
            bool checked = index.model()->data(index, Qt::UserRole).toBool();
            QStyleOptionButton checkBoxStyle;
            checkBoxStyle.state = checked ? QStyle::State_On : QStyle::State_Off;
             checkBoxStyle.rect = option.rect.adjusted(5, 0, 0, 0);
            QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxStyle, painter);
        }
    }

    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
    {
        if ((event->type() == QEvent::MouseButtonRelease) && (index.column() == 0)) {
             QRect checkBoxRect = option.rect.adjusted(5, 0, 0, 0);
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            // if (checkBoxRect.contains(mouseEvent->pos())) {
                bool checked = index.model()->data(index, Qt::UserRole).toBool();
                bool result = model->setData(index, !checked, Qt::UserRole);
                return true;
            // }
        }
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
};


//class CheckBoxDelegate : public QStyledItemDelegate
//{
//public:
//    CheckBoxDelegate(QObject* parent = 0)
//        : QStyledItemDelegate(parent)
//    {
//    }
//
//    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
//    {
//        if (index.column() == 0)
//        {
//            QCheckBox* checkBox = new QCheckBox(parent);
//            return checkBox;
//        }
//        return QStyledItemDelegate::createEditor(parent, option, index);
//    }
//
//    void setEditorData(QWidget* editor, const QModelIndex& index) const override
//    {
//        if (QCheckBox* checkBox = qobject_cast<QCheckBox*>(editor))
//        {
//            bool value = index.model()->data(index, Qt::UserRole).toBool();
//            checkBox->setChecked(value);
//        }
//        else
//        {
//            QStyledItemDelegate::setEditorData(editor, index);
//        }
//    }
//
//    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
//    {
//        if (QCheckBox* checkBox = qobject_cast<QCheckBox*>(editor))
//        {
//            model->setData(index, checkBox->isChecked(), Qt::UserRole);
//        }
//        else
//        {
//            QStyledItemDelegate::setModelData(editor, model, index);
//        }
//    }
//};
//
class FileModelSystem : public QFileSystemModel
{
public:
    FileModelSystem(QObject *parent = nullptr) : QFileSystemModel(parent)
    {
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override
    {
        if (index.column() == 0)
        {
            //ItemIsSelectable = 1,
            //ItemIsEditable = 2,
            //ItemIsDragEnabled = 4,
            //ItemIsDropEnabled = 8,
            //ItemIsUserCheckable = 16,
            //ItemIsEnabled = 32,

            return  Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
        }

        return QFileSystemModel::flags(index);
    }

    QVariant data(const QModelIndex &idx, int role) const
    {
        if (role == Qt::DecorationRole && 0 == idx.column())
        {
            QModelIndex par_index = idx.parent();
            if (par_index.isValid())
            {
				QImage pixmap(16, 1, QImage::Format_ARGB32_Premultiplied);
				pixmap.fill(Qt::transparent);
				return QIcon(QPixmap::fromImage(pixmap));
            }
            else
            {
                return QIcon();
            }
        }
        else if (role == Qt::UserRole && 0 == idx.column())
        {
            auto itr = _map_check_data.find(idx);
            if (_map_check_data.end() == itr)
            {
                return false;
            }
            else {
                return itr.value();
            }
            return true;
        }
        else
        {
            return QFileSystemModel::data(idx, role);
        }
    }

    bool setData(const QModelIndex &idx, const QVariant &value, int role)
    {
        if (role == Qt::UserRole && 0 == idx.column())
        {
            auto itr = _map_check_data.find(idx);
            if (_map_check_data.end() == itr)
            {
                _map_check_data[idx] = value.toBool();
                return true;
            }
            else {
                _map_check_data[idx] = value.toBool();
                return true;
            }
            return true;
        }
        else
        {
            return QFileSystemModel::setData(idx, value, role);
        }
    }


private:
     QHash<QModelIndex, bool> _map_check_data;

};

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    //QStandardItemModel model;
    //QStandardItem* rootItem = model.invisibleRootItem();

    //// 添加带有复选框的项
    //for (int i = 0; i < 5; ++i)
    //{
    //    QStandardItem* item = new QStandardItem(QString("Item %1").arg(i));
    //    item->setData(false, Qt::UserRole); // 用UserRole存储复选框状态
    //    item->setCheckable(true);
    //    rootItem->appendRow(item);
    //}

    //QTreeView treeView;
    //treeView.setModel(&model);
    //treeView.setItemDelegateForColumn(0, new CheckBoxDelegate(&treeView)); // 为第一列设置自定义委托

    //treeView.show();

    //FileModelSystem* model = new FileModelSystem();
    //model->setRootPath(QDir::currentPath());


    //QTreeView* tree = new QTreeView();
    //tree->setModel(model);

    //tree->setRootIndex(model->index(QDir::currentPath()));
    //// tree->setItemDelegateForColumn(0, new CheckBoxDelegate(tree)); // 为第一列设置自定义委托
    //tree->setItemDelegate(new CheckBoxDelegate(tree));
    //tree->show();

        // 创建文件系统模型
    // QFileSystemModel model;
    FileModelSystem model;
    model.setRootPath(QDir::currentPath());
    model.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);

    // 创建树视图
    QTreeView treeView;
    treeView.setModel(&model);
    //treeView.setRootIsDecorated(false);
    //treeView.setAnimated(false);
    // treeView.setExpandsOnDoubleClick(false);
    treeView.setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置自定义委托
    CheckBoxDelegate* delegate = new CheckBoxDelegate(&treeView);
    treeView.setItemDelegateForColumn(0, delegate); // 设置第一列使用自定义委托


        // 连接信号槽，实现父节点选中子节点也自动选中
    QObject::connect(&treeView, &QTreeView::clicked, [&model, &treeView](const QModelIndex& index) {
        bool state = model.data(index, Qt::UserRole).value<bool>();
        if (state) {
            // 如果父节点已选中，则将所有子节点也选中
            for (int i = 0; i < model.rowCount(index); ++i) {
                const QModelIndex& childIndex = model.index(i, 0, index);
                model.setData(childIndex, QVariant(true), Qt::UserRole);
                treeView.update(childIndex);
            }
        }
        else {
            // 如果父节点取消选中，则将所有子节点也取消选中
            for (int i = 0; i < model.rowCount(index); ++i) {
                const QModelIndex& childIndex = model.index(i, 0, index);
                model.setData(childIndex, QVariant(false), Qt::UserRole);
                treeView.update(childIndex);
            }
        }
        });

    treeView.setWindowTitle("File System with Checkboxes");
    treeView.show();


    return a.exec();
}

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    QtWidgetsApplication1 w;
//    w.show();
//    return a.exec();
//}
