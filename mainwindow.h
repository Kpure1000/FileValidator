#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_m_comboBox_currentIndexChanged(int index);

    void on_m_btBrowse_clicked();

    void on_m_btRun_clicked();

    void on_m_checkBox_stateChanged(int arg1);

    void on_m_bt_copy_clicked();

private:
    Ui::MainWindow *ui;

    QString m_selectedFile = "";

    std::function<QByteArray(QByteArrayView)> m_hashFunc;

    bool m_ischeck = false;
};
#endif // MAINWINDOW_H
