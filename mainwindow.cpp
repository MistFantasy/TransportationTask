#include "mainwindow.h"
#include "ui_mainwindow.h"

int MainWindow::CycleCounter = 0;
int MainWindow::InOutCounter = 0;

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow), MiddleMatrixArray(nullptr), RowCount(0), ColCount(0)
  , MaxDeltaRow(0), MaxDeltaCol(0)
{
  ui->setupUi(this);
  QHeaderView * HorizontalHeader = ui->tableWidget->horizontalHeader();
  HorizontalHeader->setSectionResizeMode(QHeaderView::Stretch);
  isMiddleMatrixArrayEmpty = true;
  isCorrectPathEmpty = true;
  isWasHereEmpty = true;
  isFillCellBoolArrayEmpty = true;
  ui->EnterDataFromFileButton->setEnabled(false);
  ui->InitialReferencePlanButton->setEnabled(false);
  ui->PotentialMethodButton->setEnabled(false);
  ui->CalculateCycleButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::ClearOptimalPlanLabel(float SummaryValue)
{
  QRegularExpression RegEx("[0-9]+");
  if(SummaryValue > EPS)
    {
      if(ui->OptimalPlanLabel->text().contains(RegEx))
        {
          ui->OptimalPlanLabel->setText(ui->OptimalPlanLabel->text().remove(RegEx));
        }
      ui->OptimalPlanLabel->setText(ui->OptimalPlanLabel->text() + QString::number(SummaryValue));
    }
  else
    {
      ui->OptimalPlanLabel->setText(ui->OptimalPlanLabel->text().remove(RegEx));
    }
}

void MainWindow::CreateCorrectPathArray()
{
  CorrectPath = new bool *[m_RowSize];
  for(int i = 0; i < m_RowSize; i++)
    CorrectPath[i] = new bool[m_ColSize]();
  isCorrectPathEmpty = false;
}

void MainWindow::CreateWasHereArray()
{
  WasHere = new bool *[m_RowSize];
  for(int i = 0; i < m_RowSize; i++)
    WasHere[i] = new bool[m_ColSize]();
  isWasHereEmpty = false;
}

void MainWindow::CreateFillCellBoolArray()
{
  FillCellBoolArray = new bool *[m_RowSize];
  for(int i = 0; i < m_RowSize; i++)
    FillCellBoolArray[i] = new bool[m_ColSize]();
  isFillCellBoolArrayEmpty = false;
}

void MainWindow::DeleteArrays(QString WhichArray)
{
  if(WhichArray == "CorrectPath")
    {
      for(int i = 0; i < m_RowSize; i++)
        {
          delete [] CorrectPath[i];
        }
      delete [] CorrectPath;
      isCorrectPathEmpty = true;
    }
  if(WhichArray == "WasHere")
    {
      for(int i = 0; i < m_RowSize; i++)
        {
          delete [] WasHere[i];
        }
      delete [] WasHere;
      isWasHereEmpty = true;
    }
  if(WhichArray == "MiddleMatrixArray")
    {
      for(int i = 0; i < m_RowSize; i++)
        {
          delete [] MiddleMatrixArray[i];
        }
      delete [] MiddleMatrixArray;
    }
  if(WhichArray == "FillCellBoolArray")
    {
      for(int i = 0; i < m_RowSize; i++)
        {
          delete [] FillCellBoolArray[i];
        }
      delete [] FillCellBoolArray;
      isFillCellBoolArrayEmpty = true;
    }
}

void MainWindow::DeleteInterfaceTable()
{
  for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
      for(int j = 0; j < ui->tableWidget->columnCount(); j++)
        {
          delete ui->tableWidget->item(i, j);
        }
    }
}

