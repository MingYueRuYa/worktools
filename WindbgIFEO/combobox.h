#pragma once

#include <QCombobox>
#include <QWidget>

class ComboBox : public QComboBox {
  Q_OBJECT

 public:
  ComboBox(QWidget* parent);
  ~ComboBox() = default;

  void update_theme();
  bool set_line_edit_enable(bool enable);

 protected slots:
  void on_text_changed(const QString& text);

 private:
  void _init_ui();
  void _init_signal();

 private:
  Q_DISABLE_COPY(ComboBox)
};
