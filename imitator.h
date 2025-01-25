#ifndef IMITATOR_H
#define IMITATOR_H

#include "model.h"
#include <QMainWindow>
#include <QTimer>
#include <QTcpSocket>

enum class DataType {
    Message,
    Vector
};

QT_BEGIN_NAMESPACE
namespace Ui {
class Imitator;
}
QT_END_NAMESPACE

class Imitator : public QMainWindow
{
    Q_OBJECT

public:
    Imitator(QWidget *parent = nullptr);
    ~Imitator();

private slots:
    void on_start_stopButton_clicked();
    void modelHandler();
    void connectToHost(QString host, quint16 port);
    void sendMessage(const QString& message);
    void sendVector(const std::vector<double>& data);
    void onConnected();
    void onDisconnected();
    void onMsgReceived();
    void on_linearsButton_clicked();
    void on_anglesButton_clicked();
    void on_overloadsButton_clicked();
    void on_connectButton_clicked();
    void on_showGraphics_triggered();
    void on_showValues_triggered();
    void on_setParamsButton_clicked();
    void on_setDesiredButton_clicked();
    bool isDouble(QString str);

private:
    Ui::Imitator *ui;
    QTimer modelTimer;
    double time;
    double dt;
    Model *model;
    QTcpSocket *socket;

};
#endif // IMITATOR_H
