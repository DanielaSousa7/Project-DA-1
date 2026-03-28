/**
 * @file EdmondsKarp.h
 * @brief Implementation of the Edmonds-Karp maximum flow algorithm.
 */

#ifndef PROJECT_DA_1_EDMONDSKARP_H
#define PROJECT_DA_1_EDMONDSKARP_H

#include "../data_structures/Graph.h"

/**
 * @brief Visits a vertex if it has not been visited and there is residual capacity.
 *
 * @param q BFS queue.
 * @param e Edge used to reach the vertex.
 * @param w Vertex to visit.
 * @param residual Residual capacity.
 */
template <class T>
void testAndVisit(std::queue< Vertex<T>*> &q, Edge<T> *e, Vertex<T> *w, double residual) {
    if (! w->isVisited() && residual > 0) {
        w->setVisited(true);
        w->setPath(e);
        q.push(w);
    }
}

/**
 * @brief Finds an augmenting path using BFS.
 *
 * Time complexity: O(V + E), where V is the number of vertices
 * and E is the number of edges.
 *
 * @param g Graph.
 * @param s Source vertex.
 * @param t Target vertex.
 * @return True if an augmenting path exists, false otherwise.
 */
template <class T>
bool findAugmentingPath(Graph<T> *g, Vertex<T> *s, Vertex<T> *t) {
    for(auto v : g->getVertexSet()) {
        v->setVisited(false);
    }
    s->setVisited(true);
    std::queue< Vertex<T>* > q;
    q.push(s);
    while (!q.empty() && !t->isVisited()) {
        auto v = q.front();
        q.pop();
        for (auto e: v->getAdj()) {
            testAndVisit(q, e, e->getDest(), e->getWeight()-e->getFlow());
        }
        for (auto e: v->getIncoming()) {
            testAndVisit(q, e, e->getOrig(), e->getFlow());
        }
    }
    return t->isVisited();
}

/**
 * @brief Finds the minimum residual capacity along an augmenting path.
 *
 * Time complexity: O(V), where V is the number of vertices.
 *
 * @param s Source vertex.
 * @param t Target vertex.
 * @return Minimum residual capacity.
 */
template <class T>
double findMinResidualAlongPath(Vertex<T> *s, Vertex<T> *t) {
    double f = INF;
    for (auto v= t; v!=s; ) {
        auto e = v->getPath();
        if (e->getDest() == v) {
            f = std::min(f, e->getWeight()-e->getFlow());
            v = e->getOrig();
        }
        else {
            f = std::min(f, e->getFlow());
            v = e->getDest();
        }
    }
    return f;
}

/**
 * @brief Augments the flow along an augmenting path.
 *
 * Time complexity: O(V), where V is the number of vertices.
 *
 * @param s Source vertex.
 * @param t Target vertex.
 * @param f Flow to add.
 */
template <class T>
void augmentFlowAlongPath(Vertex<T> *s, Vertex<T> *t, double f) {
    for (auto v = t; v!=s; ) {
        auto e = v->getPath();
        double flow = e->getFlow();
        if (e->getDest() == v) {
            e->setFlow(flow+f);
            v = e->getOrig();
        }
        else {
            e->setFlow(flow-f);
            v = e->getDest();
        }
    }

}

/**
 * @brief Runs the Edmonds-Karp maximum flow algorithm.
 *
 * Time complexity: O(V * E^2), where V is the number of vertices
 * and E is the number of edges.
 *
 * @param g Graph.
 * @param source Source vertex id.
 * @param target Target vertex id.
 */
template <class T>
void edmondsKarp(Graph<T> *g, int source, int target) {
    Vertex<T>* s = g->findVertex(source);
    Vertex<T>* t = g->findVertex(target);
    for (auto v: g->getVertexSet()) {
        for (auto e: v->getAdj()) {
            e->setFlow(0);
        }
    }
    while (findAugmentingPath(g, s, t)) {
        double f = findMinResidualAlongPath(s, t);
        augmentFlowAlongPath(s, t, f);
    }
}

#endif //PROJECT_DA_1_EDMONDSKARP_H