void MainWindow::CreateEmptyInterfaceTable()
{
  ui->tableWidget->setRowCount(RowCount);
  ui->tableWidget->setColumnCount(ColCount);
  for (int i = 0; i < RowCount; i++)
    {
      for (int j = 0; j < ColCount; j++)
        {
          ui->tableWidget->setItem(i, j, new QTableWidgetItem(""));
          ui->tableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void MainWindow::FillInterfaceTable(QString WhereToUse)
{
  ui->tableWidget->blockSignals(true);
  if(WhereToUse == "CreateTableButton")
    {
      RowCount = ui->RowSpinBox->value() + 2, //+ 2 - Це виділення рядка під заголовок "Споживачі" і під Потреби
      ColCount = ui->ColSpinBox->value() + 2; //+ 2 - Це виділення стовпців під заголовок "Постачальники" і під Запас
      CreateEmptyInterfaceTable();
      if(isMiddleMatrixArrayEmpty)
        {
          CreateMiddleMatrixArray(ui->RowSpinBox->value(),
                              ui->ColSpinBox->value());
        }
      else
        {
          DeleteArrays("MiddleMatrixArray");
          CreateMiddleMatrixArray(ui->RowSpinBox->value(),
                                  ui->ColSpinBox->value());
        }
      InitializeVectors(RowCount - 1, ColCount - 1);
      for(int i = 1; i < RowCount; i++)
        {
          ui->tableWidget->item(i, 0)->setText("A" + QString::number(i));
          ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignCenter);
          ui->tableWidget->item(i, 0)->setFlags(Qt::NoItemFlags);
        }
      ui->tableWidget->item(RowCount - 1, 0)->setText("Потреби");
      for(int i = 1; i < ColCount; i++)
        {
          ui->tableWidget->item(0, i)->setText("B" + QString::number(i));
          ui->tableWidget->item(0, i)->setTextAlignment(Qt::AlignCenter);
          ui->tableWidget->item(0, i)->setFlags(Qt::NoItemFlags);
        }
      ui->tableWidget->item(0, ColCount - 1)->setText("Запас");
      ui->tableWidget->item(0, 0)->setFlags(Qt::NoItemFlags);
    }
  if(WhereToUse == "PotentialMethodButton")
    {
      RowCount = ui->RowSpinBox->value() + 3, //+ 3 - Це виділення рядка під заголовок "Споживачі", під Потреби та під V
      ColCount = ui->ColSpinBox->value() + 3; //+ 3 - Це виділення стовпців під заголовок "Постачальники", під Запаси та під U
      CreateEmptyInterfaceTable();
      for (int i = 1; i < RowCount - 2; i++)
        {
          ui->tableWidget->item(i + 1, 1)->setText("u" + QString::number(i));
          ui->tableWidget->item(i + 1, 1)->setTextAlignment(Qt::AlignCenter);
          ui->tableWidget->item(i + 1, 1)->setFlags(Qt::NoItemFlags);
        }
      for (int i = 1; i < ColCount - 2; i++)
        {
          ui->tableWidget->item(1, i + 1)->setText("v" + QString::number(i));
          ui->tableWidget->item(1, i + 1)->setTextAlignment(Qt::AlignCenter);
          ui->tableWidget->item(1, i + 1)->setFlags(Qt::NoItemFlags);
        }
      for(int i = 1; i < RowCount - 1; i++)
        {
          ui->tableWidget->item(i + 1, 0)->setText("A" + QString::number(i));
          ui->tableWidget->item(i + 1, 0)->setTextAlignment(Qt::AlignCenter);
          ui->tableWidget->item(i + 1, 0)->setFlags(Qt::NoItemFlags);
        }
      ui->tableWidget->item(RowCount - 1, 0)->setText("Потреби");
      for(int i = 1; i < ColCount - 1; i++)
        {
          ui->tableWidget->item(0, i + 1)->setText("B" + QString::number(i));
          ui->tableWidget->item(0, i + 1)->setTextAlignment(Qt::AlignCenter);
          ui->tableWidget->item(0, i + 1)->setFlags(Qt::NoItemFlags);
        }
      ui->tableWidget->item(0, ColCount - 1)->setText("Запас");
      ui->tableWidget->item(0, 0)->setFlags(Qt::NoItemFlags);
    }
    ui->tableWidget->blockSignals(false);
}

void MainWindow::RefreshTable(int Row, int Col, QString WhichToUse)
{
  ui->tableWidget->blockSignals(true);
  for (int i = 0; i < m_RowSize; i++)
    {
      for (int j = 0; j < m_ColSize; j++)
        {
          if(MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey())
             > EPS || MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey())
             < -EPS)
            {
              ui->tableWidget->item(i + Row, j + Col)->setText(
                QString::number(MiddleMatrixArray[i][j].firstKey())
                + '/' + QString::number(MiddleMatrixArray[i][j].value(
                                          MiddleMatrixArray[i][j].firstKey())));
            }
          else
            {
              ui->tableWidget->item(i + Row, j + Col)->setText(
                QString::number(MiddleMatrixArray[i][j].firstKey()));
            }
        }
    }
  for (int i = 0; i < StockVector.size(); i++)
    {
      ui->tableWidget->item(i + Row, ColCount - 1)->setText(
        QString::number(StockVector.at(i)));
    }
  for (int i = 0; i < NeedVector.size(); i++)
    {
      ui->tableWidget->item(RowCount - 1, i + Col)->setText(
        QString::number(NeedVector.at(i)));
    }
  if(WhichToUse == "PotentialMethodButton")
    {
      for (int i = 0; i < UVector.size(); i++)
        {
          ui->tableWidget->item(i + Row, Col - 1)->setText(
           ui->tableWidget->item(i + Row, Col - 1)->text()
               + " = "  + QString::number(UVector.at(i)));
        }
      for (int i = 0; i < VVector.size(); i++)
        {
          ui->tableWidget->item(Row - 1, i + Col)->setText(
                ui->tableWidget->item(Row - 1, i + Col)->text()
                    + " = "  + QString::number(VVector.at(i)));
        }
    }
  ui->tableWidget->blockSignals(false);
}

