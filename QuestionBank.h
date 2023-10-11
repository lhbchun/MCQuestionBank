#ifndef QUESTIONBANK_H
#define QUESTIONBANK_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <QString>
#include <QFile>
#include <QTextStream>
using namespace std;



class QuestionBank{

private:

    QString fileName;
    QString rawFileString;
    QVector<QString> question;
    QVector<QVector<QString>> choices;
    QVector<QVector<int>> solutions;
    QVector<QVector<int>> userAnswers;
    QVector<bool> answersLocked;
    QVector<int> displayOrder;
    int currentQuestion;
    bool initialized;


public:
    QuestionBank(QString pathToFile);
    void init();
    void initSubset(QVector<int> subset);
    int nextQuestion();
    int prevQuestion();
    QString getCurrentQuestion();
    QVector<QString> getCurrentAnswers();
    QVector<int> getCurrentSolutions();
    QVector<int> getUserSolutions();
    bool isAnswerLocked();
    bool isInitialized();
    void saveAnswer(QVector<int> userAnswer);
    bool submit(QVector<int> userAnswer);
    void submitAll();
    bool isCorrect(int i);
    int getCorrectCount();
    int getAttemptedCount();
    int getQuestionCount();
    QString getFileName();
    int getCurrentIndex();
    QVector<int> getIncorrect();
    void resetAll();


};


#endif // QUESTIONBANK_H


