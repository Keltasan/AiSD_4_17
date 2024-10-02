const int XMAX = 100; //Размер экрана
const int YMAX = 50;

class point { // Точка на экране
public:
    int xo, yo;
    explicit point(int x = 0, int y = 0) : xo(x), yo(y) {  }
};
// Набор утилит для работы с экраном
void put_point(int a, int b); // Вывод точки (2 варианта)
void put_point(point p) {
    put_point(p.xo, p.yo);
}
void put_line(int, int, int, int); // Вывод линии (2 варианта)
void put_line(point a, point b) {
    put_line(a.xo, a.yo, b.xo, b.yo);
}
void screen_init(); // Создание экрана
void screen_destroy(); // Удаление
void screen_refresh(); // Обновление
void screen_clear();   // Очистка