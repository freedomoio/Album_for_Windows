#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QPixmap>
#include <QEvent>
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
#include <QJsonArray>
#include <QFileDialog>
#include <QFileInfo>
#include <vector>
#include <QtMinMax>
#include <QRect>
#include <QInputDialog>
#include <QSettings>
#include <QCoreApplication>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend void addalbum(const QString& name);

public:
    MainWindow(QWidget *parent = nullptr);
    //关闭窗口时负责保存相册的相关数据
    void close();
    //打开程序的时候从本地文件当中读取对应的数据
    void init();
    //检查各种文件夹是否存在
    void check_dir(const QString& dir);
    ~MainWindow();

    //返回相册在相册列表中的下标
    inline std::vector<QString>* findAlbum(const QString& album_name){
        for(auto& pair : this->album_list){
            if(pair.first == album_name) return &pair.second;
        }
        return nullptr;
    }
    inline const std::vector<QString>* findAlbum(const QString& album_name) const{
        for(const auto& pair : this->album_list){
            if(pair.first == album_name) return &pair.second;
        }
        return nullptr;
    }



private:
    Ui::MainWindow *ui;

    //==============================field

    QStringList g_photoPaths;
    //一个相册对应一个QString和其中的图片的名字，名字需要记录后缀名
    std::vector<std::pair<QString, std::vector<QString>>> album_list;
    //记录选择图片时上次选的路径
    QString last_path;
    //记录当前窗口运行时的目录
    const QString app_path;

    //工具栏
    QToolBar* tool_bar;
    //左侧的相册列表组件
    QListWidget* album_list_widget;
    //右侧的缩略图区域
    QWidget* thumb_nail_area;
    //下边的状态栏，但是不知道有什么用
    QStatusBar* status_bar;
    //当前选中的相册
    std::vector<QString> cur_album;
    //缩略图区的方格层
    QGridLayout* thumb_layout;

    //===============================function

    static void add_album(MainWindow&);
    void create_detail();
};
#endif // MAINWINDOW_H


















