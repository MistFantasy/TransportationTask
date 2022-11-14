#ifndef TABLEITEM_H
#define TABLEITEM_H

#include <QTableWidgetItem>

class TableItem : public QTableWidgetItem
{
public:
  TableItem();
  TableItem(QString TextOfItem, int Row, int Col);
  QString GetItemText() const;
  int GetItemRow() const;
  int GetItemCol() const;
private:
  QString ItemText;
  int ItemRow;
  int ItemCol;
};

#endif // TABLEITEM_H
