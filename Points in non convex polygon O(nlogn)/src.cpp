#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cstdio>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/detail/standard_policies.hpp>

using namespace __gnu_pbds;

typedef long long TCoord;

#ifndef _SCANLINE_
#define _SCANLINE_


namespace NGeometry {
    namespace NScanline {
        enum TPointOrigin{startPoint, endPoint, requestPoint};

        class Point {
        public:
            Point() {}

            Point(TCoord x, TCoord y, size_t originIndex, TPointOrigin type): x_(x), y_(y), originIndex_(originIndex), type_(type) {}

            ~Point() = default;

            Point(const Point& point) {
                x_ = point.x_;
                y_ = point.y_;
                originIndex_ = point.originIndex_;
                type_ = point.type_;
            }

            Point(Point&& point) {
                x_ = point.x_;
                y_ = point.y_;
                originIndex_ = point.originIndex_;
                type_ = point.type_;
            }

            Point& operator = (const Point& point) {
                x_ = point.x_;
                y_ = point.y_;
                originIndex_ = point.originIndex_;
                type_ = point.type_;
                return *this;
            }

            void operator = (Point&& point) {
                x_ = point.x_;
                y_ = point.y_;
                originIndex_ = point.originIndex_;
                type_ = point.type_;
            }

            TPointOrigin getType() const {
                return type_;
            }

            void setType(TPointOrigin type) {
                type_ = type;
            }

            int getSegment() const {
                return originIndex_;
            }

            void setSegment(int segmentNum)  {
                originIndex_ = segmentNum;
            }

            void setAnswerIndex(int idx) {
                originIndex_ = idx;
            }

            int getAnswerIndex() const {
                return originIndex_;
            }

            TCoord getX() const {
                return x_;
            }

            void setX(TCoord x) {
                x_ = x;
            }

            TCoord getY() const {
                return y_;
            }

            void setY(TCoord y) {
                y_ = y;
            }

            bool operator == (const Point& otherPoint) const {
                return (x_ == otherPoint.x_) && (y_ == otherPoint.y_);
            }
        private:
            TCoord x_, y_;
            size_t originIndex_;
            TPointOrigin type_;
        };

        long long crossProduct(Point p1, Point p2) {
            return p1.getX() * p2.getY() - p1.getY() * p2.getX();
        }

        bool collinearVectors(Point p1, Point p2) {
            return !crossProduct(p1, p2);
        }

        bool ComparePoints(const Point& p1, const Point& p2) {
            if (p1.getX() == p2.getX()) {
                if (p1.getType() == p2.getType()) {
                    return p1.getY() < p2.getY();
                }
                return p1.getType() < p2.getType();
            }
            return p1.getX() < p2.getX();
        }

        bool compareVerticalPoints(const Point& p1, const Point& p2) {
            if (p1.getX() == p2.getX()) {
                if (p1.getType() == p2.getType()) {
                    return p1.getY() < p2.getY();
                }
                if (p1.getType() == startPoint) {
                    return true;
                }
                if (p2.getType() == startPoint) {
                    return false;
                }
                if (p1.getType() == requestPoint)
                {
                    return true;
                }
                return false;
            }
            return p1.getX() < p2.getX();
        }

        enum TPointPosition{INSIDE, BORDER, OUTSIDE};

        class Segment {
        public:
            Segment() {}

            Segment(Point s, Point f): start_(s), finish_(f) {}

            Segment(const Segment& segment) {
                start_ = segment.start_;
                finish_ = segment.finish_;
            }

            Point getStart() const {
                return start_;
            }

            void setStart(Point point) {
                start_ = point;
            }

            Point getFinish() const {
                return finish_;
            }

            void setFinish(Point point) {
                finish_ = point;
            }

            bool isDot() const {
                return start_ == finish_;
            }

            bool isHorizontal() const {
                return start_.getX() != finish_.getX();
            }

            void rotateSegment() {
                std::swap(start_, finish_);
            }
        private:
            Point start_, finish_;
        };

        bool pointOnSegment(Point point, Segment segment) {
            if (segment.isDot()) {
                if (point == segment.getStart()) {
                    return true;
                }
                return false;
            }
            Point joint;
            joint.setX(point.getX() - segment.getStart().getX());
            joint.setY(point.getY() - segment.getStart().getY());
            Point lineVector;
            lineVector.setX(segment.getFinish().getX() - segment.getStart().getX());
            lineVector.setY(segment.getFinish().getY() - segment.getStart().getY());
            if (collinearVectors(joint, lineVector)) {
                long long x1 = segment.getStart().getX();
                long long x2 = segment.getFinish().getX();
                long long y1 = segment.getStart().getY();
                long long y2 = segment.getFinish().getY();
                if (x2 < x1) {
                    std::swap(x1, x2);
                }
                if (y2 < y1) {
                    std::swap(y1, y2);
                }
                bool onX = (x1 <= point.getX() && point.getX() <= x2);
                bool onY = (y1 <= point.getY() && point.getY() <= y2);
                return onX && onY;
            }
            return false;
        }

