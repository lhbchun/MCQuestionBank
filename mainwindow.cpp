#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QGroupBox>
#include <QWidget>
#include <QDebug>
#include <QFileDialog>
#include "QuestionBank.h"


bool importVectorFromFile(QVector<int>& data, const QString& filename)
{
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);
        in >> data;
        file.close();
        return true;
    }
    else
    {
        // Failed to open the file for reading
        return false;
    }
}


bool exportVectorToFile(const QVector<int>& data, const QString& filename)
{
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out << data;
        file.close();
        return true;
    }
    else
    {
        // Failed to open the file for writing
        return false;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->importButton, SIGNAL(clicked()), this, SLOT(import()));
    connect(ui->importSubsetButton, SIGNAL(clicked()), this, SLOT(importSubset()));
    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportSubset()));


    connect(ui->prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(ui->checkButton, SIGNAL(clicked()), this, SLOT(check()));
    connect(ui->submitButton, SIGNAL(clicked()), this, SLOT(checkAll()));


    radioButton = QVector<QRadioButton*>();
    checkbox = QVector<QCheckBox*>();
    answerLabel = QVector<QLabel*>();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::testClicked(){
    QString file1Name = QFileDialog::getOpenFileName(this, tr("Open QB File 1"), "/home", tr("Question Bank Files (*.qb)"));
    qDebug() << "Opened File: " + file1Name;
}


void MainWindow::import(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open QB File 1"), "/home", tr("Question Bank Files (*.qb)"));
    if (fileName.isEmpty()){
        return;
    }
    QB = new QuestionBank(fileName);

    ui->prevButton->setDisabled(false);
    ui->submitButton->setDisabled(false);
    ui->checkButton->setDisabled(false);
    ui->nextButton->setDisabled(false);

    resetWindow();
    refreshQuizQuestion();

    ui->importSubsetButton->setDisabled(false);
    ui->resetButton->setDisabled(false);


}

void MainWindow::importSubset(){
    if (QB == nullptr) {return; }
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open QB File 1"), QDir::homePath(), tr("QB Subset Files (*.qbsubset)"));
    if (fileName.isEmpty()){
        return;
    }
    QVector<int> subset;
    importVectorFromFile(subset, fileName);
    QB->initSubset(subset);

    resetWindow();
    refreshQuizQuestion();
}

void MainWindow::exportSubset(){
    if (QB == nullptr) {return; }
    if (QB->getAttemptedCount() == QB->getCorrectCount()){return;}
    QVector<int> subset = QB->getIncorrect();
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "QB Subset Files (*.qbsubset)");
    exportVectorToFile(subset, fileName);
}

void MainWindow::reset(){
    if (QB == nullptr) {return; }
    QB->resetAll();
    resetWindow();
    refreshQuizQuestion();
    ui->exportButton->setDisabled(true);
}

void MainWindow::resetWindow(){
    ui->questionLabel->setStyleSheet("font: bold 16px;");
    ui->questionLabel->setText("");

    for (int i = radioButton.size() - 1; i>=0; i--){
        radioButton[i]->deleteLater();
        radioButton.pop_back();
    }

    for (int i = checkbox.size() - 1; i>=0; i--){
        checkbox[i]->deleteLater();
        checkbox.pop_back();
    }

    for (int i = answerLabel.size() - 1; i>=0; i--){
        answerLabel[i]->deleteLater();
        answerLabel.pop_back();
    }
}

