#include <QWidget>
#include <QPainter>
 
class MyWidget : public QWidget {
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setPen(Qt::blue); // 设置画笔颜色为蓝色
        painter.setBrush(Qt::blue); // 设置画刷颜色为蓝色
        painter.drawPoint(50, 50); // 在位置(50, 50)画一个点
    }
};
 
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
 
    MyWidget widget;
    widget.show();
 
    return app.exec();
}
