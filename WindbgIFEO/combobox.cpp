#include "combobox.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QLineEdit>
#include <QScrollBar>
#include <QStyledItemDelegate>

class ComboBoxItemDelegate : public QStyledItemDelegate {
 public:
  ComboBoxItemDelegate(QObject* parent = nullptr)
      : QStyledItemDelegate(parent) {}

  void initStyleOption(QStyleOptionViewItem* option,
                       const QModelIndex& index) const {
    QStyledItemDelegate::initStyleOption(option, index);
    option->displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
  }
};

ComboBox::ComboBox(QWidget* parent) : QComboBox(parent) {
  this->_init_ui();
  this->_init_signal();
  qApp->setEffectEnabled(Qt::UI_AnimateCombo, false);
  this->setItemDelegate(new ComboBoxItemDelegate());
  this->update_theme();
}

void ComboBox::update_theme() {
  std::string qss = R"(QComboBox{
border:none;
background:
#EDEDED;
				  color:
				  #4C4C4C;
				  font-size:12px;
				  padding:0 0 0 8px;
				  border-radius:2px;
				  margin:0 6px;
				  text-align: Middle;
				  }
				  QComboBox[error=true]{
				  border: 1px solid rgba(255, 61, 61, 0.2);
				  color: #FF3D3D;
				  }
				  QComboBox::hover,QComboBox::on{
				  padding-left:7px;
				  background:transparent;
				  
				  border:
				  1px solid #CCCCCC;
				  }
				  QComboBox::down-arrow{
				  border:none;
				  background:transparent;
				  }
				  QComboBox::drop-down{
				  width:12px;
				  height:7px;
				  subcontrol-position:right center;
				  image:url(:/WindbgConfig/images/arrow_down.png);
				  padding-right: 11px;
				  }
				  QComboBox::drop-down:on{
				  image:url(:/WindbgConfig/images/arrow_up.png);
				  }
				  QComboBox:editable{
				  background:
				  #EDEDED;
				  }
				  QComboBox:disabled{
				  padding-left:7px;
				  background:transparent;
				  border:
				  1px solid #CCCCCC;
				  })";

  std::string view_str = R"(QAbstractItemView {
outline: none;
show-decoration-selected:0;
border:
1px solid #CCCCCC;
background:
#ffffff;
				   padding: 6px;
				   margin:4 6px;
				   border-radius:2px;
				   min-width: 0px;
				   }
				   QAbstractItemView::item {
				   color:
				   #4c4c4c;
				   font-size:12px;
				   height:28px;
				   padding:0 0 0 8px;
				   }
				   QAbstractItemView::item:hover, QAbstractItemView::item:selected{
				   padding:0 0 0 0;
				   border-radius:2px;
				   background-color:
				   #E0E0E0;
				   color:
				   #7e63f1;
				   })";

  std::string scrollbar_str = R"(QScrollBar:vertical{
border:1px solid transparent;
width:4px;
background-color:
transparent;
						 padding-top:16px;
						 padding-bottom:16px;
						 }
						 QScrollBar:vertical:hover{
						 border:none;
						 border-radius:2px;
						 }
						 QScrollBar::handle:vertical{
						 background:
						 #ededed;
						 width:4px;
						 border-radius:2px;
						 }
						 QScrollBar::handle:vertical:hover{
						 background:
						 #e6e6e6;
						 }
						 QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical{background:none;}
						 QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical{
						 height:0px;width:0px;
						 background-color:rgba(0,0,0,0);
						 })";

  std::string parent_str = R"(QWidget{
background-color:transparent;
border:none;
margin:0;
})";

  this->setStyleSheet(QString::fromStdString(qss));
  this->view()->setStyleSheet(QString::fromStdString(view_str));

  QWidget* w = this->view()->window();
  w->setWindowFlag(Qt::FramelessWindowHint, true);
  w->setWindowFlag(Qt::NoDropShadowWindowHint, true);
  w->setAttribute(Qt::WA_NoSystemBackground);
  w->setAttribute(Qt::WA_TranslucentBackground);
  QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(w);
  shadow->setOffset(0, 0);
  shadow->setColor(QColor("#19000000"));
  shadow->setBlurRadius(10);
  this->view()->setGraphicsEffect(shadow);

  this->view()->verticalScrollBar()->setStyleSheet(
      QString::fromStdString(scrollbar_str));
  this->view()->parentWidget()->setStyleSheet(
      QString::fromStdString(parent_str));
}

bool ComboBox::set_line_edit_enable(bool enable) {
  QLineEdit* line_edit = this->lineEdit();
  if (nullptr == line_edit) {
    return false;
  }

  line_edit->setFocusPolicy(enable ? Qt::FocusPolicy::ClickFocus
                                   : Qt::FocusPolicy::NoFocus);
  line_edit->setEnabled(enable);
  return true;
}

void ComboBox::on_text_changed(const QString& text) {
  QLineEdit* line_edit = this->lineEdit();
  if (nullptr == line_edit) {
    return;
  }
  QFontMetrics metrics(line_edit->font());
  int width = line_edit->width() - 5;
  QString elided_str = metrics.elidedText(text, Qt::ElideMiddle, width);
  line_edit->setText(elided_str);
}

void ComboBox::_init_signal() {
  connect(this, SIGNAL(currentTextChanged(QString)), this,
          SLOT(on_text_changed(QString)));
}

void ComboBox::_init_ui() {}
