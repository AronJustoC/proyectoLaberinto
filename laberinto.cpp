#include <iostream> //Libreria para entrada y salida
#include <ncurses.h>
#include <random>

using namespace std;

enum class Color { red = 1, green, white };

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

  void Imprimir(const Color &color) {
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
};
