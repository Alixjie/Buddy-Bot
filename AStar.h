//
// Created by 田杰 on 2018/4/15.
//

#ifndef ASTAR_ASTAR_H
#define ASTAR_ASTAR_H

#include <list>
#include <climits>
#include <cmath>

struct Point {
    int x;
    int y;

    Point()
    { x = y = -1; }  //Point的无参构造函数

    Point(int pX, int pY)
    { //Point的带参构造函数
        x = pX;
        y = pY;
    }
};

class RealPoint {
private:
    Point parentPoint;
    Point currentPoint;

public:
    void setParentPoint(const Point &point)
    {  //设置父亲节点
        parentPoint.x = point.x;
        parentPoint.y = point.y;
    }

    const Point &getParentPoint() const
    { return parentPoint; }  //得到父亲节点

    const Point &getCurrentPoint() const
    { return currentPoint; }  //得到当前节点

private:
    void setCurrentPoint(const Point &point)
    {  //设置当前节点
        currentPoint.x = point.x;
        currentPoint.y = point.y;
    }

    void setFx(int fX)
    { fx = fX; }  //设置fx的值

private:
    int fx;
    int gx;
    int hx;

public:
    const int getFx() const
    { return fx; }  //得到fx的值

    void calcFx()
    { fx = gx + hx; }  //计算fx

    const int getGx() const
    { return gx; }  //得到gx的值

    void setGx(int gX)
    { gx = gX; }  //设置gx的值

    const int getHx() const
    { return hx; }  //得到hx的值

    void setHx(int hX)
    { hx = hX; }  //设置hx的值

public:
    RealPoint(int cX, int cY) : currentPoint(cX, cY)
    { fx = gx = hx = 0; }  //RealPoint不带父亲的构造函数

    RealPoint(int cX, int cY, int pX, int pY) : currentPoint(cX, cY), parentPoint(pX, pY)
    {  //RealPoint带父亲的构造函数
        hx = gx = hx = 0;
    }

    RealPoint(const RealPoint &realPoint);  //RealPoint的拷贝构造函数
};

class AStar {
private:
    Point startPoint;
    Point endPoint;
    Point temp;

public:
    const Point &getStartPoint() const
    { return startPoint; }  //得到开始点

    const Point &getEndPoint() const
    { return endPoint; }  //得到终止点

    void setStartPoint(const Point &point)
    {  //设置开始点
        startPoint.x = point.x;
        startPoint.y = point.y;
    }

    void setEndPoint(const Point &point)
    {  //设置终止点
        endPoint.x = point.x;
        endPoint.y = point.y;
    }

private:
    std::list<RealPoint> openList;
    std::list<RealPoint> closeList;
    std::list<Point> returnList;

public:
    std::list<Point> &getReturnList()
    { return returnList; }

    void findWay();

    const Point &getFatherInOpenList(Point &endPoint);

    const Point &getFatherInCloseList(Point &curPoint);

    void clearAllPoints();

    void addToOpenList(RealPoint realPoint)
    { openList.push_back(realPoint); }  //往openList里加RealPoint

    std::list<RealPoint> &getOpenList()
    { return openList; }  //用于测试

    std::list<RealPoint> &getCloseList()
    { return closeList; }  //用于测试

    int calcFxOnOpenList();  //计算openList里各节点的fx的值

    void transOpenToClose(int min);  //从openList里删除节点并添加到closeList

    bool isInCloseList(int iX, int iY);  //是否在closeList里

    bool isInOpenList(int iX, int iY);  //是否在openList里

    bool isABlock(int iX, int iY);  //是否是障碍物

    int getParentGxInCloseList(int iX, int iY);  //在closeList里得到父节点gx值

    void seleteAndAdd();  //根据父节点创建符合要求的节点并加入到openList中

    bool totalLogic();  //AStar算法开始函数

private:
    int mapX;
    int mapY;
    int **map;

public:
    int **getMap()
    { return map; }  //得到地图信息函数

public:
    AStar(int mX, int mY);  //AStar带参构造函数

    ~AStar();  //析构函数

    bool changeBlockState(int mX, int mY);  //改变地图上的障碍物信息

};


#endif //ASTAR_ASTAR_H
