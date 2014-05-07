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
    QStringList input_list;
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    if (file.isOpen()){
        input_list = QString(file.readAll()).split('\n');
        foreach(QString str,input_list){
             ui->decoded_text->append(str);
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
