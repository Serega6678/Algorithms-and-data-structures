#include <exception>
#include <vector>
#include <list>
#include <cstdio>
#include <iostream>
#include <queue>
#include <cmath>

typedef long long TFlow;
typedef long long TVertex;

#ifndef _NETWORK_
#define _NETWORK_

namespace NFlow {
    namespace NInner {

        const int INF = 1e9;

        struct Edge {
            TVertex start, finish;
            TFlow cap, flow;

            Edge (TVertex start, TVertex finish, TFlow cap, TFlow flow): start(start), finish(finish), cap(cap), flow(flow) {}
        };

        struct SourceIsEqualToSinkException : public std::exception {};

        struct TooBigSourceOrSinkException : public std::exception {};

        struct NegativeSourceOrSinkException : public std::exception {};

        struct NegativeCapacityException : public std::exception {};

        struct InvalidIteratorAction : public std::exception {};

        struct InvalidVertex : public std::exception {};

        class Network {
        private:
            TVertex vertexNumber_;
            TVertex source_, sink_;

            std::vector<Edge> edges_;
            std::vector<int> ptr_;
            std::vector<int> lasts_;

            void addEdgeLocal (TVertex start, TVertex finish, TFlow cap, TFlow flow) {
                edges_.push_back(Edge(start, finish, cap, flow));
                if (lasts_[start] == -1) {
                    lasts_[start] = edges_.size() - 1;
                    ptr_.push_back(-1);
                } else {
                    int prevEdgeIndex = lasts_[start];
                    lasts_[start] = edges_.size() - 1;
                    ptr_.push_back(prevEdgeIndex);
                }
            }

        public:
            Network (TVertex vertexNumber, TVertex source, TVertex sink): vertexNumber_(vertexNumber), source_(source), sink_(sink), lasts_(vertexNumber, -1) {
                if (source == sink) {
                    throw SourceIsEqualToSinkException();
                }
                if (source >= vertexNumber || sink >= vertexNumber) {
                    throw TooBigSourceOrSinkException();
                }
                if (source < static_cast<TVertex>(0) || sink < 0) {
                    throw NegativeSourceOrSinkException();
                }
            }

            void addOrEdge (TVertex start, TVertex finish, TFlow cap) {
                if (cap < static_cast<TFlow>(0)) {
                    throw NegativeCapacityException();
                }
                addEdgeLocal (start, finish, cap, static_cast<TFlow>(0));
                addEdgeLocal (finish, start, static_cast<TFlow>(0), static_cast<TFlow>(0));
            }

            void addEdge(TVertex start, TVertex finish, TVertex cap) {
                if (cap < static_cast<TFlow>(0)) {
                    throw NegativeCapacityException();
                }
                addEdgeLocal(start, finish, cap, static_cast<TFlow>(0));
                addEdgeLocal(finish, start, cap, static_cast<TFlow>(0));
            }

            TVertex getVertexNumber () const {
                return vertexNumber_;
            }

            TVertex getSource () const {
                return source_;
            }

            TVertex getSink () const {
                return sink_;
            }

            friend class EdgeIterator;

            class EdgeIterator {
            private:
                Network& network_;
                int edgeIndex_;

                EdgeIterator (int edgeIndex, Network& network): network_(network), edgeIndex_(edgeIndex) {}
            public:
                EdgeIterator& operator = (const EdgeIterator& it) {
                    network_ = it.network_;
                    edgeIndex_ = it.edgeIndex_;
                    return *this;
                }

                friend class Network;

                bool isValid () const {
                    return edgeIndex_ != -1;
                }

                EdgeIterator next () {
                    if (!isValid()) {
                        throw InvalidIteratorAction();
                    }
                    edgeIndex_ = network_.ptr_[edgeIndex_];
                    return *this;
                }

                TVertex getStart () const {
                    return network_.edges_[edgeIndex_].start;
                }

                TVertex getFinish () const {
                    return network_.edges_[edgeIndex_].finish;
                }

                TFlow getCapacity () const {
                    return network_.edges_[edgeIndex_].cap;
                }

                TFlow getFlow () const {
                    return network_.edges_[edgeIndex_].flow;
                }

                TFlow getResidualCapacity () const {
                    return getCapacity() - getFlow();
                }

