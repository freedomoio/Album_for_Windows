#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <qDebug>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_CancelButton_clicked()
{
    QPixmap pixmap;

    QString exeDir = QCoreApplication::applicationDirPath();
    QString parentDir = QDir(exeDir).absoluteFilePath("../../../");
    QString photoDir = QDir::cleanPath(parentDir + "/resources/images/");
    QString photoPath = photoDir + "/1.jpg";

    qDebug() << "最终图片路径：" << photoPath;

    if(pixmap.load(photoPath)) {

        ui->label_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ui->label_2->setScaledContents(false);
        ui->label_2->setAlignment(Qt::AlignCenter);

        connect(this, &MainWindow::resizeEvent, this, [=](QResizeEvent *event){
            Q_UNUSED(event);
            QPixmap scaledPix = pixmap.scaled(
                ui->label_2->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
            ui->label_2->setPixmap(scaledPix);
        });

        QPixmap scaledPix = pixmap.scaled(
            ui->label_2->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );
        ui->label_2->setPixmap(scaledPix);
    } else {
        QMessageBox::information(this,"信息","未找到文件，路径：" + photoPath);
    }
}


