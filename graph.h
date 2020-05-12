#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QVector>
#include <QString>
#include <exception>

using AdjacencyList = QVector<QVector<unsigned>>;

struct Node
{
    Node(){}
    Node(unsigned id, QString u, QString p) : id(id), url(u) ,page(p) {}

    unsigned id;
    QString url;
    QString page;
};

struct Edge
{
    Edge(){}
    Edge(unsigned src, unsigned dst) : source(src), destination(dst) {}

    unsigned source;
    unsigned destination;
};

class Graph
{
public:
    Graph();

    void pushBackNode(Node n);
    void pushBackEdge(Edge e);
    void buildAdjList();
    QVector<QString> find(QString str);

    QVector<Node> nodes {0};
private:
    //unsigned numOfNodes {0};
    QVector<Edge> edges {0};
    AdjacencyList adjList {0};
};

#endif // GRAPH_H