        struct CompareSegments{
            bool operator () (const Segment& s1, const Segment& s2) const {
                Point s1Vector, s2Vector;
                s1Vector.setX(s1.getFinish().getX() - s1.getStart().getX());
                s1Vector.setY(s1.getFinish().getY() - s1.getStart().getY());
                s2Vector.setX(s2.getFinish().getX() - s2.getStart().getX());
                s2Vector.setY(s2.getFinish().getY() - s2.getStart().getY());
                if (s1.getStart().getX() < s2.getStart().getX()) {
                    Point joint;
                    joint.setX(s2.getStart().getX() - s1.getStart().getX());
                    joint.setY(s2.getStart().getY() - s1.getStart().getY());
                    if (crossProduct(s1Vector, joint) == 0) {
                        return true;
                    }
                    return crossProduct(s1Vector, joint) > 0;

                }
                if (s1.getStart().getX() > s2.getStart().getX()) {
                    Point joint;
                    joint.setX(s1.getStart().getX() - s2.getStart().getX());
                    joint.setY(s1.getStart().getY() - s2.getStart().getY());
                    if (crossProduct(s2Vector, joint) == 0) {
                        return false;
                    }
                    return crossProduct(s2Vector, joint) < 0;
                }
                if (s1.getStart() == s2.getStart()) {
                    if (s1.isDot()) {
                        return false;
                    }
                    if (s2.isDot()) {
                        return true;
                    }
                    return crossProduct(s1Vector, s2Vector) > 0;
                }
                return s1.getStart().getY() < s2.getStart().getY();
            }
        };

        struct CompareVerticalSegments {
            bool operator () (const Segment& s1, const Segment& s2) const {
                if (s1.getStart().getY() == s2.getStart().getY()) {
                    return s1.getFinish().getY() < s2.getFinish().getY();
                }
                return s1.getStart().getY() < s2.getStart().getY();
            }
        };

        typedef tree<Segment, null_type, CompareSegments, rb_tree_tag, tree_order_statistics_node_update> indexedSet;

        class Scanline {
        public:
            Scanline(std::vector<Segment>& polygonSegments, std::vector<Point>& querryPoints) {
                querryPointPositions.resize(querryPoints.size());
                for (size_t i = 0; i < polygonSegments.size(); ++i) {
                    Segment curSegment = polygonSegments[i];
                    if (curSegment.isDot()) {
                        continue;
                    }
                    Point p1 = curSegment.getStart();
                    Point p2 = curSegment.getFinish();
                    p1.setType(startPoint);
                    p2.setType(startPoint);
                    if (ComparePoints(p2, p1)) {
                        curSegment.rotateSegment();
                    }
                    if (curSegment.isHorizontal()) {
                        handlePointHorizontal(curSegment.getStart(), startPoint);
                        handlePointHorizontal(curSegment.getFinish(), endPoint);
                        horizontalSegments.push_back(curSegment);
                    } else {
                        handlePointVertical(curSegment.getStart(), startPoint);
                        handlePointVertical(curSegment.getFinish(), endPoint);
                        verticalSegments.push_back(curSegment);
                    }
                }
                for (size_t i = 0; i < querryPoints.size(); ++i) {
                    Point querryPoint = querryPoints[i];
                    querryPoint.setAnswerIndex(i);
                    querryPoint.setType(requestPoint);
                    querriesAndIndexes.push_back(querryPoint);
                    pointsAndIndexes.push_back(querryPoint);
                }
                std::sort(pointsAndIndexes.begin(), pointsAndIndexes.end(), ComparePoints);
                std::sort(querriesAndIndexes.begin(), querriesAndIndexes.end(), compareVerticalPoints);
            }