                void changeFlow (TFlow delta) {
                    network_.edges_[edgeIndex_].flow += delta;
                }

                void changeReversedFlow (TFlow delta) {
                    network_.edges_[edgeIndex_ ^ 1].flow += delta;
                }

                TFlow getReversedResCap () const {
                    return network_.edges_[edgeIndex_ ^ 1].cap - network_.edges_[edgeIndex_ ^ 1].flow;
                }
            };

            EdgeIterator getEdgeListBegin (TVertex vertexIdx) {
                if (vertexIdx < static_cast<TVertex>(0) || vertexIdx >= vertexNumber_) {
                    throw InvalidVertex();
                }
                return EdgeIterator(lasts_[vertexIdx], *this);
            }

            TFlow getFlow () {
                TFlow curFlow = static_cast<TFlow>(0);
                for (Network::EdgeIterator it = getEdgeListBegin(source_); it.isValid(); it.next()) {
                    curFlow += it.getFlow();
                }
                return curFlow;
            }
        };

        class Algorithm {
        public:
            virtual TFlow getMaxFlow () {}
        };

        class RelabelToFront: public Algorithm {
        private:
            Network& network_;
            TVertex vertexNumber_;
            std::vector<TFlow> overcrowding_;
            std::vector<int> h_;
            std::vector<Network::EdgeIterator> ptr_;

            TFlow minTFlow (const TFlow& a, const TFlow& b) const {
                return (a < b ? a : b);
            }

            void push (Network::EdgeIterator edge) {
                TFlow flow = minTFlow(edge.getResidualCapacity(), static_cast<TFlow>(overcrowding_[edge.getStart()]));
                edge.changeFlow(flow);
                edge.changeReversedFlow(-flow);
                overcrowding_[edge.getStart()] -= flow;
                overcrowding_[edge.getFinish()] += flow;
            }

            void relabel (TVertex curVertex) {
                int minHeight = INF;
                for (Network::EdgeIterator edge = network_.getEdgeListBegin(curVertex); edge.isValid(); edge.next()) {
                    if (edge.getResidualCapacity() > static_cast<TFlow>(0) && minHeight > h_[edge.getFinish()]) {
                        minHeight = h_[edge.getFinish()];
                    }
                }
                h_[curVertex] = minHeight + 1;
            }

            void discharge (TVertex curVertex) {
                while (overcrowding_[curVertex] > static_cast<TFlow>(0)) {
                    if (!ptr_[curVertex].isValid()) {
                        relabel(curVertex);
                        ptr_[curVertex] = network_.getEdgeListBegin(curVertex);
                    } else {
                        if (ptr_[curVertex].getResidualCapacity() > static_cast<TFlow>(0) && h_[curVertex] == h_[ptr_[curVertex].getFinish()] + 1) {
                            push(ptr_[curVertex]);
                        } else {
                            ptr_[curVertex].next();
                        }
                    }
                }
            }

        public:
            RelabelToFront (Network& network): network_(network), vertexNumber_(network.getVertexNumber()),
                                               overcrowding_(vertexNumber_, static_cast<TFlow>(0)), h_(vertexNumber_, 0) {
                h_[network.getSource()] = vertexNumber_;

                TVertex source = network_.getSource();

                for (TVertex vertexIndex = static_cast<TFlow>(0); vertexIndex < vertexNumber_; ++vertexIndex) {
                    ptr_.push_back(network_.getEdgeListBegin(vertexIndex));
                }

                for (Network::EdgeIterator edge = network.getEdgeListBegin(source); edge.isValid(); edge.next()) {
                    TFlow flow = edge.getCapacity();
                    overcrowding_[edge.getStart()] -= flow;
                    overcrowding_[edge.getFinish()] += flow;
                    edge.changeFlow(flow);
                    edge.changeReversedFlow(-flow);
                }
            }

