#include <iostream>
#include <vector>
#include <cstdio>
#include <string>
#include <algorithm>
#include <stack>
#include <map>
#include <set>
#include <cassert>
#include <iterator>
#include <string.h>


#ifndef __SuffStructures__
#define __SuffStructures__

namespace NStringStructures {
    namespace NSuffStructures {
        class SuffixAutomaton {
        private:
            struct Vertex;
        public:
            struct VertexMetaInfo {
                VertexMetaInfo(Vertex vert): maxLen(vert.maxLen), countTermRoads(vert.countTermRoads),
                                                                firstEncounter(vert.firstEncounter) {}
                int maxLen;
                int countTermRoads = 0;
                int firstEncounter = -1;
            };

            SuffixAutomaton() : lastVertex_(0) {
                states_.emplace_back(0);
            }

            void build(const std::string& str) {
                baseString_ = str;
                for (int i = 0; i < str.length(); ++i) {
                    addSymbol_(str[i]);
                }
            }

            bool checkString(const std::string& str) {
                int curIdx = 0;
                for (int i = 0; i < str.length(); ++i) {
                    if (states_[curIdx].nextVertex.find(str[i]) == states_[curIdx].nextVertex.end()) {
                        return false;
                    } else {
                        curIdx = states_[curIdx].nextVertex[str[i]];
                    }
                }
                return true;
            }

            void buildDistances() {
                int idx = lastVertex_;
                while (idx >= 0) {
                    states_[idx].countTermRoads = 1;
                    idx = states_[idx].suffLink;
                }
                buildVertexDistances_(0);
            }

            std::vector<VertexMetaInfo> getAutomationMetaInfo () const {
                std::vector<VertexMetaInfo> metaInfo;
                for (int i = 0; i < states_.size(); ++i) {
                    metaInfo.push_back(VertexMetaInfo(states_[i]));
                }
                return metaInfo;
            }
        private:
            int lastVertex_;
            std::string baseString_;
            std::vector<Vertex> states_;

            struct Vertex {
                explicit Vertex(int maxLen, int suffLink = -1) : suffLink(suffLink), maxLen(maxLen) {}

                int suffLink, maxLen;
                int countTermRoads = 0;
                int firstEncounter = -1;
                bool distanceBuilt = false;
                std::map<char, int> nextVertex;
            };

            void buildVertexDistances_(int curIdx) {
                for (auto it = states_[curIdx].nextVertex.begin(); it != states_[curIdx].nextVertex.end(); ++it) {
                    if (!states_[it->second].distanceBuilt) {
                        buildVertexDistances_(it->second);
                    }
                    states_[curIdx].countTermRoads += states_[it->second].countTermRoads;
                }
                states_[curIdx].distanceBuilt = true;
            }

            int getAutomationSize_() const {
                return static_cast<int>(states_.size());
            }

            void addSymbol_(char curLetter) {
                int nextVertex = getAutomationSize_();
                states_.push_back(Vertex(states_[lastVertex_].maxLen + 1));
                states_[nextVertex].firstEncounter = states_[nextVertex].maxLen - 1;
                int prevStateIdx = lastVertex_;
                while (prevStateIdx != -1 &&
                       states_[prevStateIdx].nextVertex.find(curLetter) == states_[prevStateIdx].nextVertex.end()) {
                    states_[prevStateIdx].nextVertex[curLetter] = nextVertex;
                    prevStateIdx = states_[prevStateIdx].suffLink;
                }
                if (prevStateIdx != -1) {
                    int midVertex = states_[prevStateIdx].nextVertex[curLetter];
                    if (states_[midVertex].maxLen == states_[prevStateIdx].maxLen + 1) {
                        states_[nextVertex].suffLink = midVertex;
                    } else {
                        int cloneVertex = getAutomationSize_();
                        states_.push_back(states_[midVertex]);
                        states_[cloneVertex].maxLen = states_[prevStateIdx].maxLen + 1;
                        while (prevStateIdx != -1 && states_[prevStateIdx].nextVertex[curLetter] == midVertex) {
                            states_[prevStateIdx].nextVertex[curLetter] = cloneVertex;
                            prevStateIdx = states_[prevStateIdx].suffLink;
                        }
                        states_[midVertex].suffLink = cloneVertex;
                        states_[nextVertex].suffLink = cloneVertex;
                    }
                } else {
                    states_[nextVertex].suffLink = 0;
                }
                lastVertex_ = nextVertex;
            }
        };

