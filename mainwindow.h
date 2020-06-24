#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartGlobal>
#include <global.h>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QCategoryAxis>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QMessageBox>
#include <qtimer.h>
#include <qfile.h>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>

#pragma once
#pragma execution_character_set("utf-8")

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

//点数据
typedef QPointF Data;
//线数据
typedef QList<Data> DataList;
//图数据
typedef QList<DataList> DataTable;


QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //从线程中中获取结果数据，添加到折线图中
    void getData(float result);

    //
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    //重新添加测试网版密度
    void on_btn_addMeshDensity_clicked();

    //清除文件
    void on_btn_clearText_clicked();

    //改变网板密度
    void on_btn_changeMeshDensity_clicked();

private:
    //更新单次测量数据
    void upDateChart1();

    //更新多次测量后的数据
    void upDateChart2(QPointF newData);

    //初始化两个图中的线条
    void initLine();

    //创建全局的折线图
    QChart *createLineChart1() const;

    //创建单次测量的折线图
    QChart *createLineChart2() const;

    //根据用户需求添加CheckBox到界面中
    void addCheckBoxToGroupBox(QStringList AxisName);

    //寻找文件文本
    void initFile();

private:
    Ui::MainWindow *ui;
    //X轴的每个坐标名称
    QStringList nameOfAixs;
    //全局折线图的数据，格式为{ {(线1的x轴坐标，值)，（线1的x轴坐标，值），（线1的x轴坐标，值），（线1的x轴坐标，值）}，
                        //  {(线2的x轴坐标，值)，（线2的x轴坐标，值），（线2的x轴坐标，值），（线1的2轴坐标，值）}，...
                        //}
    DataTable m_dataTable;
    //chartView
    QChartView *chartView;
    //全局测量折线图
    QChart *chart1;
    //单次测量折线图
    QChart *chart2;

    //全局测量折线图的横坐标与纵坐标
    QCategoryAxis *axisX1;
    QCategoryAxis *axisY1;

    //单次测量折线图的横坐标与纵坐标
    QCategoryAxis *axisX2;
    QCategoryAxis *axisY2;

    //单次测量的折线
    QLineSeries *lineOfChar2;

    //多次测量的所有折线
    QList<QLineSeries *> linesOfChar1;

    //
    QGroupBox *GroupBoxOfChecks;

    //放到GroupBox中
    QVBoxLayout *vBox;

    //勾选框
    QList<QCheckBox *> CheckBoxList;

    //
    int index = 0;

    //当前网板的测量次数
    int measureCount = 0;

    //记录单个点数据，格式为（x轴坐标，值）
    Data data;

    //记录单次测试的数据 ，格式为{（x轴坐标0，值），（x轴坐标1，值），（x轴坐标2，值），（x轴坐标3，值）...}
    DataList m_datalist;

    float test_x;

    //定时器，检测checkbox是否勾选
    QTimer *timer;

    //文本
    QFile csvFile;

private slots:
    //获取勾选结果，显示勾选了的折线
    void getCheckBoxChangeInfo();

    void on_btn_deleteFile_clicked();
};

#endif // MAINWINDOW_H