            TFlow getMaxFlow () {
                std::list<TVertex> vertexList;
                for (TVertex curVertex = static_cast<TVertex>(0); curVertex < vertexNumber_; ++curVertex) {
                    if (curVertex != network_.getSource() && curVertex != network_.getSink()) {
                        vertexList.push_back(curVertex);
                    }
                }
                auto listIterator = vertexList.begin();
                while (listIterator != vertexList.end()) {
                    TVertex curVertex = *listIterator;
                    int oldHeight = h_[curVertex];
                    discharge(curVertex);
                    if (h_[curVertex] > oldHeight) {
                        vertexList.erase(listIterator);
                        vertexList.push_front(curVertex);
                        listIterator = vertexList.begin();
                    }
                    listIterator++;
                }
                return network_.getFlow();
            }
        };

        class MalCumMah: public Algorithm {
        private:
            Network& network_;
            TVertex vertexNumber_;
            TVertex source_, sink_;
            std::vector<TFlow> inPotential_;
            std::vector<TFlow> outPotential_;
            std::vector<int> distance_;
            std::vector<Network::EdgeIterator> ptr_;
            std::vector<Network::EdgeIterator> ptrStraight_;
            std::vector<Network::EdgeIterator> ptrReversed_;


            TFlow minTFlow (TFlow a, TFlow b) const {
                return (a < b ? a : b);
            }

            void changePotentials (TVertex start, TVertex finish, TFlow change) {
                inPotential_[start] -= change;
                outPotential_[finish] -= change;
            }

            TFlow getVertexPotential (TVertex curVertex) const {
                if (curVertex == source_) {
                    return outPotential_[source_];
                }
                if (curVertex == sink_) {
                    return inPotential_[sink_];
                }
                return minTFlow(inPotential_[curVertex], outPotential_[curVertex]);
            }

            bool bfs () {
                for (int i = 0; i < distance_.size(); ++i) {
                    distance_[i] = INF;
                }
                distance_[source_] = 0;
                std::queue<TVertex> vertexIndexQ;
                vertexIndexQ.push(source_);
                while (!vertexIndexQ.empty()) {
                    TVertex curVertex = vertexIndexQ.front();
                    vertexIndexQ.pop();
                    for (Network::EdgeIterator edge = ptr_[curVertex]; edge.isValid(); edge.next()) {
                        if (edge.getResidualCapacity() > static_cast<TFlow>(0) && distance_[edge.getFinish()] == INF) {
                            distance_[edge.getFinish()] = distance_[curVertex] + 1;
                            vertexIndexQ.push(edge.getFinish());
                        }
                    }
                }
                return distance_[sink_] != INF;
            }

            void makeFlowChangingIteration (TVertex referenceVertex, TFlow potential, TVertex endVertex, std::vector<Network::EdgeIterator>& edgePtr_, bool straight) {
                std::queue<TVertex> vertexIndexQ;
                vertexIndexQ.push(referenceVertex);
                std::vector<TFlow> flowChange(vertexNumber_, static_cast<TFlow>(0));
                flowChange[referenceVertex] = potential;
                while (!vertexIndexQ.empty()) {
                    TVertex curVertex = vertexIndexQ.front();
                    vertexIndexQ.pop();
                    for (; edgePtr_[curVertex].isValid(); edgePtr_[curVertex].next()) {
                        Network::EdgeIterator edge = edgePtr_[curVertex];
                        if (distance_[curVertex] != distance_[edge.getFinish()] + 1 && !straight) {
                            continue;
                        }
                        if (distance_[edge.getFinish()] != distance_[curVertex] + 1 && straight) {
                            continue;
                        }
                        if (flowChange[curVertex] == static_cast<TFlow>(0)) {
                            break;
                        }
                        TFlow canChange;
                        if (straight) {
                            canChange = minTFlow(edge.getResidualCapacity(), flowChange[curVertex]);
                        } else {
                            canChange = minTFlow(edge.getReversedResCap(), flowChange[curVertex]);
                        }
                        if (canChange == 0) {
                            continue;
                        }
                        if (flowChange[edge.getFinish()] == static_cast<TFlow>(0) && edge.getFinish() != endVertex) {
                           vertexIndexQ.push(edge.getFinish());
                        }
                        if (straight) {
                            edge.changeFlow(canChange);
                            edge.changeReversedFlow(-canChange);
                            flowChange[curVertex] -= canChange;
                            flowChange[edge.getFinish()] += canChange;
                            changePotentials(edge.getFinish(), curVertex, canChange);

                        } else {
                            edge.changeFlow(-canChange);
                            edge.changeReversedFlow(canChange);
                            flowChange[curVertex] -= canChange;
                            flowChange[edge.getFinish()] += canChange;
                            changePotentials(curVertex, edge.getFinish(), canChange);
                        }
                        if (flowChange[curVertex] == static_cast<TFlow>(0)) {
                            break;
                        }
                    }
                }
            }

