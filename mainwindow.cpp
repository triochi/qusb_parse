#include "mainwindow.h"
#include "qfiledialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::do_decode(QString file_name)
{
    // values fro bmRequest type

    enum data_phase_transfer_direction {HostToDevice,DeviceToHost};
    enum setup_types {Standard, Class, Vendor, Reserved};
    enum setup_recepients { Device, Interface, Endpoint, Other, Reserved_recipient};
    QStringList data_phase_transfer_direction_list= QStringList() <<"HostToDevice"<<"DeviceToHost";
    QStringList setup_types_list= QStringList() <<"Standard"<<"Class"<<"Vendor"<<"Reserved";
    QStringList setup_recepients_list= QStringList() <<"Device"<<"Interface"<<"Endpoint"<<"Other"<<"Reserved_recipient";
    typedef   struct
    {
        char direction;
        char type;
        char recepient;
    } bmRequestType_t;

    typedef struct  {
        bmRequestType_t bmRequestType;
        char bRequest;
        short int wValue;
        short int wIndex;
        short int wLength;
    }setup_packet;

    QStringList input_list;
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    volatile  int d;
    bool isOK;
    if (file.isOpen()){
        setup_packet packet;
        input_list = QString(file.readAll()).split('\n');
        foreach(QString str,input_list){
            QStringList data_list = str.split(' ');
            if(data_list.count()>6){
                if(data_list.at(4) == "s"){
                   d = data_list.at(5).toInt(&isOK,16);
                    packet.bmRequestType.direction = (d>>7) & 0xFF;
                    packet.bmRequestType.type = (d>>5) & 0x03;
                    packet.bmRequestType.recepient = d & 0x1F;
                    packet.bRequest = data_list.at(6).toInt(&isOK,16);
                    packet.wValue = data_list.at(7).toInt(&isOK,16);
                    packet.wIndex = data_list.at(8).toInt(&isOK,16);
                    packet.wLength = data_list.at(9).toInt(&isOK,16);

                    ui->decoded_text->append(QString("[") + data_phase_transfer_direction_list.at( packet.bmRequestType.direction) + "]"+
                                             " [" + setup_types_list.at( packet.bmRequestType.type) + "]" +
                                             " [" + setup_recepients_list.at( packet.bmRequestType.recepient) + "] " +
                                             "\tbRequest=" + QString::number(packet.bRequest,16) +
                                             "\tbwValue=" + QString::number(packet.wValue,16) +
                                             "\tbwIndex=" + QString::number(packet.wIndex,16) +
                                             "\tbwLength=" + QString::number(packet.wLength,16) );
                }

                ui->decoded_text->append(str);
            }

        }


        file.close();
    }

}

void MainWindow::on_browseButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "",
                                                     tr("Files (*.*)"));
    if (!fileName.isEmpty()) ui->inputFileEdit->setText(fileName);

}

void MainWindow::on_openBtn_clicked()
{
    if(QFile::exists( ui->inputFileEdit->text())){
        do_decode(ui->inputFileEdit->text());
    }
}
