#pragma once

#include "ui_EnvTools.h"
#include "EnvHelper.h"

#include <QtWidgets/QWidget>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_set>

enum class EditType
{
  Unknown = -1,
  DelKey = 0,
  AddKey = 1,
  UpdateKey = 3,
  UpdateValue = 4
};

struct CachedData
{
public:
  CachedData() {
  }
  ~CachedData() {}
  EditType type = EditType::Unknown;

  wstring old_key = L"";
  wstring new_key = L"";
  wstring old_value = L"";
  wstring new_value = L"";

  bool operator==(const CachedData& right) const
  {
    if (this->type == right.type &&
      this->old_key == right.old_key &&
      this->new_key == right.new_key &&
      this->old_value == right.old_value &&
      this->new_value == right.new_value)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

};

template<>
struct std::hash<CachedData*>
{
  std::size_t operator()(const CachedData* s) const noexcept
  {
    std::size_t h1 = std::hash<std::wstring>{}(s->old_key);
    std::size_t h2 = std::hash<std::wstring>{}(s->new_key);
    std::size_t h3 = std::hash<std::wstring>{}(s->old_value);
    std::size_t h4 = std::hash<std::wstring>{}(s->new_value);
    std::size_t val = h1 ^ (h2 << 1) ^ h3 ^ h4 ^ (int)s->type;
    return val; // or use boost::hash_combine
  }
};

template <>
struct std::equal_to<CachedData*> {

  bool operator()(const CachedData* _Left, const CachedData* _Right) const {
    return *_Left == *_Right;
  }
};

class EnvTools : public QWidget
{
  Q_OBJECT
public:
  EnvTools(QWidget* parent = nullptr);
  ~EnvTools();

private:
  void _init_ui();
  void _init_signal();
  void _update_cached(EditType type,
    const QString& old_key,
    const QString& new_key,
    const QString& old_value,
    const QString& new_value);
  CachedData* _gen_cached(EditType type,
    const QString& old_key,
    const QString& new_key,
    const QString& old_value,
    const QString& new_value);
  void _clear_cached();
  void test_data();

protected slots:
  void DoCellClicked(int row, int column);
  void OnItemChanged(QTableWidgetItem*);
  void on_btn_add_all_clicked();
  void on_btn_delete_all_clicked();
  void on_btn_add_sub_clicked();
  void on_btn_del_sub_clicked();
  void on_btn_save_clicked();

private:
  Ui::EnvToolsClass ui;
  EnvHelper _env_helper;
  //std::hash<CachedData*> _cached_data;
  std::unordered_set<CachedData*> _cached_data;
};
