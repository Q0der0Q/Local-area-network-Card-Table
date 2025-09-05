#include "chatwindow.h"
#include "gamewindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QInputDialog>
#include <random>
#include <ctime>

ChatWindow::ChatWindow(QWidget *parent)
    : QWidget(parent), tcpServer(nullptr), tcpSocket(nullptr), udpSocket(nullptr), serverPort(0), broadcastTimer(nullptr), isReady(false)
{
    setupUi();
    setupConnections();
}

ChatWindow::~ChatWindow() {
    if (tcpServer) delete tcpServer;
    if (tcpSocket) delete tcpSocket;
    if (udpSocket) delete udpSocket;
    if (broadcastTimer) delete broadcastTimer;
}

void ChatWindow::setupUi() {
    modeComboBox = new QComboBox(this);
    modeComboBox->addItem("服务端");
    modeComboBox->addItem("客户端");
    portLabel = new QLabel("端口:", this);
    portEdit = new QLineEdit(this);
    portEdit->setText("12345");
    roomLabel = new QLabel("房间名:", this);
    roomEdit = new QLineEdit(this);
    roomEdit->setText("默认房间");
    nickLabel = new QLabel("昵称:", this);
    nickEdit = new QLineEdit(this);
    nickEdit->setText("用户");
    pwdLabel = new QLabel("房间密码:", this);
    pwdEdit = new QLineEdit(this);
    pwdEdit->setEchoMode(QLineEdit::Password);
    pwdEdit->setText("");
    openServerBtn = new QPushButton("开放服务器", this);
    searchBtn = new QPushButton("搜索服务器", this);
    serverList = new QListWidget(this);
    joinBtn = new QPushButton("加入房间", this);
    chatDisplay = new QTextEdit(this);
    chatDisplay->setReadOnly(true);
    messageEdit = new QLineEdit(this);
    sendBtn = new QPushButton("发送", this);
    readyBtn = new QPushButton("准备", this);
    startGameBtn = new QPushButton("开始游戏", this);
    leaveRoomBtn = new QPushButton("退出房间", this);
    readyBtn->setVisible(false);
    startGameBtn->setVisible(false);
    leaveRoomBtn->setVisible(false);

    identityLabel = new QLabel("设置身份数量：", this);
    zhongchenSpin = new QSpinBox(this);
    zhongchenSpin->setRange(0, 10);
    zhongchenSpin->setValue(1);
    zhongchenSpin->setPrefix("忠臣:");
    fanzaiSpin = new QSpinBox(this);
    fanzaiSpin->setRange(0, 10);
    fanzaiSpin->setValue(2);
    fanzaiSpin->setPrefix("反贼:");
    neijianSpin = new QSpinBox(this);
    neijianSpin->setRange(0, 10);
    neijianSpin->setValue(1);
    neijianSpin->setPrefix("内奸:");
    identityLabel->setVisible(false);
    zhongchenSpin->setVisible(false);
    fanzaiSpin->setVisible(false);
    neijianSpin->setVisible(false);

    // 创建拓展包选择框
    expansionGroupBox = new QGroupBox("拓展包选择", this);
    QVBoxLayout *expansionLayout = new QVBoxLayout();
    standardPackCheckBox = new QCheckBox("标准包", this);
    windPackCheckBox = new QCheckBox("风包", this);
    firePackCheckBox = new QCheckBox("火包", this);
    standardPackCheckBox->setChecked(true); // 默认勾选标准包
    expansionLayout->addWidget(standardPackCheckBox);
    expansionLayout->addWidget(windPackCheckBox);
    expansionLayout->addWidget(firePackCheckBox);
    expansionGroupBox->setLayout(expansionLayout);
    expansionGroupBox->setVisible(false);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(modeComboBox);
    topLayout->addWidget(portLabel);
    topLayout->addWidget(portEdit);
    topLayout->addWidget(roomLabel);
    topLayout->addWidget(roomEdit);
    topLayout->addWidget(nickLabel);
    topLayout->addWidget(nickEdit);
    topLayout->addWidget(pwdLabel);
    topLayout->addWidget(pwdEdit);
    topLayout->addWidget(openServerBtn);
    topLayout->addWidget(searchBtn);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(serverList);
    mainLayout->addWidget(joinBtn);
    mainLayout->addWidget(chatDisplay);
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(messageEdit);
    bottomLayout->addWidget(sendBtn);
    mainLayout->addLayout(bottomLayout);
    mainLayout->addWidget(readyBtn);
    mainLayout->addWidget(startGameBtn);
    mainLayout->addWidget(leaveRoomBtn);

    mainLayout->addWidget(identityLabel);
    mainLayout->addWidget(zhongchenSpin);
    mainLayout->addWidget(fanzaiSpin);
    mainLayout->addWidget(neijianSpin);
    mainLayout->addWidget(expansionGroupBox);

    setLayout(mainLayout);
    serverList->setVisible(false);
    joinBtn->setVisible(false);
    searchBtn->setVisible(false);
    roomLabel->setVisible(true);
    roomEdit->setVisible(true);
    nickLabel->setVisible(true);
    nickEdit->setVisible(true);
    pwdLabel->setVisible(true);
    pwdEdit->setVisible(true);
}