            void pullFromSource (TVertex referenceVertex, TFlow potential) {
                makeFlowChangingIteration(referenceVertex, potential, source_, ptrReversed_, false);
            }

            void pushTowardsSink (TVertex referenceVertex, TFlow potential) {
                makeFlowChangingIteration(referenceVertex, potential, sink_, ptrStraight_, true);
            }

            TVertex referenceNode () const {
                TFlow minPotential = INF;
                TVertex minPotentialVertex = INF;

                for (TVertex curVertex = static_cast<TFlow>(0); curVertex < vertexNumber_; ++curVertex) {
                    if (distance_[curVertex] == INF) {
                        continue;
                    }
                    if (getVertexPotential(curVertex) < minPotential) {
                        minPotential = getVertexPotential(curVertex);
                        minPotentialVertex = curVertex;
                    }
                }
                return minPotentialVertex;
            }

            TFlow blockingFlow () {
                while (true) {
                    TVertex minPotentialVertex = referenceNode();
                    if (minPotentialVertex == static_cast<TVertex>(INF)) {
                        return static_cast<TFlow>(0);
                    }
                    if (getVertexPotential(minPotentialVertex) == static_cast<TFlow>(0)) {
                        for (Network::EdgeIterator edge = ptr_[minPotentialVertex]; edge.isValid(); edge.next()) {
                            if (distance_[edge.getFinish()] == distance_[minPotentialVertex] + 1) {
                                changePotentials(edge.getFinish(), minPotentialVertex, edge.getResidualCapacity());
                            }
                            if (distance_[edge.getFinish()] + 1 == distance_[minPotentialVertex]) {
                                changePotentials(minPotentialVertex, edge.getFinish(), edge.getReversedResCap());
                            }
                        }
                        distance_[minPotentialVertex] = INF;
                        continue;
                    }
                    TFlow change = getVertexPotential(minPotentialVertex);
                    pushTowardsSink(minPotentialVertex, change);
                    pullFromSource(minPotentialVertex, change);
                    return change;
                }
            }

            void potentialInit() {
                for (int i = 0; i < vertexNumber_; ++i) {
                    inPotential_[i] = static_cast<TFlow>(0);
                    outPotential_[i] = static_cast<TFlow>(0);
                }
                for (TVertex curVertex = static_cast<TVertex>(0); curVertex < vertexNumber_; ++curVertex) {
                    for (Network::EdgeIterator edge = network_.getEdgeListBegin(curVertex); edge.isValid(); edge.next()) {
                        if (distance_[edge.getFinish()] == distance_[curVertex] + 1) {
                            changePotentials(edge.getFinish(), curVertex, -edge.getResidualCapacity());
                        }
                    }
                }

            }

        public:
            MalCumMah(Network& network): network_(network), vertexNumber_(network.getVertexNumber()), source_(network_.getSource()), sink_(network_.getSink()),
                                         inPotential_(vertexNumber_, static_cast<TFlow>(0)), outPotential_(vertexNumber_, static_cast<TFlow>(0)),
                                         distance_(vertexNumber_, 0) {
                for (TVertex curVertex = static_cast<TVertex>(0); curVertex < vertexNumber_; ++curVertex) {
                    ptr_.push_back(network.getEdgeListBegin(curVertex));
                    ptrStraight_.push_back(ptr_[curVertex]);
                    ptrReversed_.push_back(ptr_[curVertex]);
                }
            }

            TFlow getMaxFlow () {
                TFlow flow = static_cast<TFlow>(0);
                while(bfs()) {
                    ptrStraight_ = ptr_;
                    ptrReversed_ = ptr_;
                    potentialInit();
                    TFlow deltaFlow = blockingFlow();
                    while (deltaFlow > static_cast<TFlow>(0)) {
                        flow += deltaFlow;
                        deltaFlow = blockingFlow();
                    }
                }
                return flow;
            }
        };
    }
}
#endif
