#include <algorithm>
#include <fstream>
#include <iostream>
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
  Laberinto(const size_type x, const size_type y) : ancho{x}, alto{y} {
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

  void imprimir_rastro(const Color &color1, const Color &color2,
                       const Color &color3) {
    move(0, 0);
    for (size_type y = 0; y < alto; ++y) {
      for (size_type x = 0; x < ancho; ++x) {
        int celda = laberinto[y * ancho + x];
        if (celda == 1) {
          attron(COLOR_PAIR(color1));
          printw("[]"); // Pared
          attroff(COLOR_PAIR(color1));
        } else if (celda == 2) {
          attron(COLOR_PAIR(color2));
          printw("o "); // Pasó dos veces
          attroff(COLOR_PAIR(color2));
        } else if (celda == 3) {
          attron(COLOR_PAIR(color3));
          printw("x "); // Pasó tres veces
          attroff(COLOR_PAIR(color3));
        } else {
          printw("  "); // Camino vacío
        }
      }
      printw("\n");
    }
    refresh();
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

  char *getlaberinto() noexcept { return laberinto; }
  const size_type getancho() const noexcept { return ancho; }
  const size_type getalto() const noexcept { return alto; }
};

class Jugador {
  using size_type = long;

private:
  size_type x;
  size_type y;
  size_type contador;

  bool puedeMover(Laberinto &m, int dx, int dy) {
    auto mz = m.getlaberinto();
    size_type newX = x + dx;
    size_type newY = y + dy;

    if (newX <= 0 || newX >= m.getancho() || newY <= 0 || newY >= m.getalto())
      return false;

    return mz[newY * m.getancho() + newX] != 1;
  }

  void moverConDelay(Laberinto &m, int dx, int dy) {
    mov(m, dx, dy);
    m.imprimir_rastro(Color::verde, Color::blanco, Color::rojo);
    imprimir(Color::rojo);
    imprimirInfo(m, Color::blanco);
    refresh();
    napms(100); // Delay de 100ms para visualizar el movimiento
  }

  bool solverBacktrackingRecursivo(Laberinto &m,
                                   vector<vector<bool>> &visitado) {
    // Si llegamos a la última fila, hemos encontrado la salida
    if (y == m.getalto() - 1) {
      return true;
    }

    // Definir las cuatro direcciones posibles: derecha, abajo, izquierda,
    // arriba
    const int dx[] = {1, 0, -1, 0};
    const int dy[] = {0, 1, 0, -1};

    // Probar cada dirección
    for (int i = 0; i < 4; i++) {
      size_type newX = x + dx[i];
      size_type newY = y + dy[i];

      // Verificar si podemos movernos a la nueva posición
      if (newX > 0 && newX < m.getancho() && newY > 0 && newY < m.getalto() &&
          !visitado[newY][newX] && puedeMover(m, dx[i], dy[i])) {

        // Marcar como visitado
        visitado[newY][newX] = true;

        // Mover al jugador
        moverConDelay(m, dx[i], dy[i]);

        // Llamada recursiva desde la nueva posición
        if (solverBacktrackingRecursivo(m, visitado)) {
          return true; // Se encontró un camino a la salida
        }

        // Si no se encontró solución, retroceder
        moverConDelay(m, -dx[i], -dy[i]);

        // Opcional: marcar como no visitado para permitir otros caminos
        // visitado[newY][newX] = false;
      }
    }

    return false; // No se encontró solución desde esta posición
  }

public:
  Jugador(const size_type xx, const size_type yy) : x{xx}, y{yy}, contador{0} {}

  Jugador() : x{1}, y{0}, contador{0} {}

  void imprimir(const Color &color) {
    attron(COLOR_PAIR(color));
    move(y, x * 2);
    printw("00");
    attroff(COLOR_PAIR(color));
  }

  void imprimirInfo(const Laberinto &m, const Color &color) {
    attron(COLOR_PAIR(color));
    move(m.getalto() + 1, 0); // Impresión una línea debajo del laberinto
    printw("Posicion actual: X = %li, Y = %li\n", x, y);
    printw("Total de movimientos: %li\n", contador);
    attroff(COLOR_PAIR(color));
    refresh(); // Actualizar la pantalla
  }

  void mov(Laberinto &m, const size_type xx, const size_type yy) {
    auto mz = m.getlaberinto(); // Obtiene el laberinto para mover el personaje

    // Si la posición está fuera del laberinto, no se mueve
    if (x + xx <= 0 || x + xx >= m.getancho() || y + yy <= 0 ||
        y + yy >= m.getalto())
      return;

    // Si el laberinto está ocupado por un muro, no se mueve
    if (mz[(y + yy) * m.getancho() + (x + xx)] == 1)
      return;

    if (mz[y * m.getancho() + x] == 0) {
      mz[y * m.getancho() + x] = 2; // Primera vez
    } else if (mz[y * m.getancho() + x] == 2) {
      mz[y * m.getancho() + x] = 3; // Segunda vez
    }

    // Mueve el personaje
    x += xx;
    y += yy;

    ++contador;
  }

  void solverManoIzquierda(Laberinto &m) {
    // Dirección: 0 = norte, 1 = este, 2 = sur, 3 = oeste
    int direccion = 1; // Comenzamos mirando hacia el este

    while (y < m.getalto() - 1) { // Mientras no lleguemos al final
      // Checar la dirección izquierda
      int izquierda = (direccion + 3) % 4;
      int dx = 0, dy = 0;

      // Intentar moverse a la izquierda primero
      switch (izquierda) {
      case 0:
        dy = -1;
        break; // norte
      case 1:
        dx = 1;
        break; // este
      case 2:
        dy = 1;
        break; // sur
      case 3:
        dx = -1;
        break; // oeste
      }

      if (puedeMover(m, dx, dy)) {
        direccion = izquierda;
        moverConDelay(m, dx, dy);
      } else {
        // Si no puede ir a la izquierda, intenta seguir adelante
        dx = dy = 0;
        switch (direccion) {
        case 0:
          dy = -1;
          break;
        case 1:
          dx = 1;
          break;
        case 2:
          dy = 1;
          break;
        case 3:
          dx = -1;
          break;
        }

        if (puedeMover(m, dx, dy)) {
          moverConDelay(m, dx, dy);
        } else {
          // Si no puede seguir adelante, gira a la derecha
          direccion = (direccion + 1) % 4;
        }
      }
    }
  }

  void solverBacktracking(Laberinto &m) {
    // Matriz para marcar las celdas visitadas
    vector<vector<bool>> visitado(m.getalto(),
                                  vector<bool>(m.getancho(), false));
    visitado[y][x] = true; // Marcar posición inicial como visitada

    // Inicializar visualización
    m.imprimir_rastro(Color::verde, Color::blanco, Color::rojo);
    imprimir(Color::rojo);
    imprimirInfo(m, Color::blanco);

    // Comenzar búsqueda recursiva
    solverBacktrackingRecursivo(m, visitado);
  }

  void guardarPuntuacion() {
    ofstream file;

    // Apertura en modo de adición
    file.open("puntuacion.txt", ios::app);

    if (!file) {
      cerr << "Error: No se pudo abrir el archivo 'puntuacion.txt'." << endl;
      return;
    }

    int puntuacionBase = 1000;
    int puntuacion = puntuacionBase - contador;

    // Guardar la puntuación en el archivo
    file << puntuacion << endl;

    file.close();

    printw("\nPuntuación guardada exitosamente: %d\n", puntuacion);
  }

  vector<int> listarTopCincoPuntuaciones() {
    ifstream file("puntuacion.txt");
    if (!file) {
      cerr << "Error: No se pudo abrir el archivo 'puntuacion.txt'." << endl;
      return {};
    }

    vector<int> puntuaciones;
    int puntuacion;

    // Leer las puntuaciones desde el archivo
    while (file >> puntuacion) {
      puntuaciones.push_back(puntuacion);
    }
    file.close();

    // Ordenar en orden descendente
    sort(puntuaciones.begin(), puntuaciones.end(), greater<int>());

    return puntuaciones;
  }

  void imprimirGanador(const Color &color) {
    clear();
    refresh();
    attron(COLOR_PAIR(color));

    printw("                ¡Ganaste!                \n");
    printw("Puntuación: %li\n", 1000 - contador);
    printw("Total de movimientos: %li\n", contador);

    guardarPuntuacion();

    printw("\nLos 5 mejores puntuaciones:\n");

    // Almacenar el resultado de listarTopCincoPuntuaciones() en una variable
    auto puntuaciones = listarTopCincoPuntuaciones();

    for (size_t i = 0; i < min(puntuaciones.size(), size_t(5)); i++) {
      printw("%zu. %d\n", i + 1, puntuaciones[i]); // Usar %d si son enteros
    }

    attroff(COLOR_PAIR(color));
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

class MenuJuego {
private:
  VentanaCurses &ventana;
  Laberinto *laberinto;
  Jugador *jugador;
  int ancho, alto;
  int puntuacion;

public:
  MenuJuego(VentanaCurses &v, int w, int h)
      : ventana(v), ancho(w), alto(h), puntuacion(0) {
    laberinto = new Laberinto(ancho, alto);
    jugador = new Jugador();
  }

  ~MenuJuego() {
    delete laberinto;
    delete jugador;
  }

  void mostrarMenu() {
    clear();
    printw("MENU PRINCIPAL\n");
    printw("[1] Jugar\n");
    printw("[2] Resolver (Mano Izquierda)\n");
    printw("[3] Resolver (Backtracking)\n");
    printw("[4] Ver Puntuación\n");
    printw("[5] Salir\n");
    refresh();
  }

  void ejecutarJuego() {
    bool salir = false;
    while (!salir) {
      mostrarMenu();
      int opcion;
      scanw("%d", &opcion);

      switch (opcion) {
      case 1:
        jugarPartida();
        break;
      case 2:
        resolverManoIzquierda();
        break;
      case 3:
        resolverBacktracking();
        break;
      case 4:
        mostrarPuntuacion();
        break;
      case 5:
        salir = true;
        break;
      default:
        printw("Opción no válida\n");
        getch();
      }
    }
  }

private:
  void jugarPartida() {
    resetearJuego();
    bool partidaActiva = true;
    int ch = 0;

    while (partidaActiva) {
      actualizarPantalla();

      ch = getch();
      switch (ch) {
      case 'w':
      case KEY_UP:
        jugador->mov(*laberinto, 0, -1);
        break;
      case 's':
      case KEY_DOWN:
        jugador->mov(*laberinto, 0, 1);
        break;
      case 'a':
      case KEY_LEFT:
        jugador->mov(*laberinto, -1, 0);
        break;
      case 'd':
      case KEY_RIGHT:
        jugador->mov(*laberinto, 1, 0);
        break;
      case 'r':
        resetearJuego();
        break;
      case 'q':
      case 27:
        partidaActiva = false;
        break;
      }

      if (verificarVictoria()) {
        actualizarPuntuacion();
        mostrarMensajeVictoria();
        partidaActiva = false;
      }
    }
    esperarTecla();
  }

  void resolverManoIzquierda() {
    resetearJuego();
    actualizarPantalla();
    jugador->solverManoIzquierda(*laberinto);
    if (verificarVictoria()) {
      mostrarMensajeVictoria();
    }
    esperarTecla();
  }

  void resolverBacktracking() {
    resetearJuego();
    actualizarPantalla();
    jugador->solverBacktracking(*laberinto);
    if (verificarVictoria()) {
      mostrarMensajeVictoria();
    }
    esperarTecla();
  }

  void actualizarPantalla() {
    laberinto->imprimir_rastro(Color::verde, Color::blanco, Color::rojo);
    jugador->imprimir(Color::rojo);
    jugador->imprimirInfo(*laberinto, Color::blanco);
    refresh();
  }

  void resetearJuego() {
    delete laberinto;
    delete jugador;

    laberinto = new Laberinto(ancho, alto);
    jugador = new Jugador();

    clear();
    refresh();
  }

  bool verificarVictoria() {
    return jugador->getpos_y() == laberinto->getalto() - 1;
  }

  void actualizarPuntuacion() { puntuacion += 100; }

  void mostrarPuntuacion() {
    clear();
    printw("Puntuación actual: %d\n", puntuacion);
    esperarTecla();
  }

  void mostrarMensajeVictoria() { jugador->imprimirGanador(Color::blanco); }

  void esperarTecla() {
    printw("\nPresione cualquier tecla para continuar...");
    getch();
  }
};

int main() {
  // Inicializar ncurses
  initscr();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  start_color();

  // Configurar dimensiones
  VentanaCurses ventana;
  int ancho = 23, alto = 15;
  while (ancho < (ventana.getx() / 2) - 3)
    ancho += 2;
  while (alto < ventana.gety() - 4)
    alto += 2;

  // Crear y ejecutar el juego
  MenuJuego menu(ventana, ancho, alto);
  menu.ejecutarJuego();

  // Finalizar ncurses
  endwin();
  return 0;
}
