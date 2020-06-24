#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    g_thread_ImageHandle = new Thread_ImageHandle(); //实时显示线程的对象
    g_thread_ImageShow = new Thread_Image_Show();    //单独显示线程的对象
    g_PylonCamera = new PylonCamera();               //相机对象
    g_thread_ImageShow->MotorIsRunFlag = 1;

    //定时器
    timer = new QTimer();

    //横坐标与纵坐标坐标
    axisX1 = new QCategoryAxis();
    axisY1 = new QCategoryAxis();
    axisX2 = new QCategoryAxis();
    axisY2 = new QCategoryAxis();

    //相机初始化
    if(g_PylonCamera->camera_init())
    {

    }

    //创建一个新的文件，命名为当前时间
    initFile();

    //放折线图的
    QVBoxLayout *vLayout_1 = ui->verticalLayout_1;
    QVBoxLayout *vLayout_2 = ui->verticalLayout_2;

    //折线图1、2
    chart1 = new QChart();
    chart2 = new QChart();

    //实例化折线图窗口2
    chartView = new QChartView(createLineChart2());
    //折线反走样
    chartView->setRenderHint(QPainter::Antialiasing);
    //实例化折线2
    lineOfChar2 = new QLineSeries();
    //初始化线条
    initLine();
    //添加线条
    chart2->addSeries(lineOfChar2);
    lineOfChar2->attachAxis(axisX2);
    lineOfChar2->attachAxis(axisY2);
    vLayout_2->addWidget(chartView);

    //实例化折线图窗口1
    chartView = new QChartView(createLineChart1());
    for(int i=0; i<linesOfChar1.size();i++)
    {
        chart1->addSeries(linesOfChar1[i]);
        linesOfChar1[i]->attachAxis(axisX1);
        linesOfChar1[i]->attachAxis(axisY1);
    }
    chartView->setRenderHint(QPainter::Antialiasing);
    vLayout_1->addWidget(chartView);

    //选择框
    GroupBoxOfChecks = new QGroupBox(ui->centralWidget);
    vBox = new QVBoxLayout();
    GroupBoxOfChecks->setLayout(vBox);
    GroupBoxOfChecks->setTitle("选择框");
    GroupBoxOfChecks->setGeometry(320,0,100,860);
    addCheckBoxToGroupBox(axisX2->categoriesLabels());

    //该定时器检测哪个checkBox被勾选
    connect(timer,SIGNAL(timeout()),this,SLOT(getCheckBoxChangeInfo()));
    timer->start(50);
}

MainWindow::~MainWindow()
{
    if(csvFile.isOpen())
    {
        csvFile.close();
    }

    delete ui;
}

void MainWindow::getData(float result)
{
    //运动时更新二号图
    if(g_thread_ImageShow->MotorIsRunFlag == 1)
    {
        if(index < axisX2->count())
        {
            upDateChart2(Data(index,result));
            m_datalist << Data(index,result);
            index ++;
            qDebug() <<Data(index,result);
        } else {
            index = 0;
            QMessageBox::warning(NULL,"警告","超出监测次数");
        }
    } else { //运动停止更新一号图
        if(!m_datalist.empty())
        {
            m_dataTable << m_datalist;
            for(int i=0; i<m_datalist.size(); i++)
            {
                QString data = QString::number(m_datalist[i].y(),10,5);
                csvFile.write(data.toStdString().data());
                csvFile.write(",");
            }
            csvFile.write("\n");
            m_datalist.clear();
            upDateChart1();
        }
        lineOfChar2->clear();
        index = 0;
    }
}

//更新多次检测的折线
void MainWindow::upDateChart1()
{
    //先清除
    for(int i =0;i<linesOfChar1.size();i++)
    {
        linesOfChar1[i]->clear();
    }
    int num = 0;
    //在添加
    for(int i=0; i< m_dataTable.size(); i++)
    {

        for(int j=0; j<m_dataTable[i].size();j++)
        {
           //qDebug()<<m_dataTable[i];
            *linesOfChar1[m_dataTable[i][j].rx()] << QPointF(num,m_dataTable[i][j].ry());

        }
        num++;
    }
}

