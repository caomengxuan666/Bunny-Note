#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPixmap>
#include <QTimer>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 创建主窗口的实例
    MainWindow window;

    // 创建一个窗口用于加载
    QWidget splashScreen;
    splashScreen.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    QVBoxLayout *layout = new QVBoxLayout(&splashScreen);

    // 加载 ICO 图片
    QLabel *logoLabel = new QLabel;
    QPixmap logoPixmap(":/wyw.ico");
    logoLabel->setPixmap(logoPixmap.scaledToWidth(100)); // 调整大小以适应布局
    logoLabel->setAlignment(Qt::AlignCenter); // 居中显示

    QLabel *loadingLabel = new QLabel("Loading...\nWelcome To Bunny Note!");
    QProgressBar *progressBar = new QProgressBar();
    progressBar->setRange(0, 100);

    layout->addWidget(logoLabel); // 添加 ICO 图片标签
    layout->addWidget(loadingLabel);
    layout->addWidget(progressBar);
    splashScreen.setLayout(layout);

    splashScreen.resize(300, 150); // 调整窗口大小以容纳图片
    splashScreen.show();

    // 创建一个定时器用于更新进度条
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        int value = progressBar->value() + 33; // 每次增加 33
        if (value >= 100) {
            value = 100;
            timer.stop(); // 停止计时器
            splashScreen.close(); // 关闭加载窗口

            // 显示主窗口
            window.show();
        }
        progressBar->setValue(value);
    });

    // 启动定时器，每秒更新一次
    timer.start(1000);

    return app.exec();
}
