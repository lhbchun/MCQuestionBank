#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <vector>
#include "QuestionBank.h"
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPushButton *importButton;
    QVector<QRadioButton*> radioButton;
    QVector<QCheckBox*> checkbox;
    QVector<QLabel*> answerLabel;

    QVector<int> refreshQuizQuestion();
    void resetWindow();
    bool checkSolution(QVector<int> shuffledSolutions);
    void updateStatus();
    QVector<int> getUserAnswer();

    QuestionBank* QB;


private slots:
    void testClicked();
    void import();
    void importSubset();
    void exportSubset();
    void reset();

    void prev();
    void next();
    void check();
    void checkAll();
};
#endif // MAINWINDOW_H