//更新单次检测的折线
void MainWindow::upDateChart2(QPointF newData)
{
    *lineOfChar2 << newData;
}

void MainWindow::initLine()
{
    //折线一
    for(int i = 0; i< axisX2->count();i++)
    {
        QLineSeries *line = new QLineSeries();
        //设置标签名
        line->setName(QString("Line")+QString::number(i));
        //设置颜色
        //QColor qc=QColor::fromHsl(rand()%360,rand()%256,rand()%200);
        //line->setColor(qc);
        //点标签是否可视
        line->setPointLabelsVisible(true);
        //点标签颜色
        line->setPointLabelsColor(QColor(0,50,70));
        //点标签字体
        line->setPointLabelsFont(QFont("微软雅黑"));
        //点标签格式
        line->setPointLabelsFormat("@yPoint");
        //是否切割边缘点标签，默认为true
        line->setPointLabelsClipping(false);
        //是否可视
        line->setPointsVisible(true);
        //将线条加入图一
        linesOfChar1.push_back(line);
    }
    qDebug()<<"图一的线条个数="<< axisX2->count();

    //折线二
    {
        QPen pen;
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(2);
        pen.setColor(QColor(21,100,255));

        lineOfChar2->setPen(pen);
        //点标签是否可视
        lineOfChar2->setPointLabelsVisible(true);
        //点标签颜色
        lineOfChar2->setPointLabelsColor(QColor(0,50,70));
        //点标签字体
        lineOfChar2->setPointLabelsFont(QFont("微软雅黑"));
        //点标签格式
        lineOfChar2->setPointLabelsFormat("@yPoint");
        //是否切割边缘点标签，默认为true
        lineOfChar2->setPointLabelsClipping(false);
        //是否可视
        lineOfChar2->setPointsVisible(true);

        //chart2->addSeries(lineOfChar2);
    }
}

