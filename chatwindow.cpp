#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "chatclient.h"
#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QHostAddress>
ChatWindow::ChatWindow(QWidget *parent)
    :QWidget(parent)
    ,ui(new Ui::ChatWindow)
    ,m_chatClient(new ChatClient(this))
    ,m_chatModel(new QStandardItemModel(this))
{
    ui->setupUi(this);
    m_chatModel->insertColumn(0);
    ui->chatView->setModel(m_chatModel);
    connect(m_chatClient,&ChatClient::connected,this,&ChatWindow::connectedToServer);
    connect(m_chatClient,&ChatClient::loggedIn,this,&ChatWindow::loggedIn);
    connect(m_chatClient,&ChatClient::loginError,this,&ChatWindow::loginFailed);
    connect(m_chatClient,&ChatClient::messageReceived,this,&ChatWindow::messageReceived);
    connect(m_chatClient,&ChatClient::disconnected ,this,&ChatWindow::disconnectedFromServer);
    connect(m_chatClient,&ChatClient::error,this,&ChatWindow::disconnectedFromServer);
    connect(m_chatClient,&ChatClient::userJoined,this,&ChatWindow::userJoined);
    connect(m_chatClient,&ChatClient::userLeft,this,&ChatWindow::userLeft);
    connect(ui->connectButton,&QPushButton::clicked,this,&ChatWindow::attemptConnection);
    connect(ui->sendButton,&QPushButton::clicked,this,&ChatWindow::sendMessage);
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::attemptConnection()
{
    ui->connectButton->setEnabled(false);
    m_chatClient->connectToServer(QHostAddress::LocalHost,1967);
}

void ChatWindow::connectedToServer()
{
    const QString newUsername = QInputDialog::getText(this,tr("Chose Username"),tr("Username"));
    if(newUsername.isEmpty())
        return m_chatClient->disconnectFromHost();
    attemptLogin(newUsername);
}

void ChatWindow::attemptLogin(const QString &userName)
{
    m_chatClient->login(userName);
}

void ChatWindow::loggedIn()
{
    ui->connectButton->setEnabled(false);
    ui->sendButton->setEnabled(true);
    ui->messageEdit->setEnabled(true);
    ui->chatView->setEnabled(true);
}

void ChatWindow::loginFailed(const QString &reason)
{
    QMessageBox::critical(this,tr("Error"),reason);
    connectedToServer();
}

void ChatWindow::messageReceived(const QString &sender, const QString &text)
{
    const int newRow = m_chatModel->rowCount();
    m_chatModel->insertRows(newRow,2);
    QFont boldFont;
    boldFont.setBold(true);
    m_chatModel->setData(m_chatModel->index(newRow,0),sender + ':');
    m_chatModel->setData(m_chatModel->index(newRow+1,0),text);
    m_chatModel->setData(m_chatModel->index(newRow,0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
    m_chatModel->setData(m_chatModel->index(newRow+1,0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
    m_chatModel->setData(m_chatModel->index(newRow,0), boldFont, Qt::FontRole);
    ui->chatView->scrollToBottom();
}

void ChatWindow::sendMessage()
{
    m_chatClient->sendMessage(ui->messageEdit->text());
    const int newRow = m_chatModel->rowCount();
    m_chatModel->insertRow(newRow);
    m_chatModel->setData(m_chatModel->index(newRow,0),ui->messageEdit->text());
    m_chatModel->setData(m_chatModel->index(newRow,0), int(Qt::AlignRight | Qt::AlignVCenter), Qt::TextAlignmentRole);
    ui->messageEdit->clear();
}

void ChatWindow::disconnectedFromServer()
{
    ui->connectButton->setEnabled(true);
    ui->sendButton->setEnabled(false);
    ui->messageEdit->setEnabled(false);
    ui->chatView->setEnabled(false);
}

void ChatWindow::userJoined(const QString &username)
{
    const int newRow = m_chatModel->rowCount();
    m_chatModel->insertRow(newRow);
    m_chatModel->setData(m_chatModel->index(newRow,0), tr("%1 Joined the Chat").arg(username));
    m_chatModel->setData(m_chatModel->index(newRow,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    m_chatModel->setData(m_chatModel->index(newRow,0), QBrush(Qt::blue), Qt::ForegroundRole);
}
void ChatWindow::userLeft(const QString &username)
{
    const int newRow = m_chatModel->rowCount();
    m_chatModel->insertRow(newRow);
    m_chatModel->setData(m_chatModel->index(newRow,0), tr("%1 Left the Chat").arg(username));
    m_chatModel->setData(m_chatModel->index(newRow,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    m_chatModel->setData(m_chatModel->index(newRow,0), QBrush(Qt::red), Qt::ForegroundRole);
}