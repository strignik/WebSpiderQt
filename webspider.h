#ifndef WEBSPIDER_H
#define WEBSPIDER_H

#include <QMainWindow>
#include <QLineEdit>
#include <QObject>
#include "webparser.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WebSpider; }
QT_END_NAMESPACE

class WebSpider : public QMainWindow
{
    Q_OBJECT

public:
    WebSpider(QWidget *parent = nullptr);
    void Init();
    ~WebSpider();

signals:
    void stopButtonClicked();

private slots:
    void on_buttonStart_clicked();
    void progressChanged(int value);

    void on_buttonStop_clicked();

private:
    void printGraph(Graph const & graph, int N);
    Ui::WebSpider *ui;
};
#endif // WEBSPIDER_H
