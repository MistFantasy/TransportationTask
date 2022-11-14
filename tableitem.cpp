#include "tableitem.h"

TableItem::TableItem()
{

}

TableItem::TableItem(QString TextOfItem, int Row, int Col)
  : ItemText(TextOfItem), ItemRow(Row), ItemCol(Col)
{

}

QString TableItem::GetItemText() const
{
  return ItemText;
}

int TableItem::GetItemRow() const
{
  return ItemRow;
}

int TableItem::GetItemCol() const
{
  return ItemCol;
}