void ChatWindow::setupConnections() {
    connect(modeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onModeChanged(int)));
    connect(openServerBtn, SIGNAL(clicked()), this, SLOT(onOpenServer()));
    connect(searchBtn, SIGNAL(clicked()), this, SLOT(onSearchServers()));
    connect(joinBtn, SIGNAL(clicked()), this, SLOT(onJoinServer()));
    connect(serverList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onJoinServer(QListWidgetItem*)));
    connect(sendBtn, SIGNAL(clicked()), this, SLOT(onSendMessage()));
    connect(readyBtn, &QPushButton::clicked, this, &ChatWindow::onReadyClicked);
    connect(startGameBtn, &QPushButton::clicked, this, &ChatWindow::onStartGameClicked);
    connect(leaveRoomBtn, &QPushButton::clicked, this, &ChatWindow::onLeaveRoom);
}

void ChatWindow::onModeChanged(int index) {
    bool isServer = (index == 0);
    openServerBtn->setVisible(isServer);
    searchBtn->setVisible(!isServer);
    serverList->setVisible(!isServer);
    joinBtn->setVisible(!isServer);
    chatDisplay->clear();
    if (tcpServer) { delete tcpServer; tcpServer = nullptr; }
    if (tcpSocket) { delete tcpSocket; tcpSocket = nullptr; }
    if (udpSocket) { delete udpSocket; udpSocket = nullptr; }
    if (broadcastTimer) { broadcastTimer->stop(); delete broadcastTimer; broadcastTimer = nullptr; }
    portLabel->setVisible(isServer);
    portEdit->setVisible(isServer);
    roomLabel->setVisible(isServer);
    roomEdit->setVisible(isServer);
    pwdLabel->setVisible(isServer);
    pwdEdit->setVisible(isServer);
    identityLabel->setVisible(isServer);
    zhongchenSpin->setVisible(isServer);
    fanzaiSpin->setVisible(isServer);
    neijianSpin->setVisible(isServer);
    expansionGroupBox->setVisible(isServer); // 只有服务端可以选择拓展包
}

void ChatWindow::onOpenServer() {
    serverPort = portEdit->text().toUShort();
    roomName = roomEdit->text();
    roomPassword = pwdEdit->text();
    if (roomName.isEmpty()) {
        QMessageBox::warning(this, "提示", "房间名不能为空！");
        return;
    }
    if (roomPassword.isEmpty()) {
        QMessageBox::warning(this, "提示", "房间密码不能为空！");
        return;
    }
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, serverPort)) {
        QMessageBox::critical(this, "错误", "服务器启动失败");
        return;
    }
    connect(tcpServer, &QTcpServer::newConnection, this, &ChatWindow::onNewConnection);
    chatDisplay->append("服务器已开放，房间名: " + roomName + " 端口:" + QString::number(serverPort));
    // 启动UDP广播
    udpSocket = new QUdpSocket(this);
    broadcastServer();
    // 定时广播
    if (!broadcastTimer) {
        broadcastTimer = new QTimer(this);
        connect(broadcastTimer, &QTimer::timeout, this, &ChatWindow::broadcastServer);
    }
    broadcastTimer->start(1000); // 每秒广播一次

    // 显示游戏设置控件
    identityLabel->setVisible(true);
    zhongchenSpin->setVisible(true);
    fanzaiSpin->setVisible(true);
    neijianSpin->setVisible(true);
    expansionGroupBox->setVisible(true); // 显示拓展包选择框

    // 显示退出房间按钮
    leaveRoomBtn->setVisible(true);

    chatDisplay->append("请设置身份数量和选择需要的拓展包后点击\"开始游戏\"");
}

