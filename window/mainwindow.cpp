#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QDebug>
#include "event/photowindoweventfilter.h"
#include "widget/clickablelabel.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , app_path(QCoreApplication::applicationDirPath())
{
    ui->setupUi(this);
    init();

    //初始化尺寸
    this ->resize(1000,600);

    //工具栏
    this->tool_bar = new QToolBar("功能栏",this);
    this->addToolBar(tool_bar);

    QPushButton *creatAlbumBtn = new QPushButton("创建相册",this);
    QPushButton *addPhotoBtn = new QPushButton("添加照片",this);
    QPushButton *thumbSmallBtn = new QPushButton("小缩略图",this);
    tool_bar->addWidget(creatAlbumBtn);
    tool_bar->addWidget(addPhotoBtn);
    tool_bar->addWidget(thumbSmallBtn);

    //中层分割器

    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal,this);
    this->setCentralWidget(mainSplitter);

    //左侧相册列表
    album_list_widget = new QListWidget(mainSplitter);
    album_list_widget->setMinimumWidth(120);
    album_list_widget->setMaximumWidth(240);

    //批量导入相册
    for(const auto& pair : this->album_list){
        this->album_list_widget->addItem(pair.first);
    }
    //否则生产默认相册
    if(album_list_widget->count() == 0){
        album_list_widget->addItem("默认相册");
        this->album_list.emplace_back("默认相册", std::vector<QString>());
    }

    //右侧缩略图区
    this->thumb_nail_area = new QWidget(mainSplitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(thumb_nail_area);
    thumb_nail_area->setLayout(rightLayout);

    QWidget *thumbArea = new QWidget(thumb_nail_area);
    thumb_layout = new QGridLayout(thumbArea);
    thumbArea->setLayout(thumb_layout);
    rightLayout->addWidget(thumbArea);

    //分配默认布局
    QList<int> splitterSizes;
    splitterSizes<<120<<880;
    mainSplitter->setSizes(splitterSizes);


    //底层状态栏
    status_bar = new QStatusBar(this);
    status_bar->showMessage("就绪");
    this->setStatusBar(status_bar);

    //const QString** path = new const QString*;
    //大图显示的lambda表达式
    auto pf = [](int idx, const std::vector<QString>* album){

        // qDebug() << "开始创建大图窗口，路径：" << path;

        QWidget *bigPhotoWindow = new QWidget();
        //bigPhotoWindow->setWindowTitle(QFileInfo(.fileName());
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
        QHBoxLayout *hLayout = new QHBoxLayout;

        //显示图片
        QLabel *bigPhotoLabel = new QLabel(bigPhotoWindow);
        //bigPhotoLabel->setGeometry(0,0,bigPhotoWindow->width(),bigPhotoWindow->height());
        bigPhotoLabel->setAlignment(Qt::AlignCenter);
        hLayout->addWidget(bigPhotoLabel);
        hLayout->setAlignment(Qt::AlignCenter);
        vLayout->addLayout(hLayout);
        vLayout->setAlignment(Qt::AlignCenter);
        bigPhotoLabel->setScaledContents(false);

        QPixmap bigPix((*album)[idx]);
        QPixmap scaledBig = bigPix.scaled(
            bigPhotoWindow->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        bigPhotoLabel->setPixmap(scaledBig);

        PhotoWindowEventFilter *filter = new PhotoWindowEventFilter(bigPix,bigPhotoLabel,screenRect,bigPhotoWindow);
        bigPhotoWindow->installEventFilter(filter);

        bigPhotoWindow->show();

    };

    //选择相册时触发
    connect(album_list_widget,&QListWidget::itemClicked,this,[=]{
        const std::vector<QString>* album = findAlbum(album_list_widget->currentItem()->text());
        if(album == nullptr){
            qDebug() << "本不应该这样的，但选择相册的时候出现了问题，请检查序列化和反序列化";
            return;
        }
        while(QLayoutItem *item = thumb_layout->takeAt(0)){
            if(QWidget *w = item->widget()) w->deleteLater();
            delete item;
        }
        int row = 0,col = 0;
        int len = album->size();
        for(int i = 0; i < len; i++){
            ClickableLabel *thumb_label = new ClickableLabel(thumbArea, album);
            //为label添加下标
            thumb_label->setIdx(i);

            QPixmap pix((*album)[i]);
            QPixmap scaledThumb = pix.scaled(
                80,80,Qt::KeepAspectRatio,Qt::SmoothTransformation);
            thumb_label->setPixmap(scaledThumb);
            thumb_label->setAlignment(Qt::AlignCenter);
            thumb_label->setStyleSheet(" solid #ccc;border-radius:4px");
            thumb_label->setCursor(Qt::PointingHandCursor);

            //大图显示
            connect(thumb_label,&ClickableLabel::clicked,this,pf);

            //显示缩略图
            thumb_layout->addWidget(thumb_label,row,col);
            col++;
            if(col >= 4){
                col = 0;
                row++;
            }
        }
    });

    //添加相册
    connect(creatAlbumBtn,&QPushButton::clicked,this,[this](){
        add_album(*this);
    });

    //添加照片
    connect(addPhotoBtn,&QPushButton::clicked,this,[=](){
        if(this->album_list_widget->currentItem() == nullptr){
            QMessageBox::information(nullptr,"信息","请选中相册");
                return;
            }
        QStringList photoPaths = QFileDialog::getOpenFileNames(
                this,"选择照片",last_path,"图片文件 (*.jpg *.png *.jepg)"
            );
        if(photoPaths.isEmpty()){
            return;
        }

        QFileInfo selectedFile(photoPaths.first());
        QString lastPath = selectedFile.absolutePath();
        QSettings settings(app_path + "/data/config.ini",QSettings::IniFormat);
        settings.setValue("LastPhotoPath",lastPath);

        while(QLayoutItem *item = thumb_layout->takeAt(0)){
            if(QWidget *w = item->widget()) w->deleteLater();
            delete item;
        }

        g_photoPaths.clear();
        g_photoPaths = photoPaths;
        int row = 0,col = 0;

        std::vector<QString>* album = findAlbum(album_list_widget->currentItem()->text());
        if(album == nullptr){
            qDebug() << "你说这扯不扯，原本不应该是空指针的，请检查序列化和反序列化";
        }

        //创建缩略图 + 文件拷贝
        for(int i = 0; i < photoPaths.size(); i++){

            QFile photoFile(photoPaths[i]);
            if(!photoFile.open(QIODevice::ReadOnly)){
                qDebug() << photoFile.errorString();
            }

            QByteArray data = photoFile.readAll();
            photoFile.close();

            QString photoDirPath = app_path + "/photo";
            QDir photoDir(photoDirPath);
            if(!photoDir.exists()){
                photoDir.mkdir(".");
            }

            QFileInfo fileInfo(photoPaths[i]);
            QString fileName = fileInfo.fileName();

            QString targetPath = photoDirPath +"/" + fileName;
            QFile WriteFile(targetPath);
            bool suc = true;
            if(!WriteFile.exists()){
                if(!WriteFile.open(QIODevice::NewOnly)){
                    qDebug() << "文件创建失败，" << WriteFile.errorString();
                    suc = false;
                }
            }
            if(!suc && !WriteFile.open(QIODevice::WriteOnly)){
                qDebug() << "文件打开失败，" << WriteFile.errorString();
                suc = false;
            }
            if(!suc){
                qDebug() << "有问题哦，251行";
                return;
            }
            if(suc){
                WriteFile.write(data);
                WriteFile.close();
            }

            album->push_back(photoPaths[i]);
            ClickableLabel *thumbLabel = new ClickableLabel(thumbArea, album);
            QPixmap pix(photoPaths[i]);
            QPixmap scaledThumb = pix.scaled(
                80,80,Qt::KeepAspectRatio,Qt::SmoothTransformation);
            thumbLabel->setPixmap(scaledThumb);
            thumbLabel->setAlignment(Qt::AlignCenter);
            thumbLabel->setStyleSheet(" solid #ccc;border-radius:4px");
            thumbLabel->setCursor(Qt::PointingHandCursor);

            //大图显示
            connect(thumbLabel,&ClickableLabel::clicked,this,pf);

            //显示缩略图
            thumb_layout->addWidget(thumbLabel,row,col);
            col++;
            if(col >= 4){
                col = 0;
                row++;
            }
        }
    });

}

void MainWindow::add_album(MainWindow& w){
    bool isOk;
    QString albumName = QInputDialog::getText(
        nullptr,"创建新相册","请输入相册名",QLineEdit::Normal,"",&isOk
        );
    if(!isOk){
        return;
    }
    albumName = albumName.trimmed();

    for(const auto& Album : w.album_list){
        if(Album.first == albumName){
            QMessageBox::information(nullptr,"提示","相册已存在");
            return;
        }
    }
    if(albumName.isEmpty()){
        QMessageBox::warning(nullptr,"警告","相册名不能为空");
        return;
    }
    w.album_list_widget->addItem(albumName);
    w.album_list.push_back(std::make_pair(albumName,std::vector<QString>()));
}

void MainWindow::init(){
    QSettings settings(app_path + "/data/config.ini",QSettings::IniFormat);
    last_path = settings.value("LastPhotoPath",app_path).toString();
    //查找album.json文件，这个文件是用来记录相册名的。
    QString path(app_path);
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
            this->album_list.push_back(p);
            ++begin;
        }
    }
}

void MainWindow::close(){
    QJsonObject root;
    QString path(app_path);
    QFile file(path+"/data/album.json");//不要试图使用QDir的相关方法，他们不会修改对象本身

    QJsonObject tar;
    /*
     * tar[""],""中的值不同，堆放的是同级的json数据
     */
    for(const std::pair<QString, std::vector<QString>>& p : this->album_list){
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