void MainWindow::CreateMiddleMatrixArray(int Row, int Col)
{
  m_RowSize = Row;
  m_ColSize = Col;
  MiddleMatrixArray = new QMap<float, float> *[m_RowSize];
  for(int i = 0; i < m_RowSize; i++)
    MiddleMatrixArray[i] = new QMap<float, float>[m_ColSize];
  isMiddleMatrixArrayEmpty = false;
}

void MainWindow::InitializeVectors(int Row, int Col)
{
  StockVector.clear();
  NeedVector.clear();
  UVector.clear();
  VVector.clear();
  for (int i = 0; i < Row; i++)
    {
      StockVector.append(0);
    }
  for (int i = 0; i < Col; i++)
    {
      NeedVector.append(0);
    }
  for (int i = 0; i < Row - 1; i++)
    UVector.append(0);
  for (int i = 0; i < Col - 1; i++)
    VVector.append(0);
}

void MainWindow::on_CreateTableButton_clicked()
{
  ui->tableWidget->blockSignals(true);
  FillInterfaceTable("CreateTableButton");
  if(!isFillCellBoolArrayEmpty)
    {
      DeleteArrays("FillCellBoolArray");
    }
  ui->EnterDataFromFileButton->setEnabled(true);
  ui->InitialReferencePlanButton->setEnabled(true);
  ui->PotentialMethodButton->setEnabled(false);
  ui->CalculateCycleButton->setEnabled(false);
  ui->tableWidget->blockSignals(false);
  ClearOptimalPlanLabel(0);
}

bool MainWindow::CheckInput(QTableWidgetItem *Item)
{
  bool IsValid = false, OK;
  if(Item->column() == ColCount - 1 || Item->row() == RowCount - 1)
    {
      Item->text().toInt(&OK);
      if(OK)
        IsValid = true;
    }
  else
    {
      Item->text().toFloat(&OK);
      if(OK)
        {
          IsValid = true;
        }
    }
  if(!IsValid)
    ui->statusBar->showMessage("Ви ввели неправильні дані, введіть, будь ласка, ще раз."
    "\nФормат ведення: 4; 100;", 5000);
  return IsValid;
}

void MainWindow::on_tableWidget_itemChanged(QTableWidgetItem *Item)
{
  int ItemStorageCounter = 0;
  ui->tableWidget->blockSignals(true);
  if(!CheckInput(Item))
    {
      if(ItemStorage.isEmpty())
        {
          Item->setText("");
        }
      for(int i = 0; i < ItemStorage.size(); i++)
        if(ItemStorage[i].GetItemRow() == Item->row() &&
           ItemStorage[i].GetItemCol() == Item->column())
          {
            Item->setText(ItemStorage[i].GetItemText());
            break;
          }
        else
          {
            Item->setText("");
          }
      ui->tableWidget->blockSignals(false);
      return;
    }
  // Масиви з нуля, таблиця з одиниці, тому треба віднімати одиницю.
  if(Item->column() == ColCount - 1) // Запаси
    {
      StockVector[Item->row() - 1] = Item->text().toInt();
    }
  else if(Item->row() == RowCount - 1) // Потреби
    {
      NeedVector[Item->column() - 1] = Item->text().toInt();
    }
  else
    {
        MiddleMatrixArray[Item->row() - 1][Item->column() - 1].clear();
        MiddleMatrixArray[Item->row() - 1][Item->column() - 1]
            [Item->text().toFloat()] = 0.0;
    }
    TableItem TempItem(Item->text(), Item->row(), Item->column());
    if(ItemStorage.isEmpty())
      ItemStorage.append(TempItem);
    for(int i = 0; i < ItemStorage.size(); i++)
    {
        if(ItemStorage[i].GetItemRow() == Item->row() &&
            ItemStorage[i].GetItemCol() == Item->column())
        {
            ItemStorage[i] = TempItem;
            ItemStorageCounter++;
            ui->tableWidget->blockSignals(false);
        }
    }
    if(ItemStorageCounter == 0)
      {
        ItemStorage.append(TempItem);
      }
  ui->tableWidget->blockSignals(false);
}

