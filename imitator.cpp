#include "imitator.h"
#include "ui_imitator.h"

Imitator::Imitator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Imitator)
{
    ui->setupUi(this);
    ui->valuesWidget->setVisible(false);
    ui->valuesWidget->setStyleSheet("background-color:white;");
    ui->centralwidget->setStyleSheet("background-color:white;");

    ui->xPlot->addGraph();
    ui->xPlot->xAxis->setLabel("Время, сек");
    ui->xPlot->yAxis->setLabel("Координата X, м");
    ui->yPlot->addGraph();
    ui->yPlot->xAxis->setLabel("Время, сек");
    ui->yPlot->yAxis->setLabel("Координата Y, м");
    ui->zPlot->addGraph();
    ui->zPlot->xAxis->setLabel("Время, сек");
    ui->zPlot->yAxis->setLabel("Координата Z, м");
    ui->vPlot->addGraph();
    ui->vPlot->xAxis->setLabel("Время, сек");
    ui->vPlot->yAxis->setLabel("Скорость V, м/с");

    ui->thetaPlot->addGraph();
    ui->thetaPlot->xAxis->setLabel("Время, сек");
    ui->thetaPlot->yAxis->setLabel("Угол Θ, град.");
    ui->psiPlot->addGraph();
    ui->psiPlot->xAxis->setLabel("Время, сек");
    ui->psiPlot->yAxis->setLabel("Угол Ψ, град.");
    ui->gammaPlot->addGraph();
    ui->gammaPlot->xAxis->setLabel("Время, сек");
    ui->gammaPlot->yAxis->setLabel("Угол γ, град.");

    ui->nxPlot->addGraph();
    ui->nxPlot->xAxis->setLabel("Время, сек");
    ui->nxPlot->yAxis->setLabel("Перегрузка nx");
    ui->nyPlot->addGraph();
    ui->nyPlot->xAxis->setLabel("Время, сек");
    ui->nyPlot->yAxis->setLabel("Перегрузка ny");
    ui->nzPlot->addGraph();
    ui->nzPlot->xAxis->setLabel("Время, сек");
    ui->nzPlot->yAxis->setLabel("Перегрузка nz");

    ui->thetaPlot->hide();
    ui->psiPlot->hide();
    ui->gammaPlot->hide();
    ui->nxPlot->hide();
    ui->nyPlot->hide();
    ui->nzPlot->hide();

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &Imitator::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Imitator::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &::Imitator::onMsgReceived);

    ui->connectionRadioButton->setEnabled(false);

    model = new Model();
    model->setDesired(35, 50, (0 * M_PI) / 180);               //ДЕФОЛТНЫЙ НАБОР ЖЕЛАЕМЫХ ПАРАМЕТРОВ

    time = 0;
    dt = 0.1;
    connect(&modelTimer, &QTimer::timeout, this, &Imitator::modelHandler);
    modelTimer.setInterval(100);
}

Imitator::~Imitator()
{
    delete ui;
}

void Imitator::on_start_stopButton_clicked()
{
    ui->connectButton->setEnabled(false);
    ui->setParamsButton->setEnabled(false);
    if (socket != nullptr && socket->state() == QTcpSocket::ConnectedState) {
        ui->setDesiredButton->setEnabled(false); //ЕСЛИ ПОДКЛЮЧЕНО К СЕРВУ, ТО ЖЕЛАЕМЫЕ ТОЛЬКО С СЕРВА
    }

    if (!modelTimer.isActive()) {
        modelTimer.start();
        qDebug() << "Моделирование";
        ui->start_stopButton->setText("Стоп");
    } else {
        modelTimer.stop();
        qDebug() << "Остановка моделирования";
        ui->start_stopButton->setText("Стоп");
    }
}

