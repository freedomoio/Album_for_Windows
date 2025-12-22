#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QDebug>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();

    //初始化尺寸
    this ->resize(1000,600);

    //工具栏
    QToolBar* toolBar = new QToolBar("功能栏",this);
    this->addToolBar(toolBar);

    QPushButton *creatAlbumBtn = new QPushButton("创建相册",this);
    QPushButton *addPhotoBtn = new QPushButton("添加照片",this);
    QPushButton *thumbSmallBtn = new QPushButton("小缩略图",this);
    toolBar->addWidget(creatAlbumBtn);
    toolBar->addWidget(addPhotoBtn);
    toolBar->addWidget(thumbSmallBtn);

    //中层分割器

    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal,this);
    this->setCentralWidget(mainSplitter);

    //左侧相册列表
    QListWidget *albumListWidget = new QListWidget(mainSplitter);
    albumListWidget->addItem("默认相册");
    albumListWidget->setMinimumWidth(120);
    albumListWidget->setMaximumWidth(240);
    //批量导入相册
    for(const auto& pair : this->album){
        albumListWidget->addItem(pair.first);
    }

    //右侧缩略图区
    QWidget *rightArea = new QWidget(mainSplitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightArea);
    rightArea->setLayout(rightLayout);

    QWidget *thumbArea = new QWidget(rightArea);
    QGridLayout *thumbLayout = new QGridLayout(thumbArea);
    thumbArea->setLayout(thumbLayout);
    rightLayout->addWidget(thumbArea);

    //分配默认布局
    QList<int> splitterSizes;
    splitterSizes<<120<<880;
    mainSplitter->setSizes(splitterSizes);


    //底层状态栏
    QStatusBar *statusBar = new QStatusBar(this);
    statusBar->showMessage("就绪");
    this->setStatusBar(statusBar);


    //信号
    connect(addPhotoBtn,&QPushButton::clicked,this,[=](){
        QStringList photoPaths = QFileDialog::getOpenFileNames(
                this,"选择照片",QCoreApplication::applicationDirPath(),"图片文件 (*.jpg *.png *.jepg)"
            );

        if(photoPaths.isEmpty())return;

        while(QLayoutItem *item = thumbLayout->takeAt(0)){
            if(QWidget *w = item->widget()) w->deleteLater();
            delete item;
        }

        g_photoPaths.clear();
        g_photoPaths = photoPaths;
        int row = 0,col = 0;

        //创建缩略图
        for(const QString& path:photoPaths){
            ClickableLabel *thumbLabel = new ClickableLabel(thumbArea);
            QPixmap pix(path);
            QPixmap scaledThumb = pix.scaled(
                80,80,Qt::KeepAspectRatio,Qt::SmoothTransformation);
            thumbLabel->setPixmap(scaledThumb);
            thumbLabel->setAlignment(Qt::AlignCenter);
            thumbLabel->setStyleSheet(" solid #ccc;border-radius:4px");
            thumbLabel->setCursor(Qt::PointingHandCursor);

            //大图显示
            connect(thumbLabel,&ClickableLabel::clicked,this,[=](){

                // qDebug() << "开始创建大图窗口，路径：" << path;

                QWidget *bigPhotoWindow = new QWidget();
                bigPhotoWindow->setWindowTitle(QFileInfo(path).fileName());
                bigPhotoWindow->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Window);
                bigPhotoWindow->setAttribute(Qt::WA_DeleteOnClose);
                bigPhotoWindow->setStyleSheet("background-color:#f5f5f5;");
                bigPhotoWindow->setMinimumSize(400, 300);
                bigPhotoWindow->resize(800,600);

                //屏幕信息
                QRect screenRect = QGuiApplication::primaryScreen()->geometry();
                int WinX = (screenRect.width() - bigPhotoWindow->width()) /2;
                int WinY = (screenRect.height() - bigPhotoWindow->height()) /2;
                bigPhotoWindow->move(WinX,WinY);
                //布局
                QVBoxLayout *vLayout = new QVBoxLayout(bigPhotoWindow);
                QHBoxLayout *hLayout = new QHBoxLayout(bigPhotoWindow);

                //显示图片
                QLabel *bigPhotoLabel = new QLabel(bigPhotoWindow);
                // bigPhotoLabel->setGeometry(0,0,bigPhotoWindow->width(),bigPhotoWindow->height());
                bigPhotoLabel->setAlignment(Qt::AlignCenter);
                hLayout->addWidget(bigPhotoLabel);
                hLayout->setAlignment(Qt::AlignCenter);
                vLayout->addLayout(hLayout);
                vLayout->setAlignment(Qt::AlignCenter);
                bigPhotoLabel->setScaledContents(false);

                QPixmap bigPix(path);
                QPixmap scaledBig = bigPix.scaled(
                bigPhotoWindow->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
                bigPhotoLabel->setPixmap(scaledBig);

                PhotoWindowEventFilter *filter = new PhotoWindowEventFilter(bigPix,bigPhotoLabel,screenRect,bigPhotoWindow);
                bigPhotoWindow->installEventFilter(filter);

                bigPhotoWindow->show();

            });

            //显示缩略图
            thumbLayout->addWidget(thumbLabel,row,col);
            col++;
            if(col >= 4){
                col = 0;
                row++;
            }
        }
    });

}


void MainWindow::init(){
    //查找album.json文件，这个文件是用来记录相册名的。
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
            QJsonArray photos = begin.value().toArray();
            std::pair<QString, std::vector<QString>> p{begin.key(), std::vector<QString>()};
            for(const auto& photo : photos){
                p.second.push_back(photo.toString());
            }
            this->album.push_back(p);
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
    for(const std::pair<QString, std::vector<QString>>& p : this->album){
        QJsonArray arr;
        const auto& photos = p.second;
        for(const QString& photo : photos){
            arr.push_back(photo);
        }
        tar[p.first] = arr;
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
