#include "webparser.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QMessageBox>
#include <QEventLoop>
#include <QRegExp>
#include <QDebug>
#include <QRegularExpressionMatchIterator>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <algorithm>
#include <thread>
#include <vector>

QVector<QString> WebParser::StartParser()
{
    graph = buildPageGraph();
    return graph->find(searchPhrase);
}

Graph* WebParser::buildPageGraph()
{
    Graph *graph = new Graph();
    unsigned currentNode {0};
    unsigned parentNode = currentNode;
    auto startPage = uploadPage(startURL);
    graph->pushBackNode(Node(currentNode, startURL, startPage));
    auto urls = findURLs(startPage);

    do {
        if(stopSignal)
        {
            graph->buildAdjList();
            changeProgressBarValue(100);
            return graph;
        }
        if(endUpload) break;
        //auto numOfURLs = urls.size();
        auto pages = loadPagePool(urls);
        //counter -= numOfURLs -
        urls.clear();
        for(auto& page : pages.keys())
            urls.push_back(page);
        for(auto& url : urls)
        {
            if(url != DELIMITER)
            {
                graph->pushBackNode(Node(++currentNode, url, pages.value(url)));
                graph->pushBackEdge(Edge(parentNode, currentNode));
            }
            else
                ++parentNode;
        }
        urls.clear();
        for(auto& page : pages)
        {
            urls += findURLs(page);
            if(pages.size() > 1)
                urls.push_back(DELIMITER);
        }
        ++parentNode;
    } while (!endUpload);
    graph->buildAdjList();
    changeProgressBarValue(++progress);
    return graph;
}

QVector<QString> WebParser::removeExists(QVector<QString> urls, QSet<QString>& existsUrls)
{
    if(!urls.empty())
    {
        for(auto& url : urls)
        {
            if(existsUrls.find(url) != existsUrls.end())
                urls.erase(std::remove(urls.begin(), urls.end(), url), urls.end());
            existsUrls.insert(url);
        }
    }
    return urls;
}

QString WebParser::uploadPage(QString url) const
{
    QNetworkAccessManager manager;
    QNetworkRequest req;
    req.setUrl(QUrl(url));
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    QNetworkReply *response = manager.get(req);
    QEventLoop event;
    QObject::connect(response, &QNetworkReply::finished, &event, &QEventLoop::quit);
    event.exec();
    QString page = response->readAll();

    return page;
}

QVector<QString> WebParser::findURLs(QString page)
{
    QVector<QString> result {};
    if(uploadedNow >= uploadLimit)
    {
        endUpload = true;
        return result;
    }
    const QRegularExpression r_url(R"(https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*))");
    QVector<QString> urls;

    QRegularExpressionMatchIterator i = r_url.globalMatch(page);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        if (match.hasMatch()) {
            urls.push_back(match.captured(0));
        }
    }
    std::sort(urls.begin(), urls.end());
    urls.erase(std::unique(urls.begin(), urls.end()), urls.end());
    result = removeExists(urls, existsUrls);
    uploadedNow += result.size();

    return result;
}

QMap<QString, QString> WebParser::loadPagePool(QVector<QString> urls)
{
    std::mutex pagesMutex;
    unsigned counter = urls.size();
    QMap<QString, QString> pages;
    auto unit = uploadLimit / 100;

    if (counter > numOfThreads)
    {
        std::vector<std::thread> threads;
        unsigned steps = counter / numOfThreads;
        unsigned lastStepSize{0};
        if(counter % numOfThreads != 0)
        {
            lastStepSize = counter - numOfThreads * steps;
            ++steps;
        }
        QAtomicInt index = 0;

        for(unsigned i = 0; i < steps; ++i)
        {
            threads.clear();
            auto jobs = numOfThreads;
            if(i == steps - 1 && lastStepSize != 0)
                jobs = lastStepSize;
            for(unsigned j = 0; j < jobs; j++)
            {
                changeProgressBarValue(++progress);
                        threads.push_back(std::thread([&] {
                          const auto url = urls.at(index++);
                          pages.insert(url, uploadPage(url));
                          }));
            }
            for (auto& thread : threads)
            {
                thread.join();
            }
        }
    }
    else
    {
        for(auto& url : urls)
        {
            changeProgressBarValue(++progress);
            pages.insert(url, uploadPage(url));
        }
    }
    return pages;
}