void Imitator::modelHandler() {
    time += 0.1;
    std::vector<double> x = model->rungeKutta(0.1);

    ui->xPlot->graph(0)->addData(time, x[0]);
    ui->xPlot->replot();
    ui->xPlot->graph(0)->rescaleAxes();
    ui->xPlot->setInteraction(QCP::iRangeDrag, true);

    ui->yPlot->graph(0)->addData(time, x[1]);
    ui->yPlot->replot();
    ui->yPlot->graph(0)->rescaleAxes();
    ui->yPlot->setInteraction(QCP::iRangeDrag, true);

    ui->zPlot->graph(0)->addData(time, x[2]);
    ui->zPlot->replot();
    ui->zPlot->graph(0)->rescaleAxes();
    ui->zPlot->setInteraction(QCP::iRangeDrag, true);

    ui->vPlot->graph(0)->addData(time, x[3]);
    ui->vPlot->replot();
    ui->vPlot->graph(0)->rescaleAxes();
    ui->vPlot->setInteraction(QCP::iRangeDrag, true);

    ui->thetaPlot->graph(0)->addData(time, x[4] * 180 / M_PI);
    ui->thetaPlot->replot();
    ui->thetaPlot->graph(0)->rescaleAxes();
    ui->thetaPlot->setInteraction(QCP::iRangeDrag, true);

    ui->psiPlot->graph(0)->addData(time, x[5] * 180 / M_PI);
    ui->psiPlot->replot();
    ui->psiPlot->graph(0)->rescaleAxes();
    ui->psiPlot->setInteraction(QCP::iRangeDrag, true);

    ui->gammaPlot->graph(0)->addData(time, x[6] * 180 / M_PI);
    ui->gammaPlot->replot();
    ui->gammaPlot->graph(0)->rescaleAxes();
    ui->gammaPlot->setInteraction(QCP::iRangeDrag, true);

    ui->nxPlot->graph(0)->addData(time, x[7]);
    ui->nxPlot->replot();
    ui->nxPlot->graph(0)->rescaleAxes();
    ui->nxPlot->setInteraction(QCP::iRangeDrag, true);

    ui->nyPlot->graph(0)->addData(time, x[8]);
    ui->nyPlot->replot();
    ui->nyPlot->graph(0)->rescaleAxes();
    ui->nyPlot->setInteraction(QCP::iRangeDrag, true);

    ui->nzPlot->graph(0)->addData(time, x[10]);
    ui->nzPlot->replot();
    ui->nzPlot->graph(0)->rescaleAxes();
    ui->nzPlot->setInteraction(QCP::iRangeDrag, true);

    if (socket != nullptr && socket->state() == QTcpSocket::ConnectedState) {
        sendVector(x);
    }
}

void Imitator::on_linearsButton_clicked()
{
    ui->xPlot->show();
    ui->yPlot->show();
    ui->zPlot->show();
    ui->vPlot->show();
    ui->thetaPlot->hide();
    ui->psiPlot->hide();
    ui->gammaPlot->hide();
    ui->nxPlot->hide();
    ui->nyPlot->hide();
    ui->nzPlot->hide();
}


void Imitator::on_anglesButton_clicked()
{
    ui->thetaPlot->show();
    ui->psiPlot->show();
    ui->gammaPlot->show();
    ui->xPlot->hide();
    ui->yPlot->hide();
    ui->zPlot->hide();
    ui->vPlot->hide();
    ui->nxPlot->hide();
    ui->nyPlot->hide();
    ui->nzPlot->hide();
}


void Imitator::on_overloadsButton_clicked()
{
    ui->nxPlot->show();
    ui->nyPlot->show();
    ui->nzPlot->show();
    ui->xPlot->hide();
    ui->yPlot->hide();
    ui->zPlot->hide();
    ui->vPlot->hide();
    ui->thetaPlot->hide();
    ui->psiPlot->hide();
    ui->gammaPlot->hide();
}

void Imitator::connectToHost(QString host, quint16 port)
{
    socket->connectToHost(host, port);
}

void Imitator::sendVector(const std::vector<double> &data) {
    QByteArray vectorData;
    QDataStream out(&vectorData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    out << DataType::Vector;
    out << (quint32)data.size();
    for (double value : data) {
        out << value;
    }
    socket->write(vectorData);
}

void Imitator::sendMessage(const QString &message) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << DataType::Message;
    out << message.toUtf8();
    socket->write(block);
}

void Imitator::onConnected()
{
    qDebug() << "Client connected to server.";
}

void Imitator::onDisconnected()
{
    qDebug() << "Client disconnected from server.";
}

