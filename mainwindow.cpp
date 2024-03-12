#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QSettings>
#include <QMetaEnum>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QMessageLogger>
#include <QClipboard>

using algor = QCryptographicHash::Algorithm;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("文件哈希校验器 - FileValidator @Kpure1000"));
    auto methodType = QMetaEnum::fromType<algor>();
    QSettings qst("qfilevalidator.ini", QSettings::Format::IniFormat, this);
    auto lastIndex = qst.value("LastMethodIndex", 0).toInt();
    for (int i = 0; i < methodType.keyCount() - 1; i++) { // -1是因为 NumAlgorithms
        ui->m_comboBox->addItem(methodType.valueToKey(methodType.value(i)));
    }
    ui->m_comboBox->setCurrentIndex(lastIndex);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_m_btBrowse_clicked()
{
    QSettings qst("qfilevalidator.ini", QSettings::Format::IniFormat, this);
    auto lastPath = qst.value("LastFilePath").toString();
    auto fileToBeSelected = QFileDialog::getOpenFileName(this, tr("浏览文件"), lastPath, "Any (*.*)");
    if (!fileToBeSelected.isEmpty()) {
        m_selectedFile = fileToBeSelected;

        auto absPath = QFileInfo(m_selectedFile).absoluteFilePath();
        qst.setValue("LastFilePath", absPath);
        ui->m_lineEdit->setText(absPath);
    }
}

void MainWindow::on_m_comboBox_currentIndexChanged(int index)
{
    QSettings qst("qfilevalidator.ini", QSettings::Format::IniFormat, this);
    qst.setValue("LastMethodIndex", index);
    auto hashMethod = static_cast<algor>(QMetaEnum::fromType<algor>().value(index));
    m_hashFunc = [=](QByteArrayView data){
        return QCryptographicHash::hash(data, hashMethod);
    };
}

void MainWindow::on_m_btRun_clicked()
{
    ui->m_btRun->setEnabled(false);
    if (!m_selectedFile.isEmpty()) {
        auto fileInfo = QFileInfo(m_selectedFile);
        if (fileInfo.exists()) {
            auto file = QFile(fileInfo.absoluteFilePath());
            if (file.open(QIODevice::ReadOnly)) {
                QByteArray data = file.readAll();
                file.close();
                auto hash = m_hashFunc(data).toHex(); // 要转换成16进制
                ui->m_textEdit->setText(hash);
                qDebug() << "计算完毕, hash =" << hash;

                if (m_ischeck) {
                    if (ui->m_compare->text() == ui->m_textEdit->toPlainText()) {
                        qDebug() << "比对成功";
                        QMessageBox::information(this, tr("比对结果"), tr("比对成功"));
                    } else {
                        qDebug() << "比对失败";
                        QMessageBox::warning(this, tr("比对结果"), tr("比对失败"));
                    }
                }

            } else {
                qDebug() << "文件打不开啊";
                QMessageBox::critical(this, tr("文件打不开啊"),tr("是不是被占用了? 或者没了?"));
            }
        } else {
            qDebug() << "文件没了啊";
            QMessageBox::critical(this, tr("文件没了啊"),tr("只怕是删了吧"));
        }
    } else {
        qDebug() << "路径空的啊";
        QMessageBox::critical(this, tr("路径空的啊"),tr("只怕是没选文件吧"));
    }
    ui->m_btRun->setEnabled(true);
}


void MainWindow::on_m_checkBox_stateChanged(int arg1)
{
    ui->m_compare->setEnabled(arg1);
    m_ischeck = arg1;
}


void MainWindow::on_m_bt_copy_clicked()
{
    auto clipboard = QApplication::clipboard();
    auto hash = ui->m_textEdit->toPlainText();
    if (!hash.isEmpty()) {
        clipboard->setText(hash);
        qDebug() << "成功将" << hash << "复制到剪切板";
        QMessageBox::information(this, tr("提示"), QString(tr("成功将 '%1' 复制到剪切板")).arg(hash));
    }
}