void ChatWindow::broadcastServer() {
    QByteArray datagram = (roomName + ":" + QString::number(serverPort)).toUtf8();
    // 遍历所有网卡，向每个网卡的广播地址发送UDP包
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        if (iface.flags() & QNetworkInterface::IsUp && iface.flags() & QNetworkInterface::IsRunning && !(iface.flags() & QNetworkInterface::IsLoopBack)) {
            for (const QNetworkAddressEntry &entry : iface.addressEntries()) {
                QHostAddress broadcastAddr = entry.broadcast();
                if (!broadcastAddr.isNull()) {
                    udpSocket->writeDatagram(datagram, broadcastAddr, 45454);
                }
            }
        }
    }
    // 兼容原有方式
    udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 45454);
}

void ChatWindow::onNewConnection() {
    while (tcpServer->hasPendingConnections()) {
        QTcpSocket *client = tcpServer->nextPendingConnection();
        clientSockets.append(client);
        connect(client, &QTcpSocket::readyRead, this, &ChatWindow::onReadyRead);
        connect(client, &QTcpSocket::disconnected, this, [this, client]() {
            clientSockets.removeAll(client);
            chatDisplay->append("客户端已断开: " + client->peerAddress().toString());
            client->deleteLater();
        });
        chatDisplay->append("新客户端已连接: " + client->peerAddress().toString());
    }
    int totalPlayers = clientSockets.size() + 1;
    int maxZhongchen = totalPlayers - 1;
    int maxFanzai = totalPlayers - 1;
    int maxNeijian = totalPlayers - 1;
    zhongchenSpin->setMaximum(maxZhongchen);
    fanzaiSpin->setMaximum(maxFanzai);
    neijianSpin->setMaximum(maxNeijian);
}

