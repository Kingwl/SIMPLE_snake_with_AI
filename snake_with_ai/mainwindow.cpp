#include "mainwindow.h"
int change[4][2] = {{-1,0}, {0,1}, {1,0}, {0,-1}};
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),timer(new QTimer(this))
{
    srand(unsigned (time(nullptr)));
    resize(640, 480);
    memset(map, none, sizeof(map));
    connect(timer, SIGNAL(timeout()), this, SLOT(move()));

    map[2][0] = snake_head;
    map[1][0] = snake_body;
    map[0][0] = snake_foot;
    createfood();

    timer->setInterval(150);
    timer->start();

    que.push_back(QPoint(0, 0));
    que.push_back(QPoint(1, 0));
    que.push_back(QPoint(2, 0));
}
MainWindow::~MainWindow()
{
}
void MainWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    for(int i = 0; i <= MAX_SIZE; ++i)
    {
        p.drawLine(40, 40 + i * 40, 440, 40 + i * 40);
        p.drawLine(40 + i * 40, 40, 40 + i * 40, 440);
    }
    QBrush b;
    b.setStyle(Qt::SolidPattern);
    for(int i = 0; i < MAX_SIZE; ++i)
    {
        for(int j = 0; j < MAX_SIZE; ++j)
        {
            int color = map[i][j];
            if(color == none)
            {
                continue;
            }else if(color == snake_head){
                b.setColor(Qt::red);
            }else if(color == snake_body ){
                b.setColor(Qt::blue);
            }else if(color == food){
                b.setColor(Qt::green);
            }else if(color == snake_foot){
                b.setColor(Qt::gray);
            }
            p.setBrush(b);
            p.drawRect(40 + 40 * i, 40 + 40 * j, 40, 40);
        }
    }
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    auto k = event->key();
    if(k == Qt::Key_W)
    {
        if(state_now != to_down) state_now = to_up;
    }else if(k == Qt::Key_S){
        if(state_now != to_up) state_now = to_down;
    }else if(k == Qt::Key_A){
        if(state_now != to_right) state_now = to_left;
    }else if(k == Qt::Key_D){
        if(state_now != to_left) state_now = to_right;
    }
}
void MainWindow::go_left()
{
    QPoint p = que.back();
    p = QPoint(p.x() - 1, p.y());
    moveOne(p);
}
void MainWindow::go_right()
{
    QPoint p = que.back();
    p = QPoint(p.x() + 1, p.y());
    moveOne(p);

}
void MainWindow::go_up()
{
    QPoint p = que.back();
    p = QPoint(p.x(), p.y() - 1);
    moveOne(p);
}
void MainWindow::go_down()
{
    QPoint p = que.back();
    p = QPoint(p.x(), p.y() + 1);
    moveOne(p);
}
void MainWindow::move()
{
    auto s_head = *(que.end()-1);
    auto s_food = QPoint(food_pos.x(), food_pos.y());
    auto s_foot = *(que.begin());
    state_now = find_way(s_head, s_food);
    if(state_now == -1){
        state_now = find_way(s_head, s_foot);
    }
    if(state_now == to_left)
    {
        go_left();
    }else if(state_now == to_right){
        go_right();
    }else if(state_now == to_up){
        go_up();
    }else if(state_now == to_down){
        go_down();
    }else{
        state_now = getRandomStep(s_head);
        if(state_now == -1){
            QMessageBox::information(0, "", "end");
            gameover();
        }else if(state_now == to_left){
            go_left();
        }else if(state_now == to_right){
            go_right();
        }else if(state_now == to_up){
            go_up();
        }else if(state_now == to_down){
            go_down();
        }
    }
    update();
}
bool MainWindow::is_in(QPoint p)
{
    if(p.x() < 0 || p.y() < 0 || p.x() >= MAX_SIZE || p.y() >= MAX_SIZE || map[p.x()][p.y()] != none && map[p.x()][p.y()] != food)
    {
        return false;
    }
    return true;
}
bool MainWindow::is_in(point p)
{
    if(p.x < 0 || p.y < 0 || p.x >= MAX_SIZE || p.y >= MAX_SIZE || map[p.x][p.y] != none && map[p.x][p.y] != food)
    {
        return false;
    }
    return true;
}
void MainWindow::gameover()
{
    exit(0);
}
void MainWindow::moveOne(QPoint p)
{
    if(is_in(p) == false){
        gameover();
        return;
    }
    bool flag = false;
    if(map[p.x()][p.y()] == food) flag = true;
    map[p.x()][p.y()] = snake_body;
    que.push_back(QPoint(p.x(), p.y()));
    if(flag == false){
        p = que.front();
        map[p.x()][p.y()] = none;
        que.pop_front();
    }else{
        createfood();
    }
    auto iter = que.begin() + 1;
    for(; iter != que.end(); ++iter)
    {
        map[iter -> x()][iter -> y()] = snake_body;
    }
    iter = que.end() - 1;
    map[iter -> x()][iter -> y()] = snake_head;
    iter = que.begin();
    map[iter -> x()][iter -> y()] = snake_foot;
}
void MainWindow::createfood()
{
    std::vector<QPoint> v;
    for(int i = 0; i < MAX_SIZE; ++i)
    {
        for(int j = 0; j < MAX_SIZE; ++j)
        {
            if(map[i][j] == none)
            {
                v.push_back(QPoint(i, j));
            }
        }
    }
    int n = rand() % v.size();
    map[v[n].x()][v[n].y()] = food;
    food_pos = QPoint(v[n].x(), v[n].y());
}
int MainWindow::find_way(QPoint start,QPoint finish)
{
    init();
    bool flag = false;
    point s;
    s = point(start.x(), start.y());
    s.calcH(finish);
    s.calcF();
    open.push_back(s);

    while(!open.empty())
    {
        sort(open.begin(), open.end());
        point *tmp = new point(*open.begin());
        open.erase(open.begin());
        close[tmp -> x][tmp -> y] = true;

        if(tmp -> x == finish.x() && tmp -> y == finish.y())
        {
            ans = *tmp;
            flag = true;
            break;
        }
        for(int i = 0; i < 4; ++i)
        {
            point *ct = new point(tmp -> x + change[i][0], tmp -> y + change[i][1]);
            if(is_in(*ct) && close[ct -> x][ct -> y] == false)
            {
                std::vector<point>::iterator iter = is_in_que(*ct);
                if(iter == open.end())
                {
                    ct -> parent = tmp;
                    ct -> g = ct -> calcG();
                    ct -> h = ct -> calcH(finish);
                    ct -> calcF();
                    open.push_back(*ct);
                }else{
                    ct -> parent = tmp;
                    ct -> g = ct -> calcG();

                    int G = iter -> g;
                    if(G > ct -> g){
                        iter -> parent = ct -> parent;
                        iter -> g = G;
                        iter -> calcF();
                    }
                }
            }
        }
    }

    point* po = &ans;
    while(po -> parent != nullptr && po -> parent -> parent != nullptr && po -> parent -> parent != nullptr)
    {
        po = po -> parent;
    }

    if(flag){
        if(s.x < po->x)
        {
            return to_right;
        }else if(s.x > po -> x)
        {
            return to_left;
        }else if(s.y < po -> y)
        {
            return to_down;
        }else if(s.y > po -> y){
            return to_up;
        }
    }
    return -1;
}
 void MainWindow::init()
 {
    open.clear();
    ans = point();
    int m[MAX_SIZE][MAX_SIZE];
    memcpy(m,map,sizeof(m));
    memset(close,0,sizeof(close));
    for(int i = 0;i < MAX_SIZE; ++i)
    {
        for(int j = 0; j < MAX_SIZE; ++j)
        {
            if(m[i][j] != none && map[i][j] != food) close[i][j] = true;
        }
    }
 }
int MainWindow::getRandomStep(QPoint p)
{
    int m[MAX_SIZE][MAX_SIZE];
    memcpy(m,map,sizeof(m));
    bool flag = true;
    int cou = 0;
    int step = state_now;
    while(flag){
        step = (step + 1)%4;
        switch (step) {
        case to_left:
                if(is_in(QPoint(p.x() - 1, p.y())) && m[p.x() - 1][p.y()] == none){
                    flag = false;
                }
            break;
        case to_right:
                if(is_in(QPoint(p.x() + 1, p.y())) && m[p.x() + 1][p.y()] == none){
                    flag = false;
                }
            break;
        case to_up:
                if(is_in(QPoint(p.x(), p.y() - 1)) && m[p.x()][p.y() - 1] == none){
                    flag = false;
                }
            break;
        case to_down:
                if(is_in(QPoint(p.x(), p.y() + 1)) && m[p.x()][p.y() + 1] == none){
                    flag = false;
                }
            break;
        }
        cou++;
        if(cou > 400){
            step = -1;
            break;
        }
    }
    return step;

}
