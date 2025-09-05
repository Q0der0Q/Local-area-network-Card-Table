#include "gamewindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QGridLayout>
#include <QScrollArea>
#include <QGroupBox>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("游戏牌桌");
    resize(400, 300);
    QVBoxLayout *layout = new QVBoxLayout(this);
    identityLabel = new QLabel("身份未分配", this);
    layout->addWidget(identityLabel);

    // 添加武将显示标签
    generalLabel = new QLabel("武将未分配", this);
    layout->addWidget(generalLabel);

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

// 实现武将选择对话框
void GameWindow::showGeneralSelectionDialog(const QList<General> &generals) {
    // 创建对话框
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("选择武将");
    dialog->setMinimumSize(650, 450);

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);

    QLabel *titleLabel = new QLabel("请从以下八位武将中选择一位：", dialog);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // 创建滚动区域
    QScrollArea *scrollArea = new QScrollArea(dialog);
    scrollArea->setWidgetResizable(true);
    QWidget *scrollContent = new QWidget();
    QGridLayout *gridLayout = new QGridLayout(scrollContent);

    // 在网格布局中添加武将选项
    const int columns = 2;
    for (int i = 0; i < generals.size(); ++i) {
        const General &general = generals[i];

        QGroupBox *generalBox = new QGroupBox(scrollContent);
        QVBoxLayout *boxLayout = new QVBoxLayout(generalBox);

        QLabel *nameLabel = new QLabel(general.name + " - " + general.kingdom, generalBox);
        QFont nameFont = nameLabel->font();
        nameFont.setBold(true);
        nameLabel->setFont(nameFont);
        boxLayout->addWidget(nameLabel);

        QLabel *hpLabel = new QLabel("体力: " + QString::number(general.hp) + "/" + QString::number(general.maxHp), generalBox);
        boxLayout->addWidget(hpLabel);

        QLabel *skillsLabel = new QLabel(generalBox);
        QString skillText = "技能：\n";
        for (const Skill &skill : general.skills) {
            skillText += "【" + skill.name + "】: " + skill.description + "\n";
        }
        skillsLabel->setText(skillText);
        skillsLabel->setWordWrap(true);
        boxLayout->addWidget(skillsLabel);

        QPushButton *selectBtn = new QPushButton("选择", generalBox);
        boxLayout->addWidget(selectBtn);

        // 连接选择按钮的点击信号
        connect(selectBtn, &QPushButton::clicked, this, [this, dialog, general]() {
            emit generalSelected(general.name);
            generalLabel->setText("你的武将: " + general.name);
            dialog->accept();
        });

        gridLayout->addWidget(generalBox, i / columns, i % columns);
    }

    scrollContent->setLayout(gridLayout);
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    dialog->setLayout(mainLayout);
    dialog->setModal(true);
    dialog->show();
}
