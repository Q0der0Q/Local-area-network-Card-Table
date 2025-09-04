#include "gamewindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("游戏牌桌");
    resize(400, 300);
    QVBoxLayout *layout = new QVBoxLayout(this);
    identityLabel = new QLabel("身份未分配", this);
    layout->addWidget(identityLabel);

    // 添加拓展包显示标签
    expansionsLabel = new QLabel("使用拓展包：", this);
    layout->addWidget(expansionsLabel);

    assignBtn = new QPushButton("分配身份", this);
    assignBtn->setVisible(false);
    layout->addWidget(assignBtn);
    connect(assignBtn, &QPushButton::clicked, this, [this]() {
        emit assignIdentitiesRequested();
    });
    setLayout(layout);
}

GameWindow::~GameWindow() {}

void GameWindow::setIdentity(const QString &identity) {
    identityLabel->setText("你的身份: " + identity);
}

void GameWindow::showAssignButton(bool show) {
    assignBtn->setVisible(show);
}

// 实现设置拓展包的方法
void GameWindow::setExpansions(const QStringList &expansions) {
    if (expansions.isEmpty()) {
        expansionsLabel->setText("使用拓展包：无");
    } else {
        expansionsLabel->setText("使用拓展包：" + expansions.join("、"));
    }
}

// 实现关闭事件处理函数
void GameWindow::closeEvent(QCloseEvent *event) {
    emit gameWindowClosed();
    event->accept();
}

