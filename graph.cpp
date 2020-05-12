#include <QQueue>
#include <QDebug>
#include "graph.h"

Graph::Graph()
{

}

void Graph::pushBackNode(Node n)
{
    this->nodes.push_back(n);
}

void Graph::pushBackEdge(Edge e)
{
    this->edges.push_back(e);
}

void Graph::buildAdjList()
{
    adjList.resize(nodes.size());
    for (auto &edge : edges)
    {
        adjList[static_cast<int>(edge.source)].push_back(edge.destination);
    }
}

QVector<QString> Graph::find(QString str)
{
    QVector<QString> found;
    QVector<bool> mark(adjList.size(), false);
    QQueue<unsigned> q;

    q.push_back(0);
    mark[0] = true;

    while (!q.empty())
    {
        auto f = q.front();
        q.pop_front();
        auto node = nodes.at(static_cast<int>(f));

        if (node.page.toStdString().find(str.toStdString()) != std::string::npos)
            found.push_back(node.url);

        for (auto it = adjList.at(static_cast<int>(f)).begin(); it != adjList.at(static_cast<int>(f)).end(); ++it)
        {
            if (!mark.at(static_cast<int>(*it)))
            {
                q.push_back(*it);
                mark[static_cast<int>(*it)] = true;
            }
        }
      }
    return found;
}