void ChatWindow::onReadyRead() {
    QTcpSocket *senderSocket = qobject_cast<QTcpSocket*>(sender());
    if (!senderSocket) return;
    QByteArray data = senderSocket->readAll();
    QString msg = QString::fromUtf8(data);
    // 服务端校验密码
    if (tcpServer) {
        if (msg.startsWith("__PWD__:")) {
            QString pwd = msg.mid(QString("__PWD__:").length());
            if (pwd != roomPassword) {
                senderSocket->write(QString("密码错误，已断开连接！").toUtf8());
                senderSocket->disconnectFromHost();
                chatDisplay->append("有客户端密码错误被拒绝连接");
                return;
            } else {
                senderSocket->write(QString("密码正确，欢迎加入房间！").toUtf8());
                chatDisplay->append("有客户端通过密码验证加入房间");
                clientReadyMap[senderSocket] = false;
                return;
            }
        }
        if (msg == "__READY__") {
            onClientReady(senderSocket, true);
            return;
        }
        // 处理客户端关闭游戏窗口的消息
        if (msg == "__CLOSE_GAME_WINDOW__") {
            chatDisplay->append("有客户端关闭了游戏窗口");
            // 如果服务端有游戏窗口，也关闭它
            if (gameWindow) {
                chatDisplay->append("服务器端也关闭游戏窗口");
                gameWindow->close();
            }
            return;
        }
    }
    if (tcpSocket) {
        if (msg == "密码正确，欢迎加入房间！") {
            chatDisplay->append("已通过密码验证，准备加入游戏");
            readyBtn->setVisible(true);
            sendBtn->setEnabled(true);
            leaveRoomBtn->setVisible(true); // 显示退出房间按钮
            return;
        }
        if (msg.startsWith("__START_GAME__:")) {
            QString expansionsStr = msg.mid(QString("__START_GAME__:").length());
            QStringList expansions = expansionsStr.split(",");
            chatDisplay->append("游戏开始，使用拓展包: " + expansions.join("、"));

            // 创建游戏窗口并设置拓展包信息
            if (!gameWindow) {
                gameWindow = new GameWindow();
                // 连接窗口关闭信号，以便正确清除指针
                connect(gameWindow, &QObject::destroyed, this, [this]() {
                    gameWindow = nullptr;
                });
                // 连接窗口关闭信号，进行客户端本地处理
                connect(gameWindow, &GameWindow::gameWindowClosed, this, &ChatWindow::onGameWindowClosed);
            }
            gameWindow->setAttribute(Qt::WA_DeleteOnClose);
            gameWindow->show();
            gameWindow->showAssignButton(false);
            gameWindow->setExpansions(expansions);

            return;
        }
        if (msg == "__GAME_ENDED__") {
            // 收到游戏结束消息，重置准备状态
            chatDisplay->append("游戏已结束，请重新准备...");
            isReady = false;
            readyBtn->setEnabled(true);
            readyBtn->setVisible(true);
            // 如果游戏窗口还在，关闭它
            if (gameWindow) {
                gameWindow->close();
                gameWindow = nullptr;
            }
            return;
        }
    }
    if (msg == "__START_GAME__") {
        // 服务端收到自己发的__START_GAME__，不显示
        onGameStart();
        return;
    }
    if (msg.startsWith("__IDENTITY__:")) {
        QString identity = msg.mid(QString("__IDENTITY__:").length());
        if (!gameWindow) {
            gameWindow = new GameWindow();
            // 连接窗口关闭信号，以便正确清除指针
            connect(gameWindow, &QObject::destroyed, this, [this]() {
                gameWindow = nullptr;
            });
            // 连接窗口关闭信号，进行客户端本地处理
            connect(gameWindow, &GameWindow::gameWindowClosed, this, &ChatWindow::onGameWindowClosed);
            gameWindow->setAttribute(Qt::WA_DeleteOnClose);
            gameWindow->show();
        }
        gameWindow->setIdentity(identity);
        gameWindow->showAssignButton(false);
        return;
    }
    chatDisplay->append(msg);
    // 服务端转发消息给所有客户端
    if (tcpServer) {
        for (QTcpSocket *client : clientSockets) {
            if (client != senderSocket)
                client->write(data);
        }
    }
}

void ChatWindow::onSendMessage() {
    nickname = nickEdit->text();
    QString msg = messageEdit->text();
    if (msg.isEmpty() || nickname.isEmpty()) return;
    QString fullMsg = nickname + ": " + msg;
    chatDisplay->append("我: " + msg);
    if (tcpServer) {
        for (QTcpSocket *client : clientSockets) {
            client->write(fullMsg.toUtf8());
        }
    } else if (tcpSocket) {
        tcpSocket->write(fullMsg.toUtf8());
    }
    messageEdit->clear();
}

void ChatWindow::onSearchServers() {
    serverList->clear();
    udpSocket = new QUdpSocket(this);
    // 绑定到所有网卡，增强跨设备兼容性
    udpSocket->bind(QHostAddress::Any, 45454, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpSocket, &QUdpSocket::readyRead, this, &ChatWindow::onUdpReadyRead);
    chatDisplay->append("正在搜索局域网服务器...");
}

void ChatWindow::onUdpReadyRead() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender);
        QString info = QString::fromUtf8(datagram); // 只显示房间名:端口
        // 保存实际IP到item的data，方便后续连接
        QListWidgetItem *item = new QListWidgetItem(info);
        item->setData(Qt::UserRole, sender.toString());
        if (serverList->findItems(info, Qt::MatchExactly).isEmpty())
            serverList->addItem(item);
    }
}

void ChatWindow::onJoinServer() {
    QListWidgetItem *item = serverList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "提示", "请先选中要加入的服务器房间！");
        return;
    }
    onJoinServer(item);
}

