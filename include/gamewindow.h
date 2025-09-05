#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QStringList>
#include <QCloseEvent>
#include <QList>
#include "general.h"

class GameWindow : public QWidget {
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();
    void setIdentity(const QString &identity); // 设置身份显示
    void showAssignButton(bool show); // 控制分配按钮显示
    void setExpansions(const QStringList &expansions); // 设置使用的拓展包

    // 新增：显示武将选择对话框（主公专用）
    void showGeneralSelectionDialog(const QList<General> &generals);
signals:
    void assignIdentitiesRequested(); // 房主点击分配身份
    void gameWindowClosed(); // 游戏窗口关闭信号
    void generalSelected(const QString &generalName); // 主公选择武将后发出信号
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    QLabel *identityLabel;
    QLabel *expansionsLabel; // 显示当前选择的拓展包
    QPushButton *assignBtn;
    QLabel *generalLabel; // 显示分配到的武将名称
};


