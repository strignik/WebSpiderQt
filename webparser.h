#ifndef WEBPARSER_H
#define WEBPARSER_H

#include <QThread>
#include <QMap>
#include <QVector>
#include <QString>
#include <QSet>
#include <QApplication>
#include <mutex>
#include <set>
#include "graph.h"

const QString DELIMITER = "|";

class WebParser : public QObject
{
    Q_OBJECT
public:
    WebParser(QString url, QString phrase, unsigned threads, unsigned limit)
        : startURL(url), searchPhrase(phrase), numOfThreads(threads), uploadLimit(limit) {}
    QVector<QString> StartParser();

private:
    Graph* buildPageGraph();
    QString uploadPage(const QString url) const;
    QVector<QString> findURLs(QString page);
    QVector<QString> removeExists(QVector<QString> urls, QSet<QString>& existsUrls);
    QMap<QString, QString> loadPagePool(QVector<QString> urls);

    unsigned uploadedNow = 1;
    Graph* graph = nullptr;
    bool stopSignal = false;
    bool endUpload = false;
    QAtomicInt progress = 0;
    QSet<QString> existsUrls;
    const QString startURL;
    const QString searchPhrase;
    const unsigned numOfThreads {0};
    const unsigned uploadLimit {0};

signals:
    void progerssChangeValue(int value);

public slots:
    void changeProgressBarValue(int value) { emit progerssChangeValue(value); }
    void stopProgramm(){ stopSignal = true; }


};

#endif // WEBPARSER_H