void ChatWindow::onJoinServer(QListWidgetItem *item) {
    if (!item) {
        QMessageBox::warning(this, "提示", "请先选中要加入的服务器房间！");
        return;
    }
    QString text = item->text();
    int idx = text.lastIndexOf(":");
    if (idx == -1) {
        chatDisplay->append("服务器地址格式错误: " + text);
        return;
    }
    QString ip = item->data(Qt::UserRole).toString();
    serverAddress = ip;
    serverPort = text.mid(idx + 1).toUShort();
    // 弹出密码输入框
    QString inputPwd = QInputDialog::getText(this, "房间密码", "请输入房间密码:", QLineEdit::Password);
    if (inputPwd.isEmpty()) {
        QMessageBox::warning(this, "提示", "密码不能为空！");
        return;
    }
    roomPassword = inputPwd;
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, [this, item]() {
        // 连接成功后立即发送密码
        tcpSocket->write(("__PWD__:" + roomPassword).toUtf8());
        chatDisplay->append("已连接服务器，正在验证密码...");
        sendBtn->setEnabled(false);
        readyBtn->setVisible(false);
        isReady = false;
        readyBtn->setEnabled(true);
    });
    connect(tcpSocket, &QTcpSocket::errorOccurred, [this](QAbstractSocket::SocketError err) {
        chatDisplay->append("连接服务器失败: " + tcpSocket->errorString());
        sendBtn->setEnabled(false);
    });
    connect(tcpSocket, &QTcpSocket::disconnected, [this]() {
        chatDisplay->append("与服务器连接已断开");
        sendBtn->setEnabled(false);
    });
    connect(tcpSocket, &QTcpSocket::readyRead, this, &ChatWindow::onReadyRead);
    sendBtn->setEnabled(false); // 未连接前禁用发送
    chatDisplay->append("正在连接服务器: " + item->text());
    tcpSocket->connectToHost(serverAddress, serverPort);
}

void ChatWindow::onReadyClicked() {
    if (isReady) return;
    isReady = true;
    readyBtn->setEnabled(false);
    if (tcpSocket) {
        tcpSocket->write(QString("__READY__").toUtf8());
        chatDisplay->append("已发送准备状态，等待其他玩家...");
    }
}

void ChatWindow::onStartGameClicked() {
    int totalPlayers = clientSockets.size() + 1; // 包含房主
    if (totalPlayers < 2) {
        QMessageBox::warning(this, "人数不足", "至少需要两人才能开始游戏！");
        return;
    }
    int zhongchen = zhongchenSpin->value();
    int fanzai = fanzaiSpin->value();
    int neijian = neijianSpin->value();
    int identitySum = 1 + zhongchen + fanzai + neijian; // 1主公
    if (identitySum != totalPlayers) {
        QMessageBox::warning(this, "身份数量不匹配", QString("当前房间人数为%1，身份总数为%2，请调整身份数量使其与人数一致！").arg(totalPlayers).arg(identitySum));
        return;
    }

    // 获取选中的拓展包信息
    QStringList selectedExpansions = getSelectedExpansions();
    if (selectedExpansions.isEmpty()) {
        QMessageBox::warning(this, "拓展包未选择", "请至少选择一个拓展包！");
        return;
    }

    // 构造包含拓展包信息的开始游戏消息
    QString expansionsStr = selectedExpansions.join(",");
    QString gameStartMsg = QString("__START_GAME__:%1").arg(expansionsStr);

    // 服务端通知所有客户端开始游戏
    for (QTcpSocket *client : clientSockets) {
        client->write(gameStartMsg.toUtf8());
    }
    chatDisplay->append("已通知所有玩家开始游戏");
    chatDisplay->append("使用拓展包: " + selectedExpansions.join("、"));
    onGameStart(); // 服务端自己也进入游戏
}

