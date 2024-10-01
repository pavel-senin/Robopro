#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <cmath>
#include <QRegularExpression>
#include <QLineEdit>
#include <QJSEngine>
#include "dh_model.h"

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

    void deleteJoint();
    void addJoint();
    void computeResult();
    void info();

private slots:
    void validateCellInput(QLineEdit *lineEdit, int row, int col);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
