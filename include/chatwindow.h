#pragma once
#include "gamewindow.h"
#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include <QTimer>
#include <QMap>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>

class ChatWindow : public QWidget {
    Q_OBJECT
public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();

private slots:
    void onModeChanged(int index); // 服务端/客户端切换
    void onOpenServer();           // 服务端开放
    void onSearchServers();        // 客户端搜索
    void onJoinServer();           // 客户端加入（按钮点击）
    void onJoinServer(QListWidgetItem *item); // 客户端加入（双击列表项）
    void onSendMessage();          // 发送消息
    void onNewConnection();        // 服务端新连接
    void onReadyRead();            // 收到消息
    void onUdpReadyRead();         // UDP发现服务器
    void onReadyClicked();         // 客户端点击准备
    void onStartGameClicked();     // 服务端点击开始游戏
    void onClientReady(QTcpSocket *client, bool ready); // 服务端处理客户端准备
    void onGameStart();            // 客户端收到开始游戏
    void onAssignIdentities();     // 房主分配身份按钮槽
    void onGameWindowClosed();     // 处理游戏窗口关闭
    void onLeaveRoom();            // 退出房间

private:
    // UI控件
    QComboBox *modeComboBox;       // 服务端/客户端切换
    QLabel *portLabel;
    QLineEdit *portEdit;
    QPushButton *openServerBtn;
    QPushButton *searchBtn;
    QListWidget *serverList;
    QPushButton *joinBtn;
    QTextEdit *chatDisplay;
    QLineEdit *messageEdit;
    QPushButton *sendBtn;
    QLabel *roomLabel;              // 房间名标签
    QLineEdit *roomEdit;           // 房间名输入框
    QLabel *nickLabel;              // 昵称标签
    QLineEdit *nickEdit;           // 昵称输入框
    QLabel *pwdLabel;              // 房间密码标签
    QLineEdit *pwdEdit;           // 房间密码输入框
    QPushButton *readyBtn;         // 客户端准备按钮
    QPushButton *startGameBtn;     // 服务端开始游戏按钮
    QPushButton *leaveRoomBtn;     // 退出房间按钮
    QSpinBox *zhongchenSpin;
    QSpinBox *fanzaiSpin;
    QSpinBox *neijianSpin;
    QLabel *identityLabel;

    // 拓展包选择
    QGroupBox *expansionGroupBox;
    QCheckBox *standardPackCheckBox;   // 标准包
    QCheckBox *windPackCheckBox;       // 风包
    QCheckBox *firePackCheckBox;       // 火包

    // 网络相关
    QTcpServer *tcpServer;
    QList<QTcpSocket*> clientSockets; // 服务端管理的客户端
    QTcpSocket *tcpSocket;            // 客户端socket
    QUdpSocket *udpSocket;            // UDP用于发现服务器
    quint16 serverPort;
    QString serverAddress;
    QTimer *broadcastTimer;           // 定时广播服务器信息
    QString roomName;                 // 房间名，由服务端指定
    QString nickname;                 // 当前用户昵称
    QString roomPassword;             // 房间密码
    QMap<QTcpSocket*, bool> clientReadyMap; // 服务端：每个客户端的准备状态
    bool isReady = false;             // 客户端：自身准备状态
    GameWindow *gameWindow = nullptr; // 牌桌窗口指针

    void setupUi();
    void setupConnections();
    void broadcastServer();           // 服务端UDP广播
    QStringList getSelectedExpansions(); // 获取选中的拓展包
};