void MainWindow::FillTableFromFile()
{
  QString TempValue;
  ui->tableWidget->blockSignals(true); // blockSignals для того щоб заблокувати вхідні сигнали до таблиці
  for (int i = 0; i < m_RowSize; i++)
    {
      for (int j = 0; j < m_ColSize; j++)
        {
          TempValue = QString::number(MiddleMatrixArray[i][j].firstKey());
          ui->tableWidget->item(i + 1, j + 1)->setText(TempValue);
        }
    }
  for (int i = 0; i < StockVector.size(); i++)
    {
      ui->tableWidget->item(i + 1, ColCount - 1)->setText(
            QString::number(StockVector[i]));
    }
  for (int i = 0; i < NeedVector.size(); i++)
    {
      ui->tableWidget->item(RowCount - 1, i + 1)->setText(
            QString::number(NeedVector[i]));
    }
  ui->tableWidget->blockSignals(false);
}

void MainWindow::on_EnterDataFromFileButton_clicked()
{
  QString FileName, TempValue;
  QStringList MatrixNumbers[ui->RowSpinBox->value() + 1];
  FileName = QFileDialog::getOpenFileName(this, "Відкрити файл",
  "E:\\QT\\Projects\\Lab_4_DO\\build-Lab_4_DO-Desktop_Qt_6_1_2_MinGW_64_bit-Debug",
                                          "Data Files (*.dat);; Text Files (*.txt);; All Files (*.*)", nullptr, QFileDialog::DontUseNativeDialog);
  if(!FileName.isEmpty())
    {
      QFile InputFile(FileName);
      InputFile.open(QIODevice::ReadOnly);
      QTextStream In;
      In.setDevice(&InputFile);
      for(int i = 0; !In.atEnd(); i++)
       {
         MatrixNumbers[i] = In.readLine().split(' ');
       }
      for(int i = 0; i < m_RowSize; i++)
        {
          for(int j = 0; j < m_ColSize; j++)
            {
              TempValue = MatrixNumbers[i][j];
              MiddleMatrixArray[i][j][TempValue.toFloat()] = 0.0;
            }
        }
      for (int i = 0; i < ui->RowSpinBox->value(); i++)
        {
          StockVector[i] = MatrixNumbers[i][ui->ColSpinBox->value()].toInt();
        }
      for (int i = 0; i < ui->ColSpinBox->value(); i++)
        {
          NeedVector[i] = MatrixNumbers[ui->RowSpinBox->value()][i].toInt();
        }
     InputFile.close();
     FillTableFromFile();
    }
}

bool MainWindow::IsTaskClosed()
{
  int StockSum = 0, NeedSum = 0;
  bool IsClosed = false;
  StockVector[StockVector.size() - 1] = 0;
    NeedVector[NeedVector.size() - 1] = 0;
  for (int i = 0; i < StockVector.size(); i++)
    {
      StockSum += StockVector[i];
    }
  StockVector[StockVector.size() - 1] = StockSum;
  for (int i = 0; i < NeedVector.size(); i++)
    {
      NeedSum += NeedVector[i];
    }
  NeedVector[NeedVector.size() - 1] = NeedSum;
  ui->tableWidget->item(RowCount - 1, ColCount - 1)->setText("");
  if(StockSum == NeedSum)
    IsClosed = true;
  return IsClosed;
}

