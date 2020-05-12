#include <QDebug>
#include <QFile>
#include <QRegExpValidator>
#include "webspider.h"
#include "ui_webspider.h"
#include "webparser.h"

QString printURL(QString url)
{
    auto str = "--> <a href=\"";
    auto str1 = "\" style=\"color: rgb(32,194,14);\">";
    auto str2 = "<\a>";
    return str+url+str1+url+str2;
}

WebSpider::WebSpider(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WebSpider)
{
    ui->setupUi(this);
    ui->textBrowser->setOpenExternalLinks(true);
}

void WebSpider::Init()
{
    QString url = ui->lineUrl->text();
    QString phrase = ui->lineSearch->text();
    unsigned threads = (ui->spinBoxThreads->text()).toInt();
    unsigned limit = (ui->lineLimit->text()).toInt();

    WebParser parser(url, phrase, threads, limit);
    QObject::connect(&parser, &WebParser::progerssChangeValue, this, &WebSpider::progressChanged);
    QObject::connect(this, &WebSpider::stopButtonClicked, &parser, &WebParser::stopProgramm);
    auto res = parser.StartParser();
    for(auto& url : res)
    {
        ui->textBrowser->append(printURL(url));
    }
}

WebSpider::~WebSpider()
{
    delete ui;
}

void WebSpider::on_buttonStart_clicked()
{
    Init();
}

void WebSpider::on_buttonStop_clicked()
{
    emit stopButtonClicked();
}

void WebSpider::progressChanged(int value)
{
    ui->progressBar->setValue(value);
}