void Imitator::onMsgReceived()
{
    QByteArray data = socket->readAll();
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_6_0);

    DataType type;
    in >> type;

    if (type == DataType::Message) {
        QByteArray messageData;
        in >> messageData;
        QString message = QString::fromUtf8(messageData);
        qDebug() << "Получено сообщение:" << message;

        if (message == "1" || message == "3" || message == "0") {
            qDebug() << "Остановка моделирования";
            modelTimer.stop();
            ui->start_stopButton->setText("Старт");
            ui->start_stopButton->setEnabled(false);
        } else {
        }
    } else if (type == DataType::Vector) {
        quint32 size;
        in >> size;
        std::vector<double> receivedData(size);
        for (int i = 0; i < size; ++i) {
            in >> receivedData[i];
        }
        qDebug() << "Полученный вектор:";
        for (double value : receivedData) {
            qDebug() << value;
        }

        if (size == 3) {
            model->setDesired(receivedData[0], receivedData[1], receivedData[2]);
        } else {
            qDebug() << "Моделирование";
            modelTimer.start();
            ui->start_stopButton->setText("Стоп");
            ui->start_stopButton->setEnabled(true);
            model->setCurrentX(std::vector<double>(receivedData.begin(), receivedData.end() - 3));
            model->setDesired(receivedData[11], receivedData[12], receivedData[13]);
        }
    } else {
        qDebug() << "Неизвестный тип данных";
    }
}

void Imitator::on_connectButton_clicked()
{
    ui->connectionRadioButton->setEnabled(true);
    ui->connectionRadioButton->click();
    ui->connectionRadioButton->setEnabled(false);

    ui->connectButton->setEnabled(false);
    ui->start_stopButton->setEnabled(true);

    connectToHost("127.0.0.1", 12345);
    sendMessage("Hello from client!");
}


void Imitator::on_showGraphics_triggered()
{
    ui->valuesWidget->setVisible(false);
    this->setGeometry(0, 0, 1074, 792);
}


void Imitator::on_showValues_triggered()
{
    ui->valuesWidget->setVisible(true);
    this->setGeometry(0, 0, 980, 580);
}


void Imitator::on_setParamsButton_clicked()
{
    if (isDouble(ui->xLineEdit->text()) &&
        isDouble(ui->yLineEdit->text()) &&
        isDouble(ui->zLineEdit->text()) &&
        isDouble(ui->vLineEdit->text()) &&
        isDouble(ui->thetaLineEdit->text()) &&
        isDouble(ui->psiLineEdit->text()) &&
        isDouble(ui->gammaLineEdit->text()) &&
        isDouble(ui->nxLineEdit->text()) &&
        isDouble(ui->nyLineEdit->text()) &&
        isDouble(ui->nzLineEdit->text()) &&
        ui->vLineEdit->text().toDouble() != 0) {

        model->setCurrentX({ui->xLineEdit->text().toDouble(),
                            ui->yLineEdit->text().toDouble(),
                            ui->zLineEdit->text().toDouble(),
                            ui->vLineEdit->text().toDouble(),
                            ui->thetaLineEdit->text().toDouble(),
                            ui->psiLineEdit->text().toDouble(),
                            ui->gammaLineEdit->text().toDouble(),
                            ui->nxLineEdit->text().toDouble(),
                            ui->nyLineEdit->text().toDouble(),
                            0,
                            ui->nzLineEdit->text().toDouble()});
        qDebug() << "Начальные значения заданы";
    }
}


void Imitator::on_setDesiredButton_clicked()
{
    if (isDouble(ui->vDesLineEdit->text()) &&
        isDouble(ui->yDesLineEdit->text()) &&
        isDouble(ui->psiDesLineEdit->text())) {

        model->setDesired(ui->vDesLineEdit->text().toDouble(),
                          ui->yDesLineEdit->text().toDouble(),
                          ui->psiDesLineEdit->text().toDouble());
        qDebug() << "Желаемые значения заданы";
        qDebug() << ui->vDesLineEdit->text().toDouble() << ui->yDesLineEdit->text().toDouble() << ui->psiDesLineEdit->text().toDouble();
    }
}


bool Imitator::isDouble(QString str) {
    bool ok;
    str.toDouble(&ok);
    return ok;
}
