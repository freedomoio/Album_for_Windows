# 第8次提交日志

提交人：sergio

更改：

* 重构了代码片段，新建三个文件夹`window`、`widget`、`event`，分别放入mainwindow类、ClickableLabel类、PhotoWindowEventFilter类。将后两者类从mainwindow.h从提取出来，单独放入文件。
* 为ClickableLabel类写入字段idx(`int`)和photo(`const std::vector<QString>*`)保存当前相册中的下标和所在相册。
* 为MainWindow添加check函数，在必要时检查需要的文件夹是否存在。
