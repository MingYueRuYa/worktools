#pragma once

#include <QCombobox>
#include <QWidget>

class ComboBox : public QComboBox {
  Q_OBJECT

 public:
  ComboBox(QWidget* parent);
  ~ComboBox() = default;

  void update_theme();

 private:
  Q_DISABLE_COPY(ComboBox)
};
