#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include "tableitem.h"
#include <math.h>
#define EPS 0.00000000000001
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void CreateEmptyInterfaceTable();
  void DeleteInterfaceTable();
  void FillTableFromFile();
  void CreateMiddleMatrixArray(int Row, int Col);
  void DeleteMiddleMatrixArray();
  bool CheckInput(QTableWidgetItem *Item);
  void InitializeVectors(int Row, int Col);
  bool IsTaskClosed();
  float MinimalElementMethod();
  void RefreshTable(int Row, int Col, QString WhichToUse);
  void FillInterfaceTable(QString WhereToUse);
  void CalculateUVVectors();
  int FindMaxFilledCells();
  bool CalculateDelta();
  void CreateCorrectPathArray();
  void CreateWasHereArray();
  void CreateFillCellBoolArray();
  void DeleteArrays(QString WhichArray);
  bool FindCycle(int Row, int Col);
  void CalculateFillCellBoolArray();
  void RefreshTransportationCells();
  void ClearOptimalPlanLabel(float SummaryValue);
  void FoundPath(int Row, int Col);
  bool SearchRow(int Col);
  bool SearchCol(int Row);
private slots:
  void on_CreateTableButton_clicked();

  void on_tableWidget_itemChanged(QTableWidgetItem *Item);

  void on_EnterDataFromFileButton_clicked();

  void on_InitialReferencePlanButton_clicked();

  void on_PotentialMethodButton_clicked();

  void on_CalculateCycleButton_clicked();
private:
  Ui::MainWindow *ui;
  bool isMiddleMatrixArrayEmpty;
  bool isCorrectPathEmpty;
  bool isWasHereEmpty;
  bool isFillCellBoolArrayEmpty;
  int RowCount,       // К-сть рядків у таблиці
  m_RowSize,          // К-сть рядків у матриці
  ColCount,           // К-сть стовпців у таблиці
  m_ColSize,          // К-сть стовпців у матриці
  MaxDeltaRow,        // Координата максимального дельта-елемента за рядком
  MaxDeltaCol;        // Координата максимального дельта-елемента за стовпцем
  QVector <TableItem> ItemStorage; // Інформація в комірках таблиці, для заміни в разі неправ. вводу даних
  QMap<float, float> ** MiddleMatrixArray; //Midlle[Row][Col].value(midl[row][col].key)
  QVector<QMap<float, float>> FillCellsVector;
  QVector<QMap<int, int>> FillCellsCoordinates;
  QVector <int> StockVector, NeedVector;
  QVector <float> UVector, VVector;
  bool ** CorrectPath;
  bool ** WasHere;
  bool ** FillCellBoolArray;
  bool FoundCorrectPath;
  static int CycleCounter;
  static int InOutCounter;
};
#endif // MAINWINDOW_H
