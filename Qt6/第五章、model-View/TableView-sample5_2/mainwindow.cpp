#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"QLabel"
#include"QStandardItem"
#include "QStandardItemModel"
//#include"qstandarditemmodel.h"
#include"QItemSelectionModel"
#include"QFileDialog"
#include"QFile"
#include"QRegularExpression"

void MainWindow::initModelData(QStringList &aFileContent)
{
    int rowCnt=aFileContent.size();
    model->setRowCount(rowCnt-1);//第一行是表头

    QString header=aFileContent.at(0);//获得表头

    QStringList headerList=header.split(QRegularExpression(R"(\s)"),Qt::SkipEmptyParts);
    int j;
    QStandardItem *aItem;
    model->setHorizontalHeaderLabels(headerList);
    for(int i=1;i<rowCnt;i++){
        QString aLineText=aFileContent.at(i);
        QStringList tmpList=aLineText.split(QRegularExpression(R"(\s)"),Qt::SkipEmptyParts);
        for(j=0;j<FixedColumnCount-1;j++){
            aItem=new QStandardItem(tmpList.at(j));
            model->setItem(i-1,j,aItem);
        }
        aItem=new QStandardItem(headerList.at(j));
        aItem->setCheckable(true);
        aItem->setBackground(QBrush(Qt::yellow));
        if(tmpList.at(j)=="0")
            aItem->setCheckState(Qt::Unchecked);
        else
            aItem->setCheckState(Qt::Checked);
        model->setItem(i-1,j,aItem);
    }
}