void ChatWindow::onClientReady(QTcpSocket *client, bool ready) {
    clientReadyMap[client] = ready;
    int readyCount = 0;
    for (auto r : clientReadyMap.values()) {
        if (r) readyCount++;
    }
    chatDisplay->append(QString("当前准备人数: %1/%2").arg(readyCount).arg(clientSockets.size()));
    if (readyCount == clientSockets.size() && clientSockets.size() > 0) {
        startGameBtn->setVisible(true);
    } else {
        startGameBtn->setVisible(false);
    }
}

void ChatWindow::onGameStart() {
    if (gameWindow) {
        // 如果已经有游戏窗口，先关闭它
        gameWindow->close();
        gameWindow = nullptr;
    }

    gameWindow = new GameWindow();
    // 连接窗口关闭信号，以便正确清除指针
    connect(gameWindow, &QObject::destroyed, this, [this]() {
        gameWindow = nullptr;
    });

    // 连接窗口关闭信号，通知其他玩家
    connect(gameWindow, &GameWindow::gameWindowClosed, this, &ChatWindow::onGameWindowClosed);

    connect(gameWindow, &GameWindow::assignIdentitiesRequested, this, &ChatWindow::onAssignIdentities);

    gameWindow->setAttribute(Qt::WA_DeleteOnClose);
    gameWindow->show();

    // 只有房主显示分配身份按钮
    if (tcpServer) {
        gameWindow->showAssignButton(true);
        // 房主获取并设置当前选中的拓展包信息
        QStringList selectedExpansions = getSelectedExpansions();
        gameWindow->setExpansions(selectedExpansions);
        chatDisplay->append("已选择拓展包: " + selectedExpansions.join("、"));
    } else {
        gameWindow->showAssignButton(false);
    }
}

void ChatWindow::onAssignIdentities() {
    // 服务端分配身份
    if (!tcpServer) return;
    int totalPlayers = clientSockets.size() + 1;
    QList<QString> identities;
    identities << "主公";
    for (int i = 0; i < zhongchenSpin->value(); ++i) identities << "忠臣";
    for (int i = 0; i < fanzaiSpin->value(); ++i) identities << "反贼";
    for (int i = 0; i < neijianSpin->value(); ++i) identities << "内奸";
    // 随机打乱
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(identities.begin(), identities.end(), g);

    // 获取当前选择的拓展包
    QStringList selectedExpansions = getSelectedExpansions();

    // 从选择的拓展包中获取武将池
    QList<General> generalPool;
    for (const General &general : kAllGenerals) {
        if (selectedExpansions.contains(general.expansion)) {
            generalPool.append(general);
        }
    }

    // 分配给所有人（房主第一个）
    QList<QTcpSocket*> allSockets = clientSockets;
    allSockets.prepend(nullptr); // 房主用nullptr

    for (int i = 0; i < identities.size(); ++i) {
        QString msg = "__IDENTITY__:" + identities[i];
        if (i == 0) {
            // 房主
            gameWindow->setIdentity(identities[i]);

            // 如果房主是主公，则提供武将选择
            if (identities[i] == "主公") {
                // 随机抽取8个武将供主公选择
                if (generalPool.size() < 8) {
                    chatDisplay->append("警告：武将池不足8个，将使用所有可用武将");
                }

                // 打乱武将池并取前8个(或全部)
                std::shuffle(generalPool.begin(), generalPool.end(), g);
                QList<General> lordGenerals;
                int count = qMin(8, generalPool.size());
                for (int j = 0; j < count; ++j) {
                    lordGenerals.append(generalPool[j]);
                }

                // 移除这些武将，避免其他玩家重复抽到
                for (int j = 0; j < count; ++j) {
                    generalPool.removeAt(0);
                }

                // 连接武将选择信号
                connect(gameWindow, &GameWindow::generalSelected, this, [this](const QString &generalName) {
                    chatDisplay->append("你选择了武将: " + generalName);
                });

                // 显示武将选择对话框
                gameWindow->showGeneralSelectionDialog(lordGenerals);
            }
        } else {
            allSockets[i]->write(msg.toUtf8());

            // 为其他玩家分配随机武将
            if (!generalPool.isEmpty()) {
                // 随机抽取一个武将
                std::random_device rd;
                std::mt19937 rng(rd());
                std::uniform_int_distribution<int> dist(0, generalPool.size() - 1);
                int randomIndex = dist(rng);
                QString generalName = generalPool[randomIndex].name;
                generalPool.removeAt(randomIndex);

                // 发送武将信息给玩家
                QString generalMsg = "__GENERAL__:" + generalName;
                allSockets[i]->write(generalMsg.toUtf8());
            }
        }
    }
}

