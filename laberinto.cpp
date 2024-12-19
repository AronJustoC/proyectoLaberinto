#include <iostream> //Libreria para entrada y salida
#include <ncurses.h>
#include <random>

using namespace std;

enum class Color { rojo = 1, verde, blanco };

class Laberinto {
  using size_type = unsigned long;

private:
  const size_type ancho;
  const size_type alto;
  char *laberinto;
  mt19937 generador;
  uniform_int_distribution<int> dist;

public:
  // keyword "explicit": hace que el tipo de argumento sea especificado
  explicit Laberinto(const size_type x, const size_type y) : ancho{x}, alto{y} {
    // Asignacion y inicializacion de la matriz
    laberinto = new char[ancho * alto];
    for (size_type i = 0; i < alto * ancho; i++) {
      laberinto[i] = 1;
    }

    // generador de numeros aleatorios
    random_device seeder;
    mt19937 generador(seeder());
    uniform_int_distribution<int> dist(0, 3);

    // Crea un laberinto
    laberinto[1 * ancho + 1] = 0;
    for (size_type y = 1; y < alto; y += 2) {
      for (size_type x = 1; x < ancho; x += 2) {
        generar_camino(x, y);
      }
    }

    // Asignacion de puntos de entrada y salida
    laberinto[0 * ancho + 1] = 0;
    laberinto[(alto - 1) * ancho + (ancho - 2)] = 0;
  }

  Laberinto(const Laberinto &) = delete;
  Laberinto &operator=(const Laberinto &) = delete;
  Laberinto(Laberinto &&) = delete;
  Laberinto &operator=(Laberinto &&) = delete;

  ~Laberinto() noexcept { delete[] laberinto; }

  int myrand() { // Generara numeros aleatorios [0:3] de la misma probabilidad.
    return dist(generador); // Representaran las 4 direcciones posibles
  }

  void generar_camino(int x, int y) {
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0, dx = 0, dy = 0, dir = 0, contador = 0;
    dir = myrand();
    while (contador < 4) { // Busca 4 direcciones aleatorias
      dx = 0;
      dy = 0;
      switch (dir) {
      case 0:
        dx = 1;
        break;
      case 1:
        dy = 1;
        break;
      case 2:
        dx = -1;
        break;
      default:
        dy = -1;
        break;
      }
      x1 = x + dx;
      y1 = y + dy;
      x2 = x1 + dx;
      y2 = y1 + dy;
      if (x2 > 0 && x2 < ancho && y2 > 0 && y2 < alto &&
          laberinto[y1 * ancho + x1] == 1 && laberinto[y2 * ancho + x2] == 1) {
        laberinto[y1 * ancho + x1] = 0;
        laberinto[y2 * ancho + x2] = 0;
        x = x2;
        y = y2;
        dir = myrand();
        contador = 0;
      } else {
        dir = (dir + 1) % 4;
        contador += 1;
      }
    }
  }

  void imprimir(const Color &color) {
    attron(COLOR_PAIR(color));
    move(0, 0);
    for (size_type y = 0; y < alto; ++y) {
      for (size_type x = 0; x < ancho; ++x) {
        printw(laberinto[y * ancho + x] == 1 ? "[]" : "  ");
      }
      printw("\n");
    }
    attroff(COLOR_PAIR(color));
  }

  const char *getlaberinto() const noexcept { return laberinto; }
  const size_type getancho() const noexcept { return ancho; }
  const size_type getalto() const noexcept { return alto; }
};

class Jugador {
  using size_type = long;

private:
  size_type x;
  size_type y;
  size_type contador;

public:
  explicit Jugador(const size_type xx, const size_type yy)
      : x{xx}, y{yy}, contador{0} {}

  Jugador() : x{1}, y{0}, contador{0} {}

  void imprimir(const Color &color) {
    attron(COLOR_PAIR(color));
    move(y, x * 2);
    printw("00");
    attroff(COLOR_PAIR(color));
  }

  void imprimirInfo(const Laberinto &m, const Color &color) {
    attron(COLOR_PAIR(color));
    move(m.getalto() + 1, 0); // Impresion una linea debajo del laberinto
    printw("Posicion actual X: %li, Y: %li\n Total de movimientos: %li", x, y,
           contador);
    attroff(COLOR_PAIR(color));
  }

  void imprimirGanador(const Color &color) {
    move(0, 0);
    clear();
    refresh();
    attron(COLOR_PAIR(color));
    printw("Ganaste!!!\n Total de movimientos %li", contador);
    getch();
    attroff(COLOR_PAIR(color));
  }

  void mov(const Laberinto &m, const size_type xx, const size_type yy) {
    static auto mz =
        m.getlaberinto(); // obtiene el laberinto para poder mover el personaje

    // Si pa posicion estan fuera del laberinto no se mueve
    if (x + xx <= 0 || x + xx >= m.getancho() || y + yy <= 0 ||
        y + yy >= m.getalto())
      return;
    // Si el laberinto esta ocupado por un muro no se mueve
    if (mz[(y + yy) * m.getancho() + (x + xx)] == 1)
      return;

    x += xx;
    y += yy;
    ++contador;
  }

  const size_type getpos_x() const noexcept { return x; }
  const size_type getpos_y() const noexcept { return y; }
  const size_type getcontador() const noexcept { return contador; }
};

class VentanaCurses {
private:
  int x;
  int y;

public:
  VentanaCurses() : x{0}, y{0} {
    // Establecer el entorno local para soportar UTF-8
    setlocale(LC_ALL, "");

    initscr();
    if (has_colors() == FALSE) {
      endwin();
      cout << "Tu terminal no soporta colores\n";
      exit(1);
    }
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    curs_set(0);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    getmaxyx(stdscr, y, x);
  }

  ~VentanaCurses() noexcept { endwin(); }

  const int getx() const noexcept { return x; }
  const int gety() const noexcept { return y; }
};

int main() {
  VentanaCurses miVentana;
  int ancho = 23, alto = 15;
  while (ancho < (miVentana.getx() / 2) - 3)
    ancho += 2;
  while (alto < miVentana.gety() - 4)
    alto += 2;

  bool banderaSalir = false;

  // Crear un laberinto y un jugador
  Laberinto m(ancho, alto);
  Jugador p;
  int ch = 0;

  while (!banderaSalir) {
    m.imprimir(Color::verde);
    p.imprimir(Color::rojo);
    p.imprimirInfo(m, Color::blanco);
    ch = getch();

    switch (ch) {
    case 'w':
    case KEY_UP:
      p.mov(m, 0, -1);
      break;
    case 's':
    case KEY_DOWN:
      p.mov(m, 0, 1);
      break;
    case 'a':
    case KEY_LEFT:
      p.mov(m, -1, 0);
      break;
    case 'd':
    case KEY_RIGHT:
      p.mov(m, 1, 0);
      break;
    case 'q':
    case KEY_EXIT:
    case 27:
      banderaSalir = true;
      break;
    default:
      refresh();
    }
    // verificar si se ha llegado al final
    if (p.getpos_y() == m.getalto() - 1) {
      p.imprimirGanador(Color::blanco);
      banderaSalir = true;
    }
  }
}
