#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QDebug>
#include <QProcess>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QPixmap>
#include <QDir>
#include <QSplitter>
#include <QList>
#include <QStringList>
#include <QStatusBar>
#include <QToolBar>
#include <QListWidget>
#include <QResizeEvent>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QJsonObject>
#include <QStandardPaths>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();

    QToolBar* toolBar = new QToolBar("功能栏",this);
    this->addToolBar(toolBar);

    QPushButton *creatAlbumBtn = new QPushButton("创建相册",this);
    QPushButton *addPhotoBtn = new QPushButton("添加照片",this);
    QPushButton *thumbSmallBtn = new QPushButton("小缩略图",this);
    toolBar->addWidget(creatAlbumBtn);
    toolBar->addWidget(addPhotoBtn);
    toolBar->addWidget(thumbSmallBtn);

    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal,this);
    this->setCentralWidget(mainSplitter);

    QListWidget *albumListWidget = new QListWidget(mainSplitter);
    albumListWidget->addItem("默认相册");
    albumListWidget->setMinimumWidth(120);
    albumListWidget->setMaximumWidth(240);
    albumListWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    for(const auto& pair : this->album){
        albumListWidget->addItem(pair.first+","+pair.second);
    }


    QWidget *rightArea = new QWidget(mainSplitter);
    rightArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QGridLayout *thumbLayout = new QGridLayout(mainSplitter);
    rightArea->setLayout(thumbLayout);


    mainSplitter->setStretchFactor(0,1);
    mainSplitter->setStretchFactor(1,4);


    QStatusBar *statusBar = new QStatusBar(this);
    statusBar->showMessage("就绪");
    this->setStatusBar(statusBar);

}
#include <QJsonArray>
void MainWindow::init(){
    QString path(QCoreApplication::applicationDirPath());
    QFile file(path+"/data/album.json");
    if(!file.exists()){
        return;
    }
    if(!file.open(QIODevice::ReadOnly)){
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.read(file.size()));
    if(doc.isNull()){
        qDebug() << "JSON文件格式有误";
        return;
    }
    QJsonObject obj = doc.object();//获取顶层对象
    if(obj.contains("album")){
        QJsonObject album = obj["album"].toObject();
        auto begin = album.begin();
        auto end = album.end();
        while(begin < end){
            this->album.push_back({begin.key(), begin.value().toString()});
            ++begin;
        }
    }
}

void MainWindow::close(){
    QJsonObject root;
    QString path(QCoreApplication::applicationDirPath());
    QFile file(path+"/data/album.json");//不要试图使用QDir的相关方法，他们不会修改对象本身

    QJsonObject tar;
    /*
     * tar[""],""中的值不同，堆放的是同级的json数据
     */
    for(const std::pair<QString, QString>& p : this->album){
        tar[p.first] = p.second;
    }
    root["album"] = tar;

    if(!file.exists()){
        if(!file.open(QIODevice::NewOnly)){//只是以创建的方式打开文件
            qDebug() << file.errorString();
        }
    }
    if(!file.open(QIODevice::WriteOnly)){//只是以写入的方式打开文件
        qDebug() << file.errorString();
        return;
    }
    file.write(QJsonDocument(root).toJson());
}

MainWindow::~MainWindow()
{
    close();
    delete ui;
}