QChart *MainWindow::createLineChart1() const
{
    //设置主题
    /*
     *  ChartThemeLight = 0,
        ChartThemeBlueCerulean,
        ChartThemeDark,
        ChartThemeBrownSand,
        ChartThemeBlueNcs,
        ChartThemeHighContrast,
        ChartThemeBlueIcy,
        ChartThemeQt

        if (theme == QChart::ChartThemeLight) {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeDark) {
            pal.setColor(QPalette::Window, QRgb(0x121218));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBlueCerulean) {
            pal.setColor(QPalette::Window, QRgb(0x40434a));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBrownSand) {
            pal.setColor(QPalette::Window, QRgb(0x9e8965));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeBlueNcs) {
            pal.setColor(QPalette::Window, QRgb(0x018bba));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeHighContrast) {
            pal.setColor(QPalette::Window, QRgb(0xffab03));
            pal.setColor(QPalette::WindowText, QRgb(0x181818));
        } else if (theme == QChart::ChartThemeBlueIcy) {
            pal.setColor(QPalette::Window, QRgb(0xcee7f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        }
    */

    QChart::ChartTheme theme = QChart::ChartThemeBlueIcy;
    chart1->setTheme(theme);
    QPalette pal = window()->palette();
    pal.setColor(QPalette::Window, QRgb(0xcee7f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    window()->setPalette(pal);

    //显示线代表值
    chart1->legend()->setVisible(false);
    chart1->legend()->setAlignment(Qt::AlignRight);
    //chart1->legend()->setAlignment();//底部对齐

    //设置横坐标
    axisX1->setMin(0);
    axisX1->setMax(29);
    axisX1->setStartValue(0);

    for(int i=0; i<30; i++)
    {
        axisX1->append(QString::number(i+1),i);
    }

    //刻度线与刻度label对齐
    axisX1->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisX1->setTitleText(tr("测试次数"));

    //设置纵坐标
    axisY1->setRange(0,100);
    axisY1->setTickCount(11);
    axisY1->append("0%",0);
    axisY1->append("20%",20);
    axisY1->append("40%",40);
    axisY1->append("60%",60);
    axisY1->append("80%",80);
    axisY1->append("100%",100);
    //刻度线与刻度label对齐
    axisY1->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisY1->setTitleText(tr("实际值"));

    //隐藏网格线
    axisX1->setGridLineVisible(false);
    axisY1->setGridLineVisible(false);

    //添加到chart
    chart1->setAxisX(axisX1);
    chart1->setAxisY(axisY1);
    chart1->setTitle("检测趋势图");

    return chart1;
}

QChart *MainWindow::createLineChart2() const
{
    //设置主题
    QChart::ChartTheme theme = QChart::ChartThemeBlueIcy;
    chart2->setTheme(theme);
    QPalette pal = window()->palette();
    pal.setColor(QPalette::Window, QRgb(0xcee7f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    window()->setPalette(pal);

    //设置横坐标
    axisX2->setLabelsAngle(90);
    axisX2->setMin(0);
    axisX2->setMax(28);
    axisX2->setStartValue(0);
    axisX2->append("0%",0);
    axisX2->append("1%",1);
    axisX2->append("2%",2);
    axisX2->append("3%",3);
    axisX2->append("4%",4);
    axisX2->append("5%",5);
    axisX2->append("10%",6);
    axisX2->append("15%",7);
    axisX2->append("20%",8);
    axisX2->append("25%",9);
    axisX2->append("30%",10);
    axisX2->append("35%",11);
    axisX2->append("40%",12);
    axisX2->append("45%",13);
    axisX2->append("50%",14);
    axisX2->append("55%",15);
    axisX2->append("60%",16);
    axisX2->append("65%",17);
    axisX2->append("70%",18);
    axisX2->append("75%",19);
    axisX2->append("80%",20);
    axisX2->append("85%",21);
    axisX2->append("90%",22);
    axisX2->append("95%",23);
    axisX2->append("96%",24);
    axisX2->append("97%",25);
    axisX2->append("98%",26);
    axisX2->append("99%",27);
    axisX2->append("100%",28);

    //刻度线与刻度label对齐
    axisX2->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisX2->setTitleText(tr("预计值"));

    //设置纵坐标
    axisY2->setRange(0,100);
    axisY2->setTickCount(11);
    axisY2->append("0%",0);
    axisY2->append("20%",20);
    axisY2->append("40%",40);
    axisY2->append("60%",60);
    axisY2->append("80%",80);
    axisY2->append("100%",100);
    //刻度线与刻度label对齐
    axisY2->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisY2->setTitleText(tr("实际值"));

    //隐藏网格线
    axisX2->setGridLineVisible(false);
    axisY2->setGridLineVisible(false);

    //添加到chart
    chart2->setAxisX(axisX2);
    chart2->setAxisY(axisY2);
    chart2->setTitle("动态折线图");

    //设置legend
    chart2->legend()->setVisible(true);

    return chart2;
}

//添加CheckBox到GroupBox
void MainWindow::addCheckBoxToGroupBox(QStringList name)
{
    //先清空内容
    for(int i=0; i< CheckBoxList.size(); i++)
    {
        vBox->removeWidget(CheckBoxList[i]);
        CheckBoxList[i]->deleteLater();
    }
    CheckBoxList.clear();
    csvFile.write("\n");
    //再重新添加到list
    for(int j=0; j<name.size(); j++)
    {
        QCheckBox *checkBox = new QCheckBox();
        checkBox->setChecked(false);
        checkBox->setText(name[j]);
        CheckBoxList.append(checkBox);
        //写入文件中
        csvFile.write(name[j].toStdString().data());
        csvFile.write(",");
    }
    csvFile.write("\n");
    //添加到界面
    for(int j=0; j<CheckBoxList.size();j++)
    {
        vBox->addWidget(CheckBoxList[j]);
    }
}


void MainWindow::initFile()
{
    //读写文件data.csv
    QDateTime current_time = QDateTime::currentDateTime();
    QString time = current_time.toString("MM-dd-hh-mm-ss");
    QString fileName = QString("./data/%1.csv").arg(time);

    qDebug() <<fileName;
    csvFile.setFileName(fileName);
    if(csvFile.exists())
    {
        csvFile.open(QIODevice::ReadWrite);
        qDebug()<<"open success";
    } else {
        qDebug()<<"not find file";
        csvFile.open(QIODevice::ReadWrite|QIODevice::Text);
    }
}

void MainWindow::getCheckBoxChangeInfo()
{
    for(int i=0; i<CheckBoxList.size(); i++)
    {
        //显示
        if(!CheckBoxList[i]->isChecked())
        {
            linesOfChar1[i]->hide();
        } else { //隐藏
            linesOfChar1[i]->show();
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
  //自动添加数据
 /*vector<float> nums = {0,1,2,3,4,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,96,97,98,99,100};
 for(int j=0;j<10;j++)
 {
     g_thread_ImageShow->MotorIsRunFlag = 1;
     qDebug()<<"aixs_"<<axisX2->count();
     for(int i=0; i<axisX2->count(); i++)
     {
         if(i==28)
         {
             getData(nums[i]);
         }
         else{
             getData(nums[i]+(0.1 * test_x));
         }

     }

     g_thread_ImageShow->MotorIsRunFlag = 0;
     getData(nums[0]+0.1*test_x);

     test_x++;
 }*/
    //一个一个添加数据
    getData(ui->lineEditGetData->text().toFloat());

}

//模拟电机运行完成：g_thread_ImageShow = 1;
//电机运行中：g_thread_ImageShow = 0;
void MainWindow::on_pushButton_2_clicked()
{
    if(g_thread_ImageShow->MotorIsRunFlag == 1)
    {
        g_thread_ImageShow->MotorIsRunFlag = 0;
    } else {
        g_thread_ImageShow->MotorIsRunFlag = 1;
    }
    qDebug() <<g_thread_ImageShow->MotorIsRunFlag;
}

//添加测试网版密度
void MainWindow::on_btn_addMeshDensity_clicked()
{
    nameOfAixs.append(ui->lineEdit->text()+"%");
    ui->textEdit->append(ui->lineEdit->text()+"%");
}

//清空text内容
void MainWindow::on_btn_clearText_clicked()
{
    nameOfAixs.clear();
    ui->textEdit->clear();
}

//当密度值改变改变，1.改变折线图二的横坐标轴
//2.改变groupBox里面的checkBox
void MainWindow::on_btn_changeMeshDensity_clicked()
{
    //折线二清空
    lineOfChar2->clear();

    //读取折线图二原X轴坐标的数据
    QStringList oldList = axisX2->categoriesLabels();
    //qDebug() << oldList;

    /************重新设置X轴的坐标点**********************/
    chart2->removeAxis(axisX2);
    axisX2->setMin(0);
    axisX2->setMax(nameOfAixs.size()-1);
    axisX2->setStartValue(0);

    for(int i=0; i<oldList.size(); i++)
    {
        axisX2->remove(oldList[i]);
    }
    for(int i=0;i<nameOfAixs.size();i++)
    {
       // qDebug() << oldList[i] << nameOfAixs[i];
        axisX2->append(nameOfAixs[i],i);
    }
    axisX2->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    chart2->setAxisX(axisX2);
    lineOfChar2->attachAxis(axisX2);
    lineOfChar2->attachAxis(axisY2);
    /****************************************************/

    /**********************清空折线图一的折线******************************/
    for(int i =0;i<linesOfChar1.size();i++)
    {
        linesOfChar1[i]->clear();
    }
    //清空数据
    m_dataTable.clear();

    initLine();
    addCheckBoxToGroupBox(nameOfAixs);
}

void MainWindow::on_btn_deleteFile_clicked()
{
    QString path = "./data";
    QDir Dir(path);
        if(Dir.isEmpty())
        {
            qDebug() << "临时文件文件夹" << path << "为空";
            return;
        }

        // 第三个参数是QDir的过滤参数，这三个表示收集所有文件和目录，且不包含"."和".."目录。
        // 因为只需要遍历第一层即可，所以第四个参数填QDirIterator::NoIteratorFlags
        QDirIterator DirsIterator(path);
        while(DirsIterator.hasNext())
        {
            if (!Dir.remove(DirsIterator.next())) // 删除文件操作如果返回否，那它就是目录
            {
                QDir(DirsIterator.filePath()).removeRecursively(); // 删除目录本身以及它下属所有的文件及目录
            }
        }
}
