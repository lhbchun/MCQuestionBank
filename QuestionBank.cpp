#include "QuestionBank.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <QString>
#include <QDebug>
#include <algorithm>
using namespace std;

QVector<QString> separateString(const QString &x, const QString &sep) {
    QVector<QString> result;
    int startPos = 0;
    int sepPos = x.indexOf(sep);

    while (sepPos != -1) {
        QString part = x.mid(startPos, sepPos - startPos);
        result.append(part);
        startPos = sepPos + sep.length();
        sepPos = x.indexOf(sep, startPos);
    }

    QString lastPart = x.mid(startPos);
    result.append(lastPart);

    return result;
}

QVector<int> separateInt(const QString &x, const QString &sep) {
    QVector<int> result;
    int startPos = 0;
    int sepPos = x.indexOf(sep);

    while (sepPos != -1) {
        QString part = x.mid(startPos, sepPos - startPos);
        bool ok;
        int intValue = part.toInt(&ok);
        if (ok) {
            result.append(intValue);
        }
        startPos = sepPos + sep.length();
        sepPos = x.indexOf(sep, startPos);
    }

    QString lastPart = x.mid(startPos);
    bool ok;
    int lastIntValue = lastPart.toInt(&ok);
    if (ok) {
        result.append(lastIntValue);
    }

    return result;
}


QVector<int> randomOrder(int n) {
    QVector<int> result;

    // Fill the QVector with numbers from 1 to n in ascending order
    for (int i = 0; i < n; ++i) {
        result.append(i);
    }

    // Shuffle the QVector to randomize the order
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(result.begin(), result.end(), g);

    return result;
}


QVector<int> randomOrder(QVector<int> result) {

    // Shuffle the QVector to randomize the order
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(result.begin(), result.end(), g);

    return result;
}


QString read(const QString &dir) {
    QFile file(dir);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString fileContents = in.readAll();
        file.close();
        return fileContents;
    } else {
        // Handle the case where the file cannot be opened
        // qDebug() << "Error: Unable to open file " << dir;
        return QString(); // Return an empty string to indicate an error
    }
}



QuestionBank::QuestionBank(QString pathToFile){
    initialized = false;
    if (pathToFile.isEmpty()){
        return;
    }
    fileName = pathToFile;
    rawFileString = read(pathToFile);
    initialized = true;
    currentQuestion = 0;
    init();

}

void QuestionBank::init(){
    // split the raw string
    QVector<QString> QAs = separateString(rawFileString, "<ENDOFSOLUTION>");
    for (int i = 0; i < QAs.size(); i++){
        QString QA = QAs[i];
        if (QA.length() == 0) {continue;}
        QVector<QString> questionAnswerSep = separateString(QA, "<ENDOFQUESTION>");
        question.push_back(questionAnswerSep[0]);

        QVector<QString> answerSolutionSep = separateString(questionAnswerSep[1], "<ENDOFANSWER>");
        QVector<QString> choice = separateString(answerSolutionSep[0], "<SEP>");
        QVector<QString> QStringAnswer = QVector<QString>();
        for (int j = 0 ; j < choice.size(); j++){
            QStringAnswer.push_back(choice[j]);
        }
        choices.push_back(QStringAnswer);

        QVector<int> solution = separateInt(answerSolutionSep[1], "<SEP>");
        for (int j = 0; j<solution.size(); j++){
            solution[j]--;
        }
        sort(solution.begin(),solution.end());

        solutions.push_back(solution);
    }

    answersLocked = QVector<bool>(question.size(), false);
    userAnswers = QVector<QVector<int>>(choices.size(), QVector<int>());
    displayOrder = randomOrder(question.size());

}


void QuestionBank::initSubset(QVector<int> subset){
    displayOrder = randomOrder(subset);

}

int QuestionBank::nextQuestion(){
    if (currentQuestion == displayOrder.size() - 1){
        return currentQuestion;
    }
    return ++currentQuestion;
}

int QuestionBank::prevQuestion(){
    if (currentQuestion == 0){
        return 0;
    }
    return --currentQuestion;
}

QString QuestionBank::getCurrentQuestion(){
    return question[displayOrder[currentQuestion]];
}

QVector<QString> QuestionBank::getCurrentAnswers(){
    return choices[displayOrder[currentQuestion]];
}

QVector<int> QuestionBank::getCurrentSolutions(){
    return solutions[displayOrder[currentQuestion]];
}

void QuestionBank::saveAnswer(QVector<int> userAnswer){
    if (isAnswerLocked()){return;}
    sort(userAnswer.begin(),userAnswer.end());
    userAnswers[displayOrder[currentQuestion]] = userAnswer;
}

bool QuestionBank::submit(QVector<int> userAnswer){
    answersLocked[displayOrder[currentQuestion]] = true;
    sort(userAnswer.begin(),userAnswer.end());
    userAnswers[displayOrder[currentQuestion]] = userAnswer;
    if (userAnswer.size() != solutions[displayOrder[currentQuestion]].size()){
        return false;
    }

    for (int i = 0; i < solutions[displayOrder[currentQuestion]].size(); i++){
        if (solutions[displayOrder[currentQuestion]][i] != userAnswer[i]){
            return false;
        }
    }

    return true;
}


QVector<int> QuestionBank::getUserSolutions(){
    return userAnswers[displayOrder[currentQuestion]];
}
bool QuestionBank::isAnswerLocked(){
    return answersLocked[displayOrder[currentQuestion]];
}


bool QuestionBank::isCorrect(int i){

    if (userAnswers[i].size() != solutions[i].size()){
        return false;
    }

    for (int j = 0; j < solutions[i].size(); j++){
        if (solutions[i][j] != userAnswers[i][j]){
            return false;
        }
    }

    return true;
}

int QuestionBank::getCorrectCount(){
    int count = 0;

    for (int i = 0; i < question.size(); i++){
        if (answersLocked[i]){
            if (isCorrect(i)){
                count ++;
            }
        }
    }

    return count;
}
int QuestionBank::getAttemptedCount(){
    int count = 0;

    for (int i = 0; i < question.size(); i++){
        if (answersLocked[i]){
            count++;
        }
    }

    return count;
}

QString QuestionBank::getFileName(){
    return fileName;
}

void QuestionBank::submitAll(){
    for (int i = 0; i < question.size(); i++){
        if (!answersLocked[i] and userAnswers[i].size() > 0){
            answersLocked[i] = true;
        }
    }
}

int QuestionBank::getCurrentIndex(){
    return currentQuestion;
}

int QuestionBank::getQuestionCount(){
    return displayOrder.size();
}

QVector<int> QuestionBank::getIncorrect(){
    QVector<int> result = QVector<int>();
    for (int i = 0; i < question.size(); i++){
        if (answersLocked[i] and !isCorrect(i)){
            result.push_back(i);
        }
    }
    return result;
}


void QuestionBank::resetAll(){
    answersLocked = QVector<bool>(question.size(), false);
    userAnswers = QVector<QVector<int>>(choices.size(), QVector<int>());
    initSubset(displayOrder);
    currentQuestion = 0;
}