            std::vector<TPointPosition> solve() {
                std::set<std::pair<long long, long long> > ends;
                indexedSet activeSegments;
                for (size_t i = 0; i < pointsAndIndexes.size(); ++i) {
                    Point curPoint = pointsAndIndexes[i];
                    int idx = curPoint.getAnswerIndex();
                    if (ends.begin()->first != curPoint.getX()) {
                        ends.clear();
                    }
                    if (curPoint.getType() == startPoint) {
                        activeSegments.insert(horizontalSegments[curPoint.getSegment()]);
                    }
                    if (curPoint.getType() == endPoint) {
                        activeSegments.erase(horizontalSegments[curPoint.getSegment()]);
                        ends.insert({curPoint.getX(), curPoint.getY()});
                    }
                    if (curPoint.getType() == requestPoint) {
                        auto intersectingEnd = ends.find({curPoint.getX(), curPoint.getY()});
                        if (intersectingEnd != ends.end()) {
                            querryPointPositions[idx] = BORDER;
                            continue;
                        }
                        Segment curSegment;
                        curSegment.setStart(curPoint);
                        curSegment.setFinish(curPoint);
                        auto botomSegment = activeSegments.lower_bound(curSegment);
                        if (!activeSegments.empty() && botomSegment != activeSegments.begin()) {
                            botomSegment--;
                            if (pointOnSegment(curPoint, *botomSegment)) {
                                querryPointPositions[idx] = BORDER;
                                continue;
                            }
                        }
                        bool rayCount = activeSegments.order_of_key(curSegment) % 2;
                        if (rayCount) {
                            querryPointPositions[idx] = INSIDE;
                            continue;
                        }
                        querryPointPositions[idx] = OUTSIDE;
                    }
                }
                std::set<Segment, CompareVerticalSegments> verticalAssistant;
                for (size_t i = 0; i < querriesAndIndexes.size(); ++i) {
                    Point curPoint = querriesAndIndexes[i];
                    int idx = curPoint.getAnswerIndex();
                    if (curPoint.getType() == startPoint) {
                        verticalAssistant.insert(verticalSegments[curPoint.getSegment()]);
                    }
                    if (curPoint.getType() == endPoint) {
                        verticalAssistant.erase(verticalSegments[curPoint.getSegment()]);
                    }
                    if (curPoint.getType() == requestPoint) {
                        Segment curSegment;
                        curSegment.setStart(curPoint);
                        Point pointCopy = curPoint;
                        pointCopy.setY(INF);
                        curSegment.setFinish(pointCopy);
                        auto botomSegment = verticalAssistant.lower_bound(curSegment);
                        if (!verticalAssistant.empty() && botomSegment != verticalAssistant.begin()) {
                            botomSegment--;
                            if (pointOnSegment(curPoint, *botomSegment)) {
                                querryPointPositions[idx] = BORDER;
                            }
                        }
                    }
                }
                return querryPointPositions;
            }

        private:
            std::vector<Point> pointsAndIndexes;
            std::vector<Point> querriesAndIndexes;
            std::vector<Segment> horizontalSegments;
            std::vector<Segment> verticalSegments;
            std::vector<TPointPosition> querryPointPositions;

            const TCoord INF = static_cast<TCoord> (1e9);

            void handlePointHorizontal(Point point, TPointOrigin type) {
                point.setSegment(horizontalSegments.size());
                point.setType(type);
                pointsAndIndexes.push_back(point);
            }

            void handlePointVertical(Point point, TPointOrigin type) {
                point.setSegment(verticalSegments.size());
                point.setType(type);
                querriesAndIndexes.push_back(point);
            }
        };
    }
}

#endif

void readAndSolve() {
    int tests;
    scanf("%d", &tests);
    for (size_t i = 0; i < tests; ++i) {
        int numPoints;
        scanf("%d", &numPoints);
        std::vector<NGeometry::NScanline::Point> points(numPoints);
        TCoord a, b;
        for (size_t i = 0; i < numPoints; ++i) {
            scanf("%lld %lld", &a, &b);
            NGeometry::NScanline::Point pt;
            pt.setX(a);
            pt.setY(b);
            points[i] = pt;
        }
        std::vector<NGeometry::NScanline::Segment> segments(numPoints);
        for (size_t i = 0; i < numPoints - 1; ++i) {
            segments[i].setStart(points[i]);
            segments[i].setFinish(points[i + 1]);
        }
        segments[segments.size() - 1].setStart(points.back());
        segments[segments.size() - 1].setFinish(points[0]);
        int testPointsNum;
        scanf("%d", &testPointsNum);
        std::vector<NGeometry::NScanline::Point> testPoints(testPointsNum);
        for (size_t i = 0; i < testPointsNum; ++i) {
            scanf("%lld %lld", &a, &b);
            NGeometry::NScanline::Point pt;
            pt.setX(a);
            pt.setY(b);
            testPoints[i] = pt;
        }
        NGeometry::NScanline::Scanline alg(segments, testPoints);
        std::vector<NGeometry::NScanline::TPointPosition> ans = alg.solve();
        for (size_t i = 0; i < ans.size(); ++i) {
            if (ans[i] == NGeometry::NScanline::INSIDE) {
                printf("INSIDE\n");
            } else if(ans[i] == NGeometry::NScanline::BORDER) {
                printf("BORDER\n");
            } else {
                printf("OUTSIDE\n");
            }
        }
        printf("\n");
    }
}

int main()
{
    readAndSolve();
}
