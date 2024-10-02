#include <list>
//==1. Поддержка экрана в форме матрицы символов ==

using namespace std;

char screen[YMAX] [XMAX];

enum color {
    black = '*',
    white = '.'
};

void screen_init() {
    for (auto & y : screen)
        for (auto &x : y)  x = white;
}

void screen_destroy() {
    for (auto & y : screen)
        for (auto &x : y)  x = black;
}

bool on_screen(int a, int b) {  // проверка попадания точки на экран
    return 0 <= a && a < XMAX && 0 <= b && b < YMAX;
}

void put_point(int a, int b) {
    if (on_screen(a,b)) screen[b] [a] = black;
}

void put_line(int x0, int y0, int x1, int y1) {
    /*
        Алгоритм Брезенхэма для прямой:
        рисование отрезка прямой от (x0,y0) до (x1,y1).
        Уравнение прямой: b(x-x0) + a(y-y0) = 0.
        Минимизируется величина abs(eps), где eps = 2*(b(x-x0)) + a(y-y0).
    */

    int dx = 1;
    int a = x1 - x0;   if (a < 0) dx = -1, a = -a;
    int dy = 1;
    int b = y1 - y0;   if (b < 0) dy = -1, b = -b;
    int two_a = 2*a;
    int two_b = 2*b;
    int xcrit = -b + two_a;
    int eps = 0;

    for (;;) { // Формирование прямой линии по точкам
        put_point(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        if (eps <= xcrit) x0 += dx, eps += two_b;
        if (eps >= a || a < b) y0 += dy, eps -= two_a;
    }
}

void screen_clear() {
    screen_init( );
} // Очистка экрана

void screen_refresh() { // Обновление экрана
    for (int y = YMAX-1; 0 <= y; --y) { // с верхней строки до нижней
        for (auto x : screen[y])    // от левого столбца до правого
            cout << x;
        cout << '\n';
    }
}
//== 2. Библиотека фигур ==
struct shape { // Виртуальный базовый класс "фигура"
    static list<shape*> shapes;// Список фигур (один на все фигуры!)
    shape() {
        shapes.push_back(this);
    } //Фигура присоединяется к списку
    virtual point north() const = 0;   //Точки для привязки
    virtual point south() const = 0;
    virtual point east() const = 0;
    virtual point west() const = 0;
    virtual point neast() const = 0;
    virtual point seast() const = 0;
    virtual point nwest() const = 0;
    virtual point swest() const = 0;
    virtual void draw() = 0;      //Рисование
    virtual void move(int, int) = 0;   //Перемещение
    virtual void resize(int) = 0;  //Изменение размера
};

list<shape*> shape::shapes; // Размещение списка фигур

void shape_refresh() { // Перерисовка всех фигур на экране
    screen_clear();
    for (auto p:shape::shapes) p->draw(); //Динамическое связывание!!!
    screen_refresh();
}

class rotatable:virtual public shape { //Фигуры, пригодные к повороту
public:
    virtual void rotate_left() = 0;    //Повернуть влево
    virtual void rotate_right() = 0;   //Повернуть вправо
};

class reflectable:virtual public shape { // Фигуры, пригодные
    // к зеркальному отражению
public:
    virtual void flip_horizontally() = 0;  // Отразить горизонтально
    virtual void flip_vertically() = 0;    // Отразить вертикально
};

class line:public shape {
/*
    отрезок прямой ["w", "e"].
    north( ) определяет точку "выше центра отрезка и так далеко
    на север, как самая его северная точка", и т. п.
*/
protected:
    point w, e;
public:
    line (point a, point b) : w(a), e(b) { };
    line (point a, int L) : w(point(a.xo + L - 1, a.yo)), e(a) {  };
    point north() const override { return point((w.xo + e.xo) / 2, e.yo < w.yo ? w.yo : e.yo); }
    point south() const override { return point((w.xo + e.xo) / 2, e.yo < w.yo ? e.yo : w.yo); }
    point east() const override { return point(e.xo < w.xo ? w.xo : e.xo, (w.yo + e.yo) / 2); }
    point west() const override { return point(e.xo < w.xo ? e.xo : w.xo, (w.yo + e.yo) / 2); }
    point neast() const override { return point(w.xo < e.xo ? e.xo : w.xo, e.yo < w.yo ? w.yo : e.yo); }
    point seast() const override { return point(w.xo < e.xo ? e.xo : w.xo, e.yo < w.yo ? e.yo : w.yo); }
    point nwest() const override { return point(w.xo < e.xo ? w.xo : e.xo, e.yo < w.yo ? w.yo : e.yo); }
    point swest() const override { return point(w.xo < e.xo ? w.xo : e.xo, e.yo <w.yo ? e.yo : w.yo); }
    void move(int a, int b) override { w.xo += a; w.yo += b; e.xo += a; e.yo += b; }
    void draw() override { put_line(w, e); }
    void resize(int d) override { e.xo += (e.xo - w.xo) * (d - 1); e.yo += (e.yo - w.yo) * (d - 1); } // Увеличение длины линии в (d) раз
};

// Прямоугольник
class rectangle:public rotatable {
/* nw ------ n ------ ne
   |                |
   |                |
   w        c        e
   |                |
   |                |
   sw ------ s ------ se */
protected:
    point sw, ne;
public:
    rectangle(point a, point b) :  sw(a), ne(b) { }
    point north() const override {
        return point((sw.xo + ne.xo) / 2, ne.yo);
    }
    point south() const override {
        return point((sw.xo + ne.xo) / 2, sw.yo);
    }
    point east() const override {
        return point(ne.xo, (sw.yo + ne.yo) / 2);
    }
    point west() const override {
        return point(sw.xo, (sw.yo + ne.yo) / 2);
    }
    point neast() const override {
        return ne;
    }
    point seast() const override {
        return point(ne.xo, sw.yo);
    }
    point nwest() const override {
        return point(sw.xo, ne.yo);
    }
    point swest() const override {
        return sw;
    }
    void rotate_right() override { // Поворот вправо относительно se
        int w = ne.xo - sw.xo,
                h = ne.yo - sw.yo; // (учитывается масштаб по осям)
        sw.xo = ne.xo - h * 2;
        ne.yo = sw.yo + w / 2;
    }
    void rotate_left() override { // Поворот влево относительно sw
        int w = ne.xo - sw.xo,
                h = ne.yo - sw.yo;
        ne.xo = sw.xo + h * 2;
        ne.yo = sw.yo + w / 2;
    }
    void move(int a, int b) override {
        sw.xo += a;
        sw.yo += b;
        ne.xo += a;
        ne.yo += b;
    }
    void resize(int d) override {
        ne.xo += (ne.xo - sw.xo) * (d - 1);
        ne.yo += (ne.yo - sw.yo) * (d - 1);
    }
    void draw() override {
        put_line(nwest(), ne);
        put_line(ne, seast( ));

        put_line(seast(), sw);
        put_line(sw, nwest());
    }
};

void up(shape& p, const shape& q) { // поместить p над q
    // Это ОБЫЧНАЯ функция, не член класса! Динамическое связывание!!
    point n = q.north();
    point s = p.south();
    p.move(n.xo - s.xo, n.yo - s.yo + 1);
}

void pos2(shape& p, const shape& q) {
    point n = q.nwest();
    point s = p.swest();
    p.move(n.xo + s.xo - 8, n.yo + s.yo - 8);
}

void pos3(shape& p, const shape& q) {
    point n = q.east();
    point s = p.seast();
    p.move(n.xo + s.xo - 8, s.yo + 10);
}

void pos7(shape& p, const shape& q) {
    point n = q.nwest();
    point s = p.swest();
    p.move(s.xo - n.xo + 1, n.yo + s.yo);
}

void pos8(shape& p, const shape& q) {
    point n = q.neast();
    point s = p.seast();
    p.move(n.xo, n.yo + s.yo);
}