float MainWindow::MinimalElementMethod()
{
  int MinRow = 0, MinCol = 0, StockSum = 1, NeedSum = 1, PlanCounter = 0; //Не змінювати потрібно для входу в цикл
  float TotalSum = 0.0;
  QVector<int> TempStock = StockVector, TempNeed = NeedVector;
  while(StockSum != 0 || NeedSum != 0)
    {
      StockSum = 0, NeedSum = 0, MinRow = 0, MinCol = 0;
      for(int i = 0; i < TempStock.size(); i++)
        if(TempStock[MinRow] == 0)
          MinRow++;
      for(int i = 0; i < TempNeed.size(); i++)
        if(TempNeed[MinCol] == 0)
          MinCol++;
      for (int i = 0; i < m_RowSize; i++)
        {
          for (int j = 0; j < m_ColSize; j++)
            {
              if(MiddleMatrixArray[i][j].firstKey() <=
                 MiddleMatrixArray[MinRow][MinCol].firstKey())
                {
                  if(TempStock[i] != 0 && TempNeed[j] != 0)
                    {
                      MinRow = i;
                      MinCol = j;
                    }
                }
            }
        }
      if(TempStock[MinRow] <= TempNeed[MinCol])
        {
          MiddleMatrixArray[MinRow][MinCol].insert(MiddleMatrixArray[MinRow][MinCol].firstKey(),
                                                   TempStock[MinRow]);
          TempNeed[MinCol] -= TempStock[MinRow];
          TempStock[MinRow] -= TempStock[MinRow];
        }
      else
        {
          MiddleMatrixArray[MinRow][MinCol].insert(MiddleMatrixArray[MinRow][MinCol].firstKey(),
                                                   TempNeed[MinCol]);
          TempStock[MinRow] -= TempNeed[MinCol];
          TempNeed[MinCol] -= TempNeed[MinCol];
        }
      for (int i = 0; i < TempStock.size() - 1; i++)
        {
          StockSum += TempStock[i];
        }
      for (int i = 0; i < TempNeed.size() - 1; i++)
        {
          NeedSum += TempNeed[i];
        }
    }
  RefreshTable(1, 1, "MinimalMethod"); // Оновити табличку без U та V
  for (int i = 0; i < m_RowSize; i++)
    {
      for (int j = 0; j < m_ColSize; j++)
        {
          if(MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey()) > EPS)
            {
              PlanCounter++;
            }
          TotalSum += MiddleMatrixArray[i][j].firstKey()
              * MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey());
        }
    }
  if(m_RowSize + m_ColSize - 1 == PlanCounter)
    {
      QMessageBox::information(this, "Метод потенціалів", "Знайдено опорний розв'язок. Він є невиродженим.");
    }
  else
    {
      QMessageBox::information(this, "Метод потенціалів", "Знайдено опорний розв'язок. Він є виродженим.");
    }
  return TotalSum;
}

void MainWindow::on_InitialReferencePlanButton_clicked()
{
  ui->tableWidget->blockSignals(true);
  if(!IsTaskClosed())
    {
      QMessageBox::critical(this, "Метод потенціалів", "Програма розрахована на задачу закритого типу. Перевірте будь-ласка дані, щоб вони відповідали вимогам.");
      ui->tableWidget->blockSignals(false);
      return;
    }
  ClearOptimalPlanLabel(MinimalElementMethod());
  ui->PotentialMethodButton->setEnabled(true);
  ui->tableWidget->blockSignals(false);
}

int MainWindow::FindMaxFilledCells()
{
  int LocalMaxFilledCellsCounter = 0;
  int MaxFilledCellsCounter = 0;
  int MaxIndex = 0;
  for (int i = 0; i < m_RowSize; i++)
    {
      LocalMaxFilledCellsCounter = 0;
      for (int j = 0; j < m_ColSize; j++)
        {
          if(MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey()) > EPS)
            {
              LocalMaxFilledCellsCounter++;
            }
        }
      if(MaxFilledCellsCounter < LocalMaxFilledCellsCounter)
        {
          MaxFilledCellsCounter = LocalMaxFilledCellsCounter;
          MaxIndex = i;
        }
    }
  return MaxIndex;
}

bool MainWindow::CalculateDelta()
{
  bool OptimalPlan = false;
  int CounterToFindDeltaMaxRowMaxCol = 0, AboveZeroDeltaCounter = 0;
  MaxDeltaRow = 0;
  MaxDeltaCol = 0;
  for (int i = 0; i < m_RowSize; i++)
    {
      for (int j = 0; j < m_ColSize; j++)
        {
          if(MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey()) < EPS
             && MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey()) > -EPS)
            {
              MiddleMatrixArray[i][j][MiddleMatrixArray[i][j].firstKey()]
                  = UVector[i] + VVector[j] - MiddleMatrixArray[i][j].firstKey();
              if(CounterToFindDeltaMaxRowMaxCol == 0)
                {
                  MaxDeltaRow = i;
                  MaxDeltaCol = j;
                  CounterToFindDeltaMaxRowMaxCol++;
                }
              //Шукаю максимальне дельта
              if(MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey())
                 > MiddleMatrixArray[MaxDeltaRow][MaxDeltaCol].value(MiddleMatrixArray[MaxDeltaRow][MaxDeltaCol].firstKey()))
                {
                  MaxDeltaRow = i;
                  MaxDeltaCol = j;
                }
              if(!FillCellBoolArray[i][j] &&
                 MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey()) > EPS)
                {
                  AboveZeroDeltaCounter++;
                }
            }
        }
    }
  if(AboveZeroDeltaCounter)
    {
      OptimalPlan = false;
    }
  else
    {
      OptimalPlan = true;
    }
  return OptimalPlan;
}

void MainWindow::CalculateUVVectors()
{
  int UCounter = 1, VCounter = 1, TimeOutCounter = 0;
  int MaxIndex = FindMaxFilledCells();
  QMap<float, float>::Iterator MapIterator;
  for (int i = 0; i < UVector.size(); i++)
    {
      UVector[i] = 0;
    }
  for (int i = 0; i < VVector.size(); i++)
    {
      VVector[i] = 0;
    }
  for (int i = 0; i < m_ColSize; i++)
    {
      if(MiddleMatrixArray[MaxIndex][i].value(MiddleMatrixArray[MaxIndex][i].firstKey()) > EPS)
        {
          VVector[i] = MiddleMatrixArray[MaxIndex][i].firstKey();
        }
    }
  while(UCounter != 0 || VCounter != 0)
    {
      UCounter = 0, VCounter = 0;
      for (int i = 0; i < m_RowSize; i++)
        {
          for (int j = 0; j < m_ColSize; j++)
            {
              if(FillCellBoolArray[i][j])
                {
                  if(VVector[j] > EPS)
                    {
                      UVector[i] = MiddleMatrixArray[i][j].firstKey() - VVector[j];
                    }
                  else if(fabs(UVector[i]) > EPS)
                    {
                      VVector[j] = MiddleMatrixArray[i][j].firstKey() - UVector[i];
                    }
                  if(TimeOutCounter > 4)
                    {
                      VVector[j] = MiddleMatrixArray[i][j].firstKey();
                    }
                }
            }
        }
      for (int i = 0; i < UVector.size(); i++)
        {
          if(fabs(UVector[i]) < EPS && i != MaxIndex)
            UCounter++;
        }
      for (int i = 0; i < VVector.size(); i++)
        {
          if(VVector[i] < EPS)
            VCounter++;
        }
      if(TimeOutCounter > 50)
        break;
      TimeOutCounter++;
    }
  for (int i = 0; i < m_RowSize; i++)
    {
      for (int j = 0; j < m_ColSize; j++)
        {
          MapIterator = MiddleMatrixArray[i][j].begin();
          if(!FillCellBoolArray[i][j])
            {
              MapIterator.value() = 0;
            }
        }
    }
  if(CalculateDelta())
    {
      int SummaryValue = 0;
      for (int i = 0; i < m_RowSize; i++)
        {
          for (int j = 0; j < m_ColSize; j++)
            {
              if(MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey()) > EPS)
                {
                  SummaryValue += MiddleMatrixArray[i][j].firstKey() *
                      MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey());
                }
            }
        }
       ClearOptimalPlanLabel(SummaryValue);
       QMessageBox::information(this, "Метод потенціалів",
                               "Оптимальний план знайдено. Значення = " + QString::number(SummaryValue));
        ui->CalculateCycleButton->setEnabled(false);
        ui->InitialReferencePlanButton->setEnabled(false);
        ui->EnterDataFromFileButton->setEnabled(false);
    }
  else
    ui->CalculateCycleButton->setEnabled(true);
}

void MainWindow::CalculateFillCellBoolArray()
{
  bool FillCellsCounter = false;
  for (int i = 0; i < m_RowSize; i++)
    {
      for (int j = 0; j < m_ColSize; j++)
        {
          if(FillCellBoolArray[i][j])
            {
              FillCellsCounter = true;
            }
        }
    }
  if(FillCellsCounter)
    {
      for (int i = 0; i < m_RowSize; i++)
        {
          for (int j = 0; j < m_ColSize; j++)
            {
              if((MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey()) > EPS
                 && FillCellBoolArray[i][j]) || (i == MaxDeltaRow && j == MaxDeltaCol))
                {
                  FillCellBoolArray[i][j] = true;
                }
            }
        }
    }
  else
    {
      for (int i = 0; i < m_RowSize; i++)
        {
          for (int j = 0; j < m_ColSize; j++)
            {
              if(MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey()) > EPS)
                {
                  FillCellBoolArray[i][j] = true;
                }
            }
        }
    }
}

