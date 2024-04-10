//
// Created by JIE TIAN on 2024/4/10.
//

#include "AStar.h"


RealPoint::RealPoint(const RealPoint &realPoint) {
    setCurrentPoint(realPoint.getCurrentPoint());
    setParentPoint(realPoint.getParentPoint());
    setFx(realPoint.getFx());
    setGx(realPoint.getGx());
    setHx(realPoint.getHx());
}

int AStar::calcFxOnOpenList() {
    int min = INT_MAX;
    for (std::list<RealPoint>::iterator it = openList.begin(); it != openList.end(); ++it) {
        it->calcFx();
        if (min > it->getFx())
            min = it->getFx();
    }
    return min;
}

void AStar::transOpenToClose(int min) {
    for (std::list<RealPoint>::iterator it = openList.begin(); it != openList.end();) {
        if (it->getFx() == min) {
            closeList.push_back(RealPoint(*it));
            temp.x = it->getCurrentPoint().x;
            temp.y = it->getCurrentPoint().y;
            it = openList.erase(it);
            return;
        } else
            ++it;
    }
}

bool AStar::isInCloseList(int iX, int iY) {
    for (std::list<RealPoint>::iterator it = closeList.begin(); it != closeList.end(); ++it)
        if ((it->getCurrentPoint().x == iX) && (it->getCurrentPoint().y == iY))
            return true;
    return false;
}

bool AStar::isInOpenList(int iX, int iY) {
    for (std::list<RealPoint>::iterator it = openList.begin(); it != openList.end(); ++it)
        if ((it->getCurrentPoint().x == iX) && (it->getCurrentPoint().y == iY))
            return true;
    return false;
}

bool AStar::isABlock(int iX, int iY) {
    if (map[iX][iY] == 0)
        return false;
    else
        return true;
}

int AStar::getParentGxInCloseList(int iX, int iY) {
    for (std::list<RealPoint>::iterator it = closeList.begin(); it != closeList.end(); ++it)
        if ((it->getCurrentPoint().x == iX) && (it->getCurrentPoint().y == iY))
            return it->getGx();
    return 0;
}

void AStar::seleteAndAdd() {
    Point t;
    t.x = temp.x - 1;
    t.y = temp.y - 1;

    for (int i = 0; i < 9; ++i) {
        if (t.x < 0 || t.x >= mapX || t.y < 0 || t.y >= mapY);
        else {
            if (isInCloseList(t.x, t.y) || isABlock(t.x, t.y));

            else {
                if (!isInOpenList(t.x, t.y)) {
                    addToOpenList(RealPoint(t.x, t.y, temp.x, temp.y));
                    for (std::list<RealPoint>::iterator it = openList.begin(); it != openList.end(); ++it)
                        if ((it->getCurrentPoint().x == t.x) && (it->getCurrentPoint().y == t.y)) {
                            if (i == 0 || i == 2 || i == 6 || i == 8)
                                it->setGx(getParentGxInCloseList(temp.x, temp.y) + 14);
                            else
                                it->setGx(getParentGxInCloseList(temp.x, temp.y) + 10);

                            it->setHx((abs(endPoint.x - t.x) + abs(endPoint.y - t.y)) * 10);
                        }
                } else {
                    for (std::list<RealPoint>::iterator it = openList.begin(); it != openList.end(); ++it)
                        if ((it->getCurrentPoint().x == t.x) && (it->getCurrentPoint().y == t.y)) {
                            if (i == 0 || i == 2 || i == 6 || i == 8) {
                                if (it->getGx() > (getParentGxInCloseList(temp.x, temp.y) + 14)) {
                                    it->setParentPoint(temp);
                                    it->setGx(getParentGxInCloseList(temp.x, temp.y) + 14);
                                }
                            } else {
                                if (it->getGx() > (getParentGxInCloseList(temp.x, temp.y) + 10)) {
                                    it->setParentPoint(temp);
                                    it->setGx(getParentGxInCloseList(temp.x, temp.y) + 10);
                                }
                            }
                        }
                }
            }
        }

        if (i == 2 || i == 5) { //用于遍历相邻的八个点
            ++t.x;
            t.y = temp.y - 1;
        } else
            ++t.y;
    }
}

bool AStar::totalLogic() {
    addToOpenList(RealPoint(startPoint.x, startPoint.y));

    do {
        transOpenToClose(calcFxOnOpenList());
        seleteAndAdd();
    } while (!(isInOpenList(endPoint.x, endPoint.y) || openList.empty()));

    if (openList.empty())
        return false;
    else {
        findWay();
        return true;
    }
}

AStar::~AStar() {
    for (int i = 0; i < mapX; ++i)
        delete (map[i]);
}

AStar::AStar(int mX, int mY) {
    mapX = mX;
    mapY = mY;
    map = new int *[mX];
    for (int i = 0; i < mX; ++i)
        map[i] = new int[mY];

    for (int x = 0; x < mX; ++x)
        for (int y = 0; y < mY; ++y)
            map[x][y] = 0;
}

bool AStar::changeBlockState(int mX, int mY) {
    if (mX < 0 || mX >= mapX || mY < 0 || mY >= mapY)
        return false;
    else {
        if (map[mX][mY] == 0)
            map[mX][mY] = 1;
        else
            map[mX][mY] = 0;
        return true;
    }
}

void AStar::findWay() {
    returnList.push_back(Point(endPoint.x, endPoint.y));
    const Point &rec = getFatherInOpenList(endPoint);
    returnList.push_back(Point(rec.x, rec.y));

    while (true) {
        const Point &point = getFatherInCloseList(returnList.back());
        if ((point.x == -1) && (point.y == -1))
            break;
        else
            returnList.push_back(Point(point.x, point.y));
    }
    returnList.reverse();
}

const Point &AStar::getFatherInOpenList(Point &endPoint) {
    for (std::list<RealPoint>::iterator it = openList.begin(); it != openList.end(); ++it)
        if ((it->getCurrentPoint().x == endPoint.x) && (it->getCurrentPoint().y == endPoint.y))
            return it->getParentPoint();
}

const Point &AStar::getFatherInCloseList(Point &curPoint) {
    for (std::list<RealPoint>::iterator it = closeList.begin(); it != closeList.end(); ++it)
        if ((it->getCurrentPoint().x == curPoint.x) && (it->getCurrentPoint().y == curPoint.y))
            return it->getParentPoint();
}

void AStar::clearAllPoints() {
    openList.clear();
    closeList.clear();
    returnList.clear();

    for (int x = 0; x < mapX; ++x)
        for (int y = 0; y < mapY; ++y)
            map[x][y] = 0;
}