        class SuffixArray {
        public:
            SuffixArray(int lastPossibleLetterOrd, const std::string& str):
                    baseString_(str + "$"),
                    maxOrd_(lastPossibleLetterOrd)
            {
                SuffixArray_.resize(getStringLength_() - 1);
            }

            void build() {
                std::vector<int> stringPosition(getStringLength_());
                std::vector<int> currentIndex(getStringLength_());
                std::vector<int> stringPositionSecond(getStringLength_());
                std::vector<int> currentIndexSecond(getStringLength_());
                std::vector<int> symbolCountPosition(std::max(getStringLength_(), maxOrd_));
                int curClass = 0;

                /// first-level sort

                std::fill(symbolCountPosition.begin(), symbolCountPosition.end(), 0);

                for (int i = 0; i < getStringLength_(); ++i) {
                    symbolCountPosition[baseString_[i]] += 1;
                }
                for (int i = 1; i <= maxOrd_; ++i) {
                    symbolCountPosition[i] += symbolCountPosition[i - 1];
                }
                for (int i = 0; i < getStringLength_(); ++i) {
                    int curIdx = symbolCountPosition[baseString_[i]] - 1;
                    stringPosition[curIdx] = i;
                    symbolCountPosition[baseString_[i]] -= 1;
                }
                currentIndex[stringPosition[0]] = curClass;
                for (int i = 1; i < getStringLength_(); ++i) {
                    if (baseString_[stringPosition[i - 1]] != baseString_[stringPosition[i]]) {
                        curClass += 1;
                    }
                    currentIndex[stringPosition[i]] = curClass;
                }

                /// higher level pair sort

                for (int deg = 1; deg < getStringLength_(); deg = deg << 1) {
                    for (int i = 0; i < getStringLength_(); ++i) {
                        stringPositionSecond[i] = stringPosition[i] - deg;
                        if (stringPositionSecond[i] < 0) {
                            stringPositionSecond[i] += getStringLength_();
                        }
                    }

                    std::fill(symbolCountPosition.begin(), symbolCountPosition.end(), 0);

                    for (int i = 0; i < getStringLength_(); ++i) {
                        int curIdx = currentIndex[stringPositionSecond[i]];
                        symbolCountPosition[curIdx] += 1;
                    }
                    for (int i = 1; i <= curClass; ++i) {
                        symbolCountPosition[i] += symbolCountPosition[i - 1];
                    }
                    for (int i = getStringLength_() - 1; i >= 0; --i) {
                        symbolCountPosition[currentIndex[stringPositionSecond[i]]] -= 1;
                        int curIdx = symbolCountPosition[currentIndex[stringPositionSecond[i]]];
                        stringPosition[curIdx] = stringPositionSecond[i];
                    }
                    curClass = 0;
                    currentIndexSecond[stringPosition[0]] = curClass;
                    for (int i = 1; i < getStringLength_(); ++i) {
                        int leftFirst = (stringPosition[i - 1] + deg) % getStringLength_();
                        int rightFirst = (stringPosition[i] + deg) % getStringLength_();
                        if ((currentIndex[leftFirst] != currentIndex[rightFirst]) ||
                            (currentIndex[stringPosition[i]] != currentIndex[stringPosition[i - 1]])) {
                            curClass += 1;
                        }
                        currentIndexSecond[stringPosition[i]] = curClass;
                    }

                    currentIndex = currentIndexSecond;
                }

                for (int i = 0; i < getStringLength_() - 1; ++i) {
                    SuffixArray_[currentIndex[i] - 1] = i;
                }
                arrayBuilt_ = true;
            }

            void buildLCP() {
                SuffixArray_.insert(SuffixArray_.begin(), getStringLength_() - 1);
                LCP_.resize(getStringLength_());
                std::vector<int> revPos = std::vector<int>(getStringLength_());
                for (int i = 0; i < revPos.size(); ++i) {
                    revPos[SuffixArray_[i]] = i;
                }
                int curIdx = 0;
                for (int i = 0; i < getStringLength_(); ++i) {
                    if (curIdx > 0) {
                        curIdx--;
                    }
                    if (revPos[i] == getStringLength_() - 1) {
                        curIdx = 0;
                        LCP_[revPos[i]] = -1;
                        continue;
                    }
                    int j = SuffixArray_[revPos[i] + 1];
                    while ((curIdx + std::max(i, j) < getStringLength_()) &&
                           (baseString_[j + curIdx] == baseString_[i + curIdx])) {
                        curIdx += 1;
                    }
                    LCP_[revPos[i]] = curIdx;
                }
                LCP_.erase(LCP_.begin());
                SuffixArray_.erase(SuffixArray_.begin());
                LCP_.pop_back();
                LCPBuilt_ = true;
            }

            std::vector<int> getSuffArray() {
                if (!arrayBuilt_) {
                    build();
                }
                return SuffixArray_;
            }

            std::vector<int> getLCP() {
                if (!arrayBuilt_) {
                    build();
                }
                if (!LCPBuilt_) {
                    buildLCP();
                }
                return LCP_;
            }
        private:
            int maxOrd_;
            bool arrayBuilt_ = false;
            bool LCPBuilt_ = false;
            std::string baseString_;
            std::vector<int> SuffixArray_;
            std::vector<int> LCP_;

            int getStringLength_() const {
                return baseString_.length();
            }
        };

        class SuffixTree {
        private: 
            struct Vertex;
        public:
            struct VertexMetaInfo {
                int childCount;
                int start;
                int end;
                std::map<char, VertexMetaInfo*> children;    
                VertexMetaInfo(Vertex vert): childCount(vert.childCount), start(vert.start), end(*vert.end) {}
                friend SuffixTree;
            };
            SuffixTree(const std::string& str): baseString_(str), size(1)
            {
                buildTree();
            }
            void buildTree() {
                int length = baseString_.length();
                int* rootEnd = new int;
                *rootEnd = -1;
                root = new Vertex(-1, rootEnd, root);
                activeVertex = root;
                for (int i = 0; i < length; i++) {
                    extendTree_(i);
                }
            }
            void buildDistances() {
                buildTreeDistances_(root);
            }
            VertexMetaInfo* buildMetaInfo() const {
                VertexMetaInfo* rootMeta = new VertexMetaInfo(*root);
                buildMetaTree_(rootMeta, root);
                return rootMeta;
            }
        private:
            int size = 1;
            Vertex* root = nullptr;
            Vertex* lastNewVertex = nullptr;
            Vertex* activeVertex = nullptr;
            int activeEdge = -1;
            int activeLength = 0;
            int remainder = 0;
            int leafEnd = -1;
            int* splitEnd = nullptr;
            bool dfsFinished = false;
            std::string baseString_;
            struct Vertex {
            public:
                bool visited = false;
                int childCount = 0;
                int start;
                int *end;
                Vertex* suffLink;
                std::map<char, Vertex*> children;
                Vertex(int start, int* end, Vertex* root) :
                        start(start), end(end),
                        suffLink(root), children() {}
                int getEnd() { 
                    return (*end); 
                }
                int getStart() {
                    return start;
                };
                void addToStart(int length) {
                    start += length;
                }
                int getLength() {
                    return *(end)-(start) + 1;
                }
                bool hasNext(char c) {
                    if (children.find(c) != children.end()) {
                        return true;
                    }
                    return false;
                }
                Vertex* getNext(char c) {
                    auto it = children.find(c);
                    return it->second;
                }

                void addChild(char c, Vertex* Vertex) {
                    auto it = children.find(c);
                    if (it == children.end()) {
                        children.insert(std::make_pair(c, Vertex));
                    }
                    else {
                        it->second = Vertex;
                    }
                }

