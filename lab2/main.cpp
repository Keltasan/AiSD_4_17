#include <iostream>
#include "screen.h"
#include "shape.h"

using namespace std;

// Пример добавки: дополнительный фрагмент - полуокружность
class h_circle:public rectangle, public reflectable {
    bool reflected;
public:
    h_circle(point a, point b, bool r=true) : rectangle(a, b), reflected(r) { }
    void draw () override;
    void flip_horizontally ( ) override { }; // Отразить горизонтально (пустая функция)
    void flip_vertically ( ) override {
        reflected = !reflected;
    }; // Отразить вертикально
};

void h_circle::draw() { // Алгоритм Брезенхэма для окружностей
    // (выдаются два сектора, указываемые значением reflected)
    int x0 = (sw.xo + ne.xo) / 2,
            y0 = reflected ? sw.yo : ne.yo;

    int radius = (ne.xo - sw.xo) / 2;
    int x = 0,
            y = radius,
            delta = 2 - 2 * radius,
            error = 0;

    while (y >= 0) { // Цикл рисования
        if (reflected) {
            put_point(x0 + x, y0 + y * 0.7);
            put_point(x0 - x, y0 + y * 0.7);
        } else {
            put_point(x0 + x, y0 - y*0.7);
            put_point(x0 - x, y0 - y*0.7);
        }

        error = 2 * (delta + y) - 1;
        if (delta < 0 && error <= 0) {
            ++x;
            delta += 2 * x + 1;
            continue;
        }

        error = 2 * (delta - x) - 1;
        if (delta > 0 && error > 0) {
            --y;
            delta += 1 - 2 * y;
            continue;
        }

        ++x;
        delta += 2 * (x - y);
        --y;
    }
}

// Пример добавки: дополнительная функция присоединения…
void down(shape &p, const shape &q) {
    point n = q.south();
    point s = p.north();
    p.move(n.xo - s.xo, n.yo - s.yo - 1);
}

// Cборная пользовательская фигура - физиономия
class face: public rectangle { // Моя фигура ЯВЛЯЕТСЯ
    int w, h; // прямоугольником
    line l_eye; // левый глаз – моя фигура СОДЕРЖИТ линию
    line r_eye; // правый глаз
    line mouth; // рот
public:
    face(point, point);
    void draw() override;
    void move(int, int) override;
    void resize(int) override { }
};

face::face(point a, point b) : rectangle(a, b),    // Инициализация базового класса
                               w(neast().xo - swest().xo + 1), // Инициализация данных
                               h(neast().yo - swest().yo + 1), // строго в порядке объявления!
                               l_eye(point(swest().xo + 2, swest().yo + h * 3 / 4), 2),
                               r_eye(point(swest().xo + w - 4, swest().yo + h * 3 / 4), 2),
                               mouth(point(swest().xo + 2, swest().yo + h / 4), w - 4) { }

void face::draw() {
    rectangle::draw(); // Контур лица (глаза и нос рисуются сами!)
    int a = (swest().xo + neast().xo) / 2;
    int b = (swest().yo + neast().yo) / 2;
    put_point(point(a, b)); // Нос – существует только на рисунке!
}

void face::move(int a, int b) {
    rectangle::move(a, b);
    l_eye.move(a, b);
    r_eye.move(a, b);
    mouth.move(a, b);
}

class rectangleWithPlus: public rectangle {
    int w, h;
    line verticalLine;
    line horizontalLine;
public:
    rectangleWithPlus(point, point);
    void draw() override;
    void move(int, int) override;
    void resize(int) override { }
};

rectangleWithPlus::rectangleWithPlus(point a, point b) : rectangle(a, b),  // Инициализация базового класса
                                                         w(neast().xo - swest().xo + 1),
                                                         h(neast().yo - swest().yo + 1),
                                                         verticalLine(point(north().xo, north().yo), point(south().xo, south().yo)),
                                                         horizontalLine(point(west().xo, west().yo), point(east().xo, east().yo)) { }

void rectangleWithPlus::draw() {
    rectangle::draw();

    int a = (swest().xo + neast().xo) / 2;
    int b = (swest().yo + neast().yo) / 2;
    put_point(point(a, b));
}

void rectangleWithPlus::move(int a, int b) {
    rectangle::move(a, b);
    verticalLine.move(a, b);
    horizontalLine.move(a, b);
}

int main () {
    screen_init();

    // 1. Объявление набора фигур
    rectangle hat(point(0, 0), point(14, 5));
    line brim(point(0,15),17);
    face face(point(15, 10), point(27, 18));
    h_circle beard(point(40,10), point(50,20));
    rectangleWithPlus rectangleWithPlus2(point(0, 0), point(8, 4));
    rectangleWithPlus rectangleWithPlus3(point(0, 0), point(8, 4));
    rectangleWithPlus rectangleWithPlus7(point(0, 0), point(8, 4));
    rectangleWithPlus rectangleWithPlus8(point(0, 0), point(8, 4));
    // rectangleWithPlus rectangleWithPlusTest(point(0, 0), point(0, 4));
    shape_refresh();
    cout << endl << "Generated..." << endl << endl;
    //cin.get();

    // 2. Подготовка к сборке
    hat.rotate_right();
    brim.resize(2);
    face.resize(2);
    beard.flip_vertically();
    shape_refresh();
    cout << endl << "Prepared..." << endl << endl;
    // cin.get(); // Смотреть результат поворотов/отражений

    // 3.Сборка изображения
    // face.move(0, -10); // Лицо - в исходное положение

    up(brim, face);
    up(hat, brim);
    pos2(rectangleWithPlus2, face);
    pos3(rectangleWithPlus3, face);
    pos7(rectangleWithPlus7, brim);
    pos8(rectangleWithPlus8, brim);
    down(beard, face);
    shape_refresh();
    cout << endl << "Ready!" << endl << endl;
    // cin.get(); // Смотреть результат
    screen_destroy( );
    return 0;
}