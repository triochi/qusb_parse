#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void do_decode(QString file_name);

private slots:
    void on_browseButton_clicked();

    void on_openBtn_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