                Vertex* getSuffLink() {
                    return suffLink;
                }
                void setSuffLink(Vertex* Vertex) {
                    suffLink = Vertex;
                }
            };
            void buildTreeDistances_(Vertex* nd) {
                for (auto it = nd->children.begin(); it != nd->children.end(); ++it) {
                    if (!it->second->visited && it->first != '$') {
                        buildTreeDistances_(it->second);
                    }
                    if (it->first == '$' || it->second->getEnd() == baseString_.length() - 1) {
                        nd->childCount += 1;
                    } else {
                        nd->childCount += it->second->childCount;
                    }
                }
                nd->visited = true;
            }
            void extendTree_(int pos) {
                lastNewVertex = nullptr;
                leafEnd = pos;
                ++remainder;
                while (remainder > 0) {
                    if (activeLength == 0) {
                        activeEdge = pos;
                    }
                    if (activeVertex->hasNext(baseString_[activeEdge])) {
                        Vertex* next = activeVertex->getNext(baseString_[activeEdge]);
                        int nextLen = next->getLength();
                        if (activeLength >= nextLen) {
                            activeVertex = next;
                            activeLength -= nextLen;
                            activeEdge += nextLen;
                            continue;
                        }
                        int posIn = next->getStart() + activeLength;
                        if (baseString_[posIn] == baseString_[pos]) {
                            if (lastNewVertex != nullptr && activeVertex != root) {
                                lastNewVertex->setSuffLink(activeVertex);
                                lastNewVertex = nullptr;
                            }
                            ++activeLength;
                            break;
                        }
                        int* splitEnd = new int;
                        *splitEnd = posIn - 1;
                        ++size;
                        Vertex* newVertex = new Vertex(next->getStart(), splitEnd, root);
                        activeVertex->addChild(baseString_[next->getStart()], newVertex);
                        ++size;
                        Vertex* newLeaf = new Vertex(pos, &leafEnd, root);
                        newVertex->addChild(baseString_[pos], newLeaf);
                        next->addToStart(activeLength);
                        newVertex->children[baseString_[next->getStart()]] = next;
                        if (lastNewVertex != nullptr) {
                            lastNewVertex->setSuffLink(newVertex);
                        }
                        lastNewVertex = newVertex;
                    } else {
                        ++size;
                        Vertex* newLeaf = new Vertex(pos, &leafEnd, root);
                        activeVertex->addChild(baseString_[activeEdge], newLeaf);
                        if (lastNewVertex != nullptr) {
                            lastNewVertex->setSuffLink(activeVertex);
                            lastNewVertex = nullptr;
                        }
                    }
                    --remainder;
                    if (activeVertex == root && activeLength > 0) {
                        --activeLength;
                        activeEdge = pos - remainder + 1;
                    }
                    else if (activeVertex != root) {
                        activeVertex = activeVertex->getSuffLink();
                    }
                }
            }
            void buildMetaTree_(VertexMetaInfo* Vert, Vertex* curVertex) const {
                for (auto it = curVertex->children.begin(); it != curVertex->children.end(); ++it) {
                    if (it->second) {
                        VertexMetaInfo* nextVert = new VertexMetaInfo(*(it->second));
                        buildMetaTree_(nextVert, it->second);
                        Vert->children[it->first] = nextVert;
                    }
                }
            }
        };
    }
}

#endif

std::string readRefrenInput() {
    int n, m;
    std::cin >> n >> m;
    std::vector<int> refrenInput(n);
    for (int i = 0; i < n; ++i) {
        scanf(" %d", &refrenInput[i]);
        refrenInput[i] += '0' - 1;
    }
    std::string refrenString = std::string(refrenInput.begin(), refrenInput.end());
    return refrenString;
}

void solveRefrenSuffArr() {
    std::string refrenString = readRefrenInput();
    NStringStructures::NSuffStructures::SuffixArray arr =
            NStringStructures::NSuffStructures::SuffixArray(255, refrenString);
    arr.build();
    std::vector<int> suffArr = arr.getSuffArray();
    arr.buildLCP();
    std::vector<int> LCP = arr.getLCP();
    std::vector<std::pair<int, int> > LCPPreprocessing;
    for (int i = 0; i < LCP.size(); ++i) {
        LCPPreprocessing.emplace_back(LCP[i], i);
    }
    std::sort(LCPPreprocessing.begin(), LCPPreprocessing.end());
    long long bestAns = suffArr.size();
    std::set<int> ansFinder;
    int bestR = suffArr.size(), bestL = 0;
    ansFinder.insert(-1);
    ansFinder.insert(LCP.size());
    for (int i = 0; i <  LCPPreprocessing.size(); ++i) {
        auto curEl = ansFinder.lower_bound(LCPPreprocessing[i].second);
        auto curL = curEl;
        curL--;
        auto curR = curEl;
        if (bestAns < 1LL * LCPPreprocessing[i].first * (*curR - *curL)) {
            bestL = suffArr[*curL + 1];
            bestR = bestL + LCPPreprocessing[i].first;
            bestAns = 1LL * LCPPreprocessing[i].first * (*curR - *curL);
        }
        ansFinder.insert(LCPPreprocessing[i].second);
    }
    std::cout << bestAns << '\n';
    std::cout << bestR - bestL << '\n';
    for (int i = bestL; i < bestR; ++i) {
        printf("%d ", refrenString[i] - '0' + 1);
    }
}

