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

    // Request types
     enum request_types { GET_STATUS, CLEAR_FEATURE, UNKNOWN02,
                          SET_FEATURE, UNKNOWN04, SET_ADDRESS, GET_DESCRIPTOR,
                          SET_DESCRIPTOR, GET_CONFIGURATION,SET_CONFIGURATION};

    QStringList request_types_list= QStringList() <<"GET_STATUS "<<"CLEAR_FEATURE "<<"UNKNOWN(02)"
                                                       <<"SET_FEATURE"<<"UNKNOWN(04)"<<"SET_ADDRESS"<<"GET_DESCRIPTOR "
                                                       <<"SET_DESCRIPTOR"<<"GET_CONFIGURATION"<<"SET_CONFIGURATION"
                                                       <<"UNKNOWN10"<<"UNKNOWN11"<<"UNKNOWN12";

    // Descriptor types
    enum descriptor_types { descr_BadType, descr_Device, descr_Configuration, descr_String, descr_Interface,
                         descr_Endpoint, descr_Device_qualifier, descr_Other_speed_configuration, descr_Interface_power,
                         descr_On_the_go};

    QStringList descriptor_types_list= QStringList() <<"UNKNOWN0 "<<"Device "<<"Configuration"
                                                      <<"String"<<"Interface"<<"Endpoint"<<"Device_qualifier "
                                                      <<"Other_speed_configuration"<<"Interface_power"<<"On_the_go"<<"UNKNOWN1";

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
        int line_number = 0;
        setup_packet packet;
        input_list = QString(file.readAll()).split('\n');
        foreach(QString str,input_list){
            line_number++;
            if (line_number> 200) break;
            QStringList data_list = str.split(' ');
            if(data_list.count()>6){

                if (data_list.at(3).contains("00"+ QString::number(ui->deviceID_box->value())) ) continue;
                if (line_number == 24){
                    line_number +=1;
                    line_number--;
                }
                if(data_list.at(2) == "S"){
                    QString text;

                    d = data_list.at(5).toInt(&isOK,16);
                    packet.bmRequestType.direction = (d>>7) & 0xFF;
                    packet.bmRequestType.type = (d>>5) & 0x03;
                    packet.bmRequestType.recepient = d & 0x1F;
                    if(packet.bmRequestType.recepient > Reserved_recipient) packet.bmRequestType.recepient = Reserved_recipient;
                    QString requestType;
                    if(data_list.at(4) == "s"){
                        packet.bRequest = data_list.at(6).toInt(&isOK,16);
                        packet.wValue = data_list.at(7).toInt(&isOK,16);
                        packet.wIndex = data_list.at(8).toInt(&isOK,16);
                        packet.wLength = data_list.at(9).toInt(&isOK,16);
                        if( (unsigned)packet.bRequest< request_types_list.count()){
                            requestType = request_types_list.at( packet.bRequest);
                        } else {
                            requestType = "requestType (" + QString::number( packet.bRequest,16);
                        }
                    }
                    QString type_and_direction = data_list.at(3).split(':').at(0);
                    if (type_and_direction == "Ci") type_and_direction = "[Control input]";
                    if (type_and_direction == "Co") type_and_direction = "[Control output]";
                    if (type_and_direction == "Zi") type_and_direction = "[Isochronous input]";
                    if (type_and_direction == "Zi") type_and_direction = "[Isochronous output]";
                    if (type_and_direction == "Ii") type_and_direction = "[Interrupt input]";
                    if (type_and_direction == "Ii") type_and_direction = "[Interrupt output]";
                    if (type_and_direction == "Bi") type_and_direction = "[Bulk input]";
                    if (type_and_direction == "Bi") type_and_direction = "[Bulk output]";



                    text = ("[Sumission]\t" + type_and_direction + QString("\t[DataDirection=") + data_phase_transfer_direction_list.at( packet.bmRequestType.direction) + "]"+
                                             "\t[Type=" + setup_types_list.at( packet.bmRequestType.type) + "]" +
                                             "\t[Recepient=" + setup_recepients_list.at( packet.bmRequestType.recepient) + "] ");

                    if(data_list.at(4) == "s"){
                        text +=                  //"\tbRequest=" + QString::number(packet.bRequest,16) +
                                             "\t[" + requestType + "] ";
                        if (packet.bRequest == GET_DESCRIPTOR){
                            int descriptor_type = packet.wValue >> 8;
                               text += "\t<" + descriptor_types_list.at(descriptor_type) + ">";
                        } else {
                            text += "\tbwValue=" + QString::number(packet.wValue,16);
                        }

                        text +=
                                             "\tbwIndex=" + QString::number(packet.wIndex,16) +
                                             "\tbwLength=" + QString::number(packet.wLength,16) ;
                    }

                    ui->decoded_text->append(text);
                }
                else if(data_list.at(2) == "C"){
                     ui->decoded_text->append("[Callback]");
                }
                else if(data_list.at(2) == "E"){
                    ui->decoded_text->append("[Callback error]");
                }
            }
            ui->decoded_text->append(QString::number(line_number)+ ">>" + str);
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