// 处理游戏窗口关闭事件
void ChatWindow::onGameWindowClosed() {
    // 重置游戏窗口指针
    gameWindow = nullptr;

    // 重置所有玩家的准备状态
    if (tcpServer) {
        // 服务端：通知所有客户端游戏已结束，需要重新准备
        QString msg = "__GAME_ENDED__";
        for (QTcpSocket *client : clientSockets) {
            clientReadyMap[client] = false; // 重置客户端的准备状态
            client->write(msg.toUtf8());
        }

        // 显示等待玩家准备的消息
        chatDisplay->append("游戏已结束，等待玩家重新准备...");
        startGameBtn->setVisible(false); // 隐藏开始游戏按钮，等待玩家准备

        // 重新显示设置控件
        identityLabel->setVisible(true);
        zhongchenSpin->setVisible(true);
        fanzaiSpin->setVisible(true);
        neijianSpin->setVisible(true);
        expansionGroupBox->setVisible(true);
    } else if (tcpSocket) {
        // 客户端：重置准备状态
        isReady = false;
        readyBtn->setEnabled(true);
        readyBtn->setVisible(true);
        chatDisplay->append("游戏已结束，请重新准备...");

        // 客户端向服务端发送关闭窗口消息
        if(tcpSocket->state() == QAbstractSocket::ConnectedState) {
            tcpSocket->write("__CLOSE_GAME_WINDOW__");
        }
    }
}

// 获取选中的拓展包列表
QStringList ChatWindow::getSelectedExpansions() {
    QStringList expansions;
    if (standardPackCheckBox->isChecked()) {
        expansions << "标准包";
    }
    if (windPackCheckBox->isChecked()) {
        expansions << "风包";
    }
    if (firePackCheckBox->isChecked()) {
        expansions << "火包";
    }
    return expansions;
}

// 实现退出房间功能
void ChatWindow::onLeaveRoom() {
    // 确认是否真的要退出
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认退出", "确定要退出当前房间吗？",
                                                             QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    // 如果游戏窗口还在，先关闭它
    if (gameWindow) {
        gameWindow->close();
        gameWindow = nullptr;
    }

    // 处理服务器角色的情况
    if (tcpServer) {
        // 如果是服务器，则通知所有客户端服务器要关闭
        for (QTcpSocket *client : clientSockets) {
            client->write(QString("服务器已关闭，房间已解散！").toUtf8());
            client->disconnectFromHost();
        }
        clientSockets.clear();
        clientReadyMap.clear();

        // 关闭各种网络服务
        if (tcpServer) {
            tcpServer->close();
            delete tcpServer;
            tcpServer = nullptr;
        }
        if (broadcastTimer) {
            broadcastTimer->stop();
            delete broadcastTimer;
            broadcastTimer = nullptr;
        }
        if (udpSocket) {
            delete udpSocket;
            udpSocket = nullptr;
        }

        chatDisplay->append("已关闭房间服务器");
    }
    // 处理客户端角色的情况
    else if (tcpSocket) {
        // 如果是客户端，则断开与服务器的连接
        tcpSocket->disconnectFromHost();
        delete tcpSocket;
        tcpSocket = nullptr;
        chatDisplay->append("已退出房间");
    }

    // 重置UI状态
    readyBtn->setVisible(false);
    leaveRoomBtn->setVisible(false);
    startGameBtn->setVisible(false);
    isReady = false;

    // 重新启用房间设置控件
    bool isServer = (modeComboBox->currentIndex() == 0);
    openServerBtn->setVisible(isServer);
    searchBtn->setVisible(!isServer);
    serverList->setVisible(!isServer);
    joinBtn->setVisible(!isServer);

    // 清空聊天显示
    chatDisplay->clear();
}