void solveRefrenSuffAut() {
    std::string refrenString = readRefrenInput();
    NStringStructures::NSuffStructures::SuffixAutomaton aut = NStringStructures::NSuffStructures::SuffixAutomaton();
    aut.build(refrenString);
    aut.buildDistances();
    std::vector<NStringStructures::NSuffStructures::SuffixAutomaton::VertexMetaInfo> autVertexes = 
                                                                                        aut.getAutomationMetaInfo();
    long long bestAns = refrenString.size();
    int startIdx = 0;
    int endIdx = refrenString.size();
    for (int i = 0; i < autVertexes.size(); ++i) {
        if (bestAns < 1LL * autVertexes[i].countTermRoads * autVertexes[i].maxLen) {
            bestAns = 1LL * autVertexes[i].countTermRoads * autVertexes[i].maxLen;
            startIdx = autVertexes[i].firstEncounter - autVertexes[i].maxLen + 1;
            endIdx = startIdx + autVertexes[i].maxLen;
        }
    }
    std::cout << bestAns << '\n';
    std::cout << endIdx - startIdx << '\n';
    for (int i = startIdx; i < endIdx; ++i) {
        std::cout << refrenString[i] - '0' + 1 << " ";
    }
}

void calculateAnswerTree(NStringStructures::NSuffStructures::SuffixTree::VertexMetaInfo* curVertex, 
                         NStringStructures::NSuffStructures::SuffixTree::VertexMetaInfo* metaRoot, 
                                    long long& bestAns, int& cumSum, const std::string& baseString_, 
                                        std::vector<std::pair<int, int> >& curPos, bool secondPhase, 
                                                                                bool& answerPrinted) {
    if (secondPhase && answerPrinted) {
        return;
    }
    if (curVertex != metaRoot) {
        cumSum += curVertex->end - curVertex->start + 1;
        if (curVertex->end == baseString_.length() - 1) {
            cumSum -= 1;
            curPos.push_back({curVertex->start, curVertex->end - 1});
        } else {
            curPos.push_back({curVertex->start, curVertex->end});
        }
    }
    for (auto it = curVertex->children.begin(); it != curVertex->children.end(); ++it) {
        if (it->first != '$') {
            calculateAnswerTree(it->second, metaRoot, bestAns, cumSum, baseString_, curPos, secondPhase, answerPrinted);
        }
    }
    if (secondPhase && answerPrinted) {
        return;
    }
    if (secondPhase && bestAns == 1LL * cumSum * curVertex->childCount) {
        int len = 0;
        for (int i = 0; i < curPos.size(); ++i) {
            len += curPos[i].second - curPos[i].first + 1;
        }
        std::cout << len << '\n';
        for (int i = 0; i < curPos.size(); ++i) {
            for (int j = curPos[i].first; j <= curPos[i].second; ++j) {
                std::cout << baseString_[j] - '0' + 1 << " ";
            }
        }
        answerPrinted = true;
        return;
    } else if (bestAns < 1LL * cumSum * curVertex->childCount) {
        bestAns = 1LL * cumSum * curVertex->childCount;
    }
    if (curVertex != metaRoot) {
        cumSum -= curVertex->end - curVertex->start + 1;
        if (curVertex->end == baseString_.length() - 1) {
            cumSum += 1;
        }
        curPos.pop_back();
    }
}

void solveRefrenSuffTree() {
    std::string refrenString = readRefrenInput();
    refrenString += "$";
    NStringStructures::NSuffStructures::SuffixTree tree = NStringStructures::NSuffStructures::SuffixTree(refrenString);
    tree.buildDistances();
    NStringStructures::NSuffStructures::SuffixTree::VertexMetaInfo* metaRoot = tree.buildMetaInfo();
    int cumSum = 0;
    long long bestAns = refrenString.length() - 1;
    bool answerPrinted = false;
    std::vector<std::pair<int, int> > curPos;
    calculateAnswerTree(metaRoot, metaRoot, bestAns, cumSum, refrenString, curPos, false, answerPrinted);
    std::cout << bestAns << '\n';
    if (bestAns == refrenString.length() - 1) {
        std::cout << refrenString.length() - 1 << '\n';
        for (int i = 0; i < refrenString.length() - 1; ++i) {
            std::cout << refrenString[i] - '0' + 1 << " ";
        }
    } else {
        calculateAnswerTree(metaRoot, metaRoot, bestAns, cumSum, refrenString, curPos, true, answerPrinted);
    }
}

int main(int argc, char const* argv[])
{
    solveRefrenSuffAut();
    return 0;
}