QVector<int> MainWindow::refreshQuizQuestion(){
    // create the quiz and return the solution(s) as a vector

    QString question;
    QVector<QString> answers;
    QVector<int> solutions;

    if (QB == nullptr){
        return solutions;
    }


    question = QB->getCurrentQuestion();
    answers = QB->getCurrentAnswers();
    solutions = QB->getCurrentSolutions();


    //Setup question

    ui->questionLabel->setStyleSheet("font: bold 16px;");
    ui->questionLabel->setText(question);


    //Setup answers
    // Step 1: Get the number of solutions

    QVector<int> userSolution = QB->getUserSolutions();

    if (solutions.size() == 1){
        //create new radio choice
        for (int i = 0; i < answers.size(); i++){
            QRadioButton* button = new QRadioButton(this);
            button->setGeometry(50,320 +50*i,50,50);
            for (int j = 0; j < userSolution.size(); j++){
                if (userSolution[j] == i){
                    button->setChecked(userSolution[j]);
                }
            }
            radioButton.push_back(button);
            button->show();

            QLabel* label = new QLabel(this);
            label->setGeometry(100,320 + 50*i, 400,50);
            label->setText(answers[i]);
            label->setWordWrap(true);
            answerLabel.push_back(label);
            label->show();
        }

    } else {
        //create new checkbox choice

        for (int i = 0; i < answers.size(); i++){
            QCheckBox* button = new QCheckBox(this);
            button->setGeometry(50,320 +50*i,50,50);
            for (int j = 0; j < userSolution.size(); j++){
                if (userSolution[j] == i){
                    button->setChecked(userSolution[j]);
                }
            }
            checkbox.push_back(button);
            button->show();

            QLabel* label = new QLabel(this);
            label->setGeometry(100,320 + 50*i, 400,50);
            label->setText(answers[i]);
            label->setWordWrap(true);
            answerLabel.push_back(label);
            label->show();
        }
    }

    //Load Answers

    QVector<int> userAnswers = QB->getUserSolutions();
    bool isLocked = QB->isAnswerLocked();

    for (int i = 0; i < userAnswers.size(); i++){
        if (checkbox.size() != 0){
            checkbox[userAnswers[i]]->setChecked(true);
        }
        if (radioButton.size() != 0){
            radioButton[userAnswers[i]]->setChecked(true);
        }
    }

    for (int i = 0; i < checkbox.size(); i++){
        checkbox[i]->setDisabled(isLocked);
    }

    for (int i = 0; i < radioButton.size(); i++){
        radioButton[i]->setDisabled(isLocked);
    }

    if (isLocked){

        for (int i = 0; i < userAnswers.size(); i++){
            answerLabel[userAnswers[i]]->setStyleSheet("QLabel { color: red; font: bold;}");
        }

        QVector<int> currentSolution = QB->getCurrentSolutions();
        for (int i = 0; i < currentSolution.size(); i++){
            answerLabel[currentSolution[i]]->setStyleSheet("QLabel { color: green; font: bold;}");
        }

    }

    updateStatus();


    ui->prevButton->setDisabled(QB->getCurrentIndex() == 0);
    ui->nextButton->setDisabled(QB->getCurrentIndex() == QB->getQuestionCount() - 1);

    return solutions;
}

QVector<int> MainWindow::getUserAnswer(){
    //get the user input
    QVector<int> userAnswer = QVector<int>();
    for (int i = 0; i < radioButton.size(); i++){
        if (radioButton[i]->isChecked()){
            userAnswer.push_back(i);
        }
    }

    for (int i = 0; i < checkbox.size(); i++){
        if (checkbox[i]->isChecked()){
            userAnswer.push_back(i);
        }
    }
    return userAnswer;
}

void MainWindow::prev(){
    if (QB == nullptr) {return; }
    QB->saveAnswer(getUserAnswer());
    QB->prevQuestion();
    resetWindow();
    refreshQuizQuestion();

}
void MainWindow::next(){
    if (QB == nullptr) {return; }
    QB->saveAnswer(getUserAnswer());
    QB->nextQuestion();
    resetWindow();
    refreshQuizQuestion();

}
void MainWindow::check(){
    if (QB == nullptr) {return; }

    if (QB->isAnswerLocked()){
        return;
    }
    QVector<int> userAnswer = getUserAnswer();

    if (userAnswer.size() == 0){
        return;
    }
    QB->submit(userAnswer);
    resetWindow();
    refreshQuizQuestion();

    if (QB->getIncorrect().size() > 0){
        ui->exportButton->setDisabled(false);
    }
}
void MainWindow::checkAll(){
    if (QB == nullptr) {return; }
    check();
    QB->submitAll();
    resetWindow();
    refreshQuizQuestion();

    if (QB->getIncorrect().size() > 0){
        ui->exportButton->setDisabled(false);
    }
}


void MainWindow::updateStatus(){
    this->setWindowTitle("MCQuestionBank: " + QB->getFileName());

    int attempted = QB->getAttemptedCount();
    int correct = QB->getCorrectCount();
    if (attempted == 0){
        ui->questionBox->setTitle("Question: " + QString::number(QB->getCurrentIndex() + 1) + "/" +
                                  QString::number(QB->getQuestionCount()));
    }

    if (attempted > 0){
        ui->questionBox->setTitle("Question: " + QString::number(QB->getCurrentIndex() + 1) + "/" +
                                  QString::number(QB->getQuestionCount()) +
                                  " Correct: " + QString::number(correct) + "/" +
                                  QString::number(attempted) + " (" +
                                  QString::number((float)correct * 100/ (float)attempted, 'f', 2) +
                                  "%)");
    }
}