void MainWindow::FoundPath(int Row, int Col)
{
  QMap<float, float> TempValueMap;
  QMap<int, int> TempCoordinatesMap;
  TempValueMap[MiddleMatrixArray[Row][Col].firstKey()] =
      MiddleMatrixArray[Row][Col].value(MiddleMatrixArray[Row][Col].firstKey());
  TempCoordinatesMap[Row] = Col;
  FillCellsVector.push_back(TempValueMap);
  FillCellsCoordinates.push_back(TempCoordinatesMap);
}

bool MainWindow::SearchRow(int Col)
{
  for (int i = 0; i < m_RowSize; i++)
    {
      if(FillCellBoolArray[i][Col] && !WasHere[i][Col])
        {
          WasHere[i][Col] = true;
          CycleCounter++;
          SearchCol(i);
        }
      if((i == MaxDeltaRow && Col == MaxDeltaCol && CycleCounter >= 3)
         || FoundCorrectPath)
        {
          FoundPath(i, Col);
          CorrectPath[i][Col] = true;
          FoundCorrectPath = true;
          return true;
        }
    }
  CycleCounter--;
  return false;
}

bool MainWindow::SearchCol(int Row)
{
  for (int i = 0; i < m_ColSize; i++)
    {
      if(FillCellBoolArray[Row][i] && !WasHere[Row][i])
        {
          WasHere[Row][i] = true;
          CycleCounter++;
          SearchRow(i);
        }
      if((i == MaxDeltaCol && Row == MaxDeltaRow && CycleCounter >= 3)
         || FoundCorrectPath)
        {
          FoundPath(Row, i);
          CorrectPath[Row][i] = true;
          FoundCorrectPath = true;
          return true;
        }
    }
  CycleCounter--;
  return false;
}

bool MainWindow::FindCycle(int Row, int Col)
{
  FoundCorrectPath = false;
  int TimeOut = 0;
  while(!FoundCorrectPath)
    {
      CycleCounter = 0;
      if(SearchCol(Row))
        {
          return true;
        }
      CycleCounter = 0;
      for (int i = 0; i < m_RowSize; i++)
        {
          for (int j = 0; j < m_ColSize; j++)
            {
              WasHere[i][j] = false;
            }
        }
      if(SearchRow(Col))
        {
          return true;
        }
      TimeOut++;
      if(TimeOut > m_RowSize * m_ColSize)
        {
          QMessageBox::warning(this, "Метод потенціалів", "Не вдалось знайти цикл.");
          break;
        }
    }
  return false;
}

void MainWindow::on_PotentialMethodButton_clicked()
{
  ui->tableWidget->blockSignals(true);
  FillInterfaceTable("PotentialMethodButton");
  RefreshTransportationCells();
  if(isFillCellBoolArrayEmpty)
    {
      CreateFillCellBoolArray();
    }
  CalculateFillCellBoolArray();
  CalculateUVVectors();
  RefreshTable(2, 2, "PotentialMethodButton");
  ui->PotentialMethodButton->setEnabled(false);
  ui->tableWidget->blockSignals(false);
}

