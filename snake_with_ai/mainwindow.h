#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMessageBox>
#include <QKeyEvent>
#include <QTimer>
#include <queue>
#include <vector>
#include <time.h>
#include <algorithm>


class MainWindow : public QMainWindow
{
    Q_OBJECT
    static const int MAX_SIZE = 10;
    static const int none = 0;
    static const int snake_head = 1;
    static const int snake_body = 2;
    static const int snake_foot = 3;
    static const int food = 4;
    static const int to_left = 0;
    static const int to_right = 1;
    static const int to_up = 2;
    static const int to_down = 3;
    static const int cost = 10;
    int state_now = to_right;

    class point{
    public:
        point(int _x = 0, int _y = 0):x(_x),y(_y){
            f = g = h = 0;
            parent = nullptr;
        }
        int x,y;
        int f,g,h;
        point *parent;
        bool operator< (const point &o) const {return f < o.f;}
        point(const point& p ){
            x = p.x;
            y = p.y;
            parent = p.parent;
            g = p.g;
            h = p.h;
            f = p.f;
        }
        void calcF(){
            f = g + h;
        }
        int calcG(){
            int G = cost;
            if(parent != nullptr) G += parent -> g;
            return G;
        }

    };

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool is_in(QPoint p);
    bool is_in(point p);
    int find_way(QPoint start,QPoint finish);
    std::vector<point>::iterator MainWindow::is_in_que(point p){
       auto iter = open.begin();
       for(; iter != open.end(); ++iter)
       {
            if(iter -> x == p.x && iter -> y == p.y) break;
       }
        return iter;
    }
    void init();
protected:
    void paintEvent(QPaintEvent*);
    void keyPressEvent(QKeyEvent  *event);
public slots:
    void go_left();
    void go_right();
    void go_up();
    void go_down();
    void move();
    void gameover();
    void moveOne(QPoint p);
    void createfood();
    QPoint getHead();
    QPoint getFoot();
    int getRandomStep(QPoint p);
    int calcH(point p, QPoint food_p) {
        return sqrt((p.x - food_p.x()) * (p.x - food_p.x()) + (p.y - food_p.y()) * (p.y - food_p.y()));
    }
private:
    int map[MAX_SIZE][MAX_SIZE];
    QTimer *timer;
    std::deque<QPoint> que;
    std::vector <point> open;
    bool close[MAX_SIZE][MAX_SIZE];
    QPoint food_pos;
    point ans;
  //  int change[4][2];

};

#endif // MAINWINDOW_H