void MainWindow::do_currentChanged(const QModelIndex &current, const QModelIndex &previon)
{
    Q_UNUSED(previon);
    if(current.isValid()){
        labCellPos->setText(QString::asprintf("当前单元格：%d 行，%d列",current.row(),current.column()));
QStandardItem *aItem=model->itemFromIndex(current);
        labCellText->setText("单元内容："+aItem->text());

        ui->act_Borld->setCheckable(aItem->font().bold());
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->act_Borld->setCheckable(true);
    labCurFile=new QLabel("当前文件",this);
    labCurFile->setMinimumWidth(200);
    labCellPos=new QLabel("当前单元格",this);
    labCellPos->setMinimumWidth(200);
    labCellText=new QLabel("单元格内容",this);
    labCellText->setMinimumWidth(200);
    ui->statusbar->addWidget(labCurFile);
    ui->statusbar->addWidget(labCellText);
    ui->statusbar->addWidget(labCellPos);
    model=new QStandardItemModel(2,FixedColumnCount,this);
    SelecModel=new QItemSelectionModel(model,this);
    ui->tableView->setModel(model);
    ui->tableView->setSelectionModel(SelecModel);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    connect(SelecModel,&QItemSelectionModel::currentChanged,this,&MainWindow::do_currentChanged);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_act_openFile_triggered()
{
    QString curPath = QCoreApplication::applicationDirPath();
    QString aFileName = QFileDialog::getOpenFileName(this, "打开一个文件", curPath, "数据文件(*.txt);;所有文件(*.*)");
    if (aFileName.isEmpty())
        return;
    QFile aFile(aFileName);
    if (aFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->plainTextEdit->clear();
        QTextStream aStream(&aFile);
        QStringList aFileContent;
        while (!aStream.atEnd())
        {
            QString str = aStream.readLine();
            ui->plainTextEdit->appendPlainText(str);
            aFileContent.append(str);
        }
        aFile.close();
        labCurFile->setText("当前文件： "+aFileName);
        ui->act_AddRow->setEnabled(true);
        ui->act_InsertRow->setEnabled(true);
        ui->act_deleteRow->setEnabled(true);


        initModelData(aFileContent);
    }
}

void MainWindow::on_act_ShowData_triggered()
{
    //tableview是有数据的，直接读取tableview的数据
    ui->plainTextEdit->clear();
    QStandardItem *aItem;
    QString str;
     //读取表头
    for(int i=0;i<model->columnCount();i++){
        aItem=model->horizontalHeaderItem(i);
        str+=aItem->text();
        str+="\t";
    }
    //输出表头
    ui->plainTextEdit->appendPlainText(str);

    //读取数据
    for(int i=0;i<model->rowCount()-1;i++)
    {
        str="";
        for( int j=0;j<model->columnCount()-1;j++){
            aItem=model->item(i,j);
            str+=aItem->text();
            str+="\t";
        }
        aItem=model->item(i,FixedColumnCount-1);
        if(aItem->checkState()==Qt::Checked)
            str+="1";
            else
                str+="0";
            //添加每行数据
            ui->plainTextEdit->appendPlainText(str);
    }
}




void MainWindow::on_act_AddRow_triggered()
{
    QList<QStandardItem*> aItemList;
    QStandardItem *aItem;
    for(int i=0;i<model->columnCount()-1;++i){
            aItem=new QStandardItem("0");
            aItemList<<aItem;
    }
    QString str=model->headerData(model->columnCount()-1,Qt::Horizontal).toString();


    aItem=new QStandardItem(str);
    aItem->setCheckable(true);
    aItem->setBackground(QBrush(Qt::yellow));
    aItemList<<aItem;
    model->insertRow(model->rowCount(),aItemList);
    SelecModel->clearSelection();
    SelecModel->setCurrentIndex(model->index(model->rowCount()-1,0),QItemSelectionModel::Select);
}


void MainWindow::on_act_InsertRow_triggered()
{
    //存放新行的数据项
    QList<QStandardItem*> aItemList;
    QStandardItem *aItem;
    //创建空的Item放进AItemList
    for(int i=0;i<model->columnCount()-1;++i){
            aItem=new QStandardItem("0");
            aItemList<<aItem;
    }

    //获取最后一个表头的数据

    QString str=model->headerData(model->columnCount()-1,Qt::Horizontal).toString();

     //最后一项单独考虑、用前一项的表头初始化
    aItem=new QStandardItem(str);
    aItem->setCheckable(true);
    aItem->setBackground(QBrush(Qt::yellow));
    //最后一项写进List
    aItemList<<aItem;
   //添加一行
    model->insertRow(SelecModel->currentIndex().row(),aItemList);
    SelecModel->clearSelection();
    SelecModel->setCurrentIndex(SelecModel->currentIndex(),QItemSelectionModel::Select);
}


void MainWindow::on_act_deleteRow_triggered()
{
    QModelIndex curIndex=SelecModel->currentIndex();
    model->removeRow(curIndex.row());
    if(curIndex.row()!=model->rowCount())
            SelecModel->setCurrentIndex(curIndex,QItemSelectionModel::Select);

}


void MainWindow::on_act_Center_triggered()
{
    if(SelecModel->hasSelection()==0)
            return ;
    QModelIndexList indexList=SelecModel->selectedIndexes();
    for(auto aIndex:indexList)
    {
            model->itemFromIndex(aIndex)->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);

    }
}


void MainWindow::on_act_left_triggered()
{
    if(SelecModel->hasSelection()==0)
            return ;
    QModelIndexList indexList=SelecModel->selectedIndexes();
    for(auto aIndex:indexList)
    {
            model->itemFromIndex(aIndex)->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    }
}


void MainWindow::on_act_Borld_triggered(bool checked)
{
    if(SelecModel->hasSelection()==0)
            return ;
    //将所选的项加入列表
    QModelIndexList indexList=SelecModel->selectedIndexes();
    int i=0;
    for(auto aIndex:indexList)
    {
            //取出当前的字体
            QFont font=model->itemFromIndex(aIndex)->font();
           // qDebug()<<"项 "<<i++<<"\n";
            //根据所选性设置粗体
            font.setBold(checked);
            //设置字体
            model->itemFromIndex(aIndex)->setFont(font);

    }
}