void MainWindow::RefreshTransportationCells()
{
  QMap<float, float> Min;
  QMap<int, int> MinCoordinates;
  QMap<int, int>::Iterator MinCoordinatesIterator;
  QMap<float, float>::Iterator MapIterator;
  QMap<int, int>::Iterator MapCoordinatesIterator;
  if(!isCorrectPathEmpty)
    {
      for (int i = 0; i < FillCellsVector.size(); i++)
        {
          MapIterator = FillCellsVector[i].begin();
          if(i % 2) // якщо непарне значення
            {
              MapIterator.value() *= -1;
            }
        }
      for (int i = 0; i < FillCellsVector.size(); i++)
        {
          MapIterator = FillCellsVector[i].begin();
          MapCoordinatesIterator = FillCellsCoordinates[i].begin();
          if(MapIterator.value() < EPS)
            {
              Min.clear();
              MinCoordinates.clear();
              Min[MapIterator.key()] = MapIterator.value();
              MinCoordinates[MapCoordinatesIterator.key()] = MapCoordinatesIterator.value();
              break;
            }
        }
      for (int i = 0; i < FillCellsVector.size(); i++)
        {
          MapIterator = FillCellsVector[i].begin();
          MapCoordinatesIterator = FillCellsCoordinates[i].begin();
          if(MapIterator.value() < EPS)
            {
              if(fabs(MapIterator.value()) < fabs(Min.value(Min.firstKey())))
                {
                  Min.clear();
                  MinCoordinates.clear();
                  Min[MapIterator.key()] = MapIterator.value();
                  MinCoordinates[MapCoordinatesIterator.key()] = MapCoordinatesIterator.value();
                }
            }
        }
      MinCoordinatesIterator = MinCoordinates.begin();
      for (int i = 0; i < FillCellsVector.size(); i++)
        {
          MapIterator = FillCellsVector[i].begin();
          MapCoordinatesIterator = FillCellsCoordinates[i].begin();
          if(MapIterator.key() == Min.firstKey()
             && MapCoordinatesIterator.key() == MinCoordinatesIterator.key()
             && MapCoordinatesIterator.value() == MinCoordinatesIterator.value())
            {
              MapIterator.value() = 0;
            }
          else if(MapIterator.key() == MiddleMatrixArray[MaxDeltaRow][MaxDeltaCol].firstKey()
                  && MapCoordinatesIterator.key() == MaxDeltaRow
                  && MapCoordinatesIterator.value() == MaxDeltaCol)
            {
              MapIterator.value() = fabs(Min.value(Min.firstKey()));
            }
          else if(MapIterator.value() < EPS)
            {
              MapIterator.value() = fabs(fabs(MapIterator.value())
                                         - fabs(Min.value(Min.firstKey())));
            }
          else if(MapIterator.value() > EPS)
            {
              MapIterator.value() = MapIterator.value() + fabs(Min.value(Min.firstKey()));
            }
        }
      for (int i = 0; i < m_RowSize; i++)
        {
          for (int j = 0; j < m_ColSize; j++)
            {
              MapIterator = MiddleMatrixArray[i][j].begin();
              //Може виникнути фігня, коли в масиві зберігаються значення з однаковим ключем і вони заповнені, тоді обом вставиться останній
              for(int k = 0; k < FillCellsVector.size(); k++)
                {
                  MapCoordinatesIterator = FillCellsCoordinates[k].begin();
                  if(FillCellsVector[k].firstKey() ==
                     MapIterator.key() &&
                     MapCoordinatesIterator.key() == i && MapCoordinatesIterator.value() == j)
                    {
                      MapIterator.value() = FillCellsVector[k].value(FillCellsVector[k].firstKey());
                    }
                }
            }
        }
    }
  else
    {
      return;
    }
}

void MainWindow::on_CalculateCycleButton_clicked()
{
  ui->tableWidget->blockSignals(true);
  FillCellsVector.clear();
  FillCellsCoordinates.clear();
  CycleCounter = 0; // Змінна яка рахує чи цикл знайдений. Якщо вона є більша або рівна 3, то цикл знайдено.
  if(isCorrectPathEmpty)
    {
      CreateCorrectPathArray();
    }
  else
    {
      DeleteArrays("CorrectPath");
      CreateCorrectPathArray();
    }
  if(isWasHereEmpty)
    {
      CreateWasHereArray();
    }
  else
    {
      DeleteArrays("WasHere");
      CreateWasHereArray();
    }
  FindCycle(MaxDeltaRow, MaxDeltaCol);
  for (int i = 0; i < m_RowSize; i++)
    {
      for (int j = 0; j < m_ColSize; j++)
        {
          if(CorrectPath[i][j])
            {
              for (int k = 0; k < FillCellsCoordinates.size(); k++)
                {
                  if(FillCellsCoordinates[k].firstKey() == i
                     && FillCellsCoordinates[k].value(FillCellsCoordinates[k].firstKey()) == j)
                    {
                      if(k % 2)
                        ui->tableWidget->item(i + 2, j + 2)->setText(
                              QString::number(MiddleMatrixArray[i][j].firstKey()) + '/'
                              + QString::number(MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey())) + '-');
                      else
                        ui->tableWidget->item(i + 2, j + 2)->setText(
                              QString::number(MiddleMatrixArray[i][j].firstKey()) + '/'
                              + QString::number(MiddleMatrixArray[i][j].value(MiddleMatrixArray[i][j].firstKey())) + '+');
                      ui->tableWidget->item(i + 2, j + 2)->setBackground(Qt::cyan);
                    }
                }
            }
        }
    }
  ui->PotentialMethodButton->setEnabled(true);
  ui->CalculateCycleButton->setEnabled(false);
  ui->tableWidget->blockSignals(false);
}

