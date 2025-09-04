#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QStringList>
#include <QCloseEvent>

class GameWindow : public QWidget {
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();
    void setIdentity(const QString &identity); // 设置身份显示
    void showAssignButton(bool show); // 控制分配按钮显示
    void setExpansions(const QStringList &expansions); // 设置使用的拓展包
signals:
    void assignIdentitiesRequested(); // 房主点击分配身份
    void gameWindowClosed(); // 游戏窗口关闭信号
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    QLabel *identityLabel;
    QLabel *expansionsLabel; // 显示当前选择的拓展包
    QPushButton *assignBtn;
};

