# Documentación del Proyecto - Laberinto

Este proyecto es una implementación de un juego de laberinto en la consola utilizando la biblioteca `ncurses` para la visualización, y en el que un jugador puede recorrer un laberinto generado aleatoriamente. El juego tiene la capacidad de mover al jugador por el laberinto, mostrar un rastro del camino recorrido, y resolver el laberinto mediante algoritmos como el de "mano izquierda" y "backtracking". Además, se guarda la puntuación del jugador y se mantienen las mejores puntuaciones en un archivo.

Este documento proporciona instrucciones detalladas para configurar un entorno de desarrollo en Linux e instalar la biblioteca `ncurses`.

---

## 1. **Requisitos Previos**

### 1.1. **Distribución Linux Requerida**
Asegúrate de estar usando una distribución basada en Debian/Ubuntu, Fedora, Arch, o cualquier otra que permita la instalación de herramientas de desarrollo y bibliotecas C.

### 1.2. **Herramientas Necesarias**
1. **Compilador GCC:** Necesario para compilar programas en C que utilizan `ncurses`.
2. **Biblioteca `ncurses`:** Proporciona funciones para interfaces de usuario basadas en terminal.
3. **Editor de Texto:** Puedes usar Vim, Nano, o cualquier editor con soporte para C.

---

## 2. **Configuración del Entorno**

### 2.1. **Actualizar el Sistema**
Ejecuta los siguientes comandos para actualizar el sistema:
```bash
sudo apt update && sudo apt upgrade -y  # Para distribuciones basadas en Debian/Ubuntu
sudo dnf update -y                     # Para distribuciones Fedora
sudo pacman -Syu                       # Para distribuciones Arch
```

### 2.2. **Instalar Herramientas de Desarrollo**
Asegúrate de que las herramientas esenciales de desarrollo estén instaladas:

#### Debian/Ubuntu:
```bash
sudo apt install build-essential -y
```

#### Fedora:
```bash
sudo dnf groupinstall "Development Tools" -y
```

#### Arch:
```bash
sudo pacman -S base-devel
```

---

## 3. **Instalación de la Biblioteca `ncurses`**

### 3.1. **Verificar si `ncurses` está instalado**
Ejecuta:
```bash
ldconfig -p | grep ncurses
```
Si el comando no muestra ningún resultado, procede con la instalación.

### 3.2. **Instalar `ncurses`**

#### Debian/Ubuntu:
```bash
sudo apt install libncurses5-dev libncursesw5-dev -y
```

#### Fedora:
```bash
sudo dnf install ncurses-devel -y
```

#### Arch:
```bash
sudo pacman -S ncurses
```

### 3.3. **Confirmar la Instalación**
Ejecuta:
```bash
pkg-config --libs --cflags ncurses
```
Si no hay errores, la instalación fue exitosa.

---

## 4. **Crear un Programa de Prueba con `ncurses`**

### 4.1. **Código de Ejemplo**
Crea un archivo llamado `programa_ncurses.c` y copia el siguiente código:

```c
#include <ncurses.h>

int main() {
    initscr();              // Inicia ncurses
    printw("Hola, ncurses!\n");
    refresh();              // Muestra en pantalla
    getch();                // Espera una entrada del usuario
    endwin();               // Finaliza ncurses
    return 0;
}
```

### 4.2. **Compilar el Programa**
Compila el programa con:
```bash
gcc programa_ncurses.c -o programa_ncurses -lncurses
```

### 4.3. **Ejecutar el Programa**
Ejecuta el programa:
```bash
./programa_ncurses
```
Deberías ver "Hola, ncurses!" en la terminal.

---

## 5. **Solución de Problemas**

### Error: `ncurses.h: No such file or directory`
- Asegúrate de haber instalado correctamente las bibliotecas de desarrollo de `ncurses`.

### Error: `undefined reference to 'initscr'`
- Asegúrate de incluir `-lncurses` al compilar.

---

## 6. **Referencias Adicionales**
- [Documentación Oficial de ncurses](https://invisible-island.net/ncurses/ncurses.html)
- [Guía de Programación con ncurses](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)



## 7.  **Estructura del Código**

### Estructura `JugadorPuntuacion`
- **nombre**: Representa el nombre del jugador.
- **puntuacion**: Defiene la puntuacion del jugador.

### Enumeración `Color`

La enumeración `Color` define los colores para diferentes elementos visuales en la pantalla.

- **rojo**: Representa el color para el jugador y los obstáculos en la ruta.
- **verde**: Utilizado para mostrar el camino recorrido por el jugador.
- **blanco**: Usado para resaltar las celdas vacías o el fondo del laberinto.

### Estructura `JugadorPuntuacion` define el nombre del jugador y la puntuacion de la partida terminda.
- **nombre**: Nombre del jugador actual.
- **puntuacion**: Puntuacion del jugador.

### Clase `Laberinto`

La clase `Laberinto` representa el laberinto del juego, incluyendo su generación y visualización.

#### Atributos

- **ancho**: El ancho del laberinto (en número de celdas).
- **alto**: La altura del laberinto (en número de celdas).
- **laberinto**: Un arreglo unidimensional que representa el laberinto. Los valores son:
  - `1` para paredes,
  - `0` para caminos libres,
  - `2` y `3` para celdas que el jugador ha pasado.

#### Métodos

- **Laberinto(size_type x, size_type y)**: Constructor que inicializa un laberinto de tamaño `x` por `y` y lo llena con paredes. Genera caminos aleatorios y establece puntos de entrada y salida.
- **~Laberinto()**: Destructor que libera la memoria utilizada por el laberinto.

- **generar_camino(int x, int y)**: Genera un camino aleatorio en las celdas adyacentes utilizando una técnica de randomización. Modifica las celdas de paredes (`1`) a caminos (`0`).

- **imprimir(const Color &color)**: Imprime el laberinto en pantalla con un color específico para las paredes y caminos. Utiliza la biblioteca `ncurses` para la representación en consola.

- **imprimir_rastro(const Color &color1, const Color &color2, const Color &color3)**: Imprime el laberinto mostrando el rastro del jugador. Usa diferentes colores para indicar el camino recorrido.

- **getlaberinto()**: Retorna el arreglo que representa el laberinto.

- **getancho() y getalto()**: Devuelven el ancho y alto del laberinto respectivamente.

### Clase `Jugador`

La clase `Jugador` gestiona la posición del jugador dentro del laberinto y las operaciones asociadas a su movimiento.

#### Atributos

- **x**: Coordenada `x` del jugador.
- **y**: Coordenada `y` del jugador.
- **contador**: Contador de los movimientos realizados por el jugador.

#### Métodos

- **Jugador(size_type xx, size_type yy)**: Constructor que inicializa la posición del jugador en las coordenadas `xx`, `yy`.

- **imprimir(const Color &color)**: Imprime la posición del jugador en el laberinto.

- **imprimirInfo(const Laberinto &m, const Color &color)**: Muestra información adicional sobre la posición y los movimientos del jugador.

- **puedeMover(Laberinto &m, int dx, int dy)**: Verifica si el jugador puede moverse a una nueva posición determinada por las coordenadas `(dx, dy)` dentro del laberinto. Esto asegura que el jugador no se mueva fuera del laberinto o a una pared.

- **moverConDelay(Laberinto &m, int dx, int dy)**: Mueve al jugador con un retraso visual, de modo que el movimiento sea visible en la pantalla.

- **mov(Laberinto &m, const size_type xx, const size_type yy)**: Realiza el movimiento del jugador en las coordenadas `(xx, yy)` dentro del laberinto. Actualiza el laberinto con el nuevo estado del jugador.

- **solverBacktrackingRecursivo(Laberinto &m, vector<vector<bool>> &visitado)**: Implementación recursiva del algoritmo de backtracking para resolver el laberinto. Intenta mover al jugador hacia la salida del laberinto explorando las celdas no visitadas.

- **solverManoIzquierda(Laberinto &m)**: Resuelve el laberinto usando la técnica de "mano izquierda", donde el jugador mantiene una mano (izquierda) en las paredes para encontrar la salida.

- **solverBacktracking(Laberinto &m)**: Inicia la resolución del laberinto con backtracking. Marca las celdas visitadas y llama a `solverBacktrackingRecursivo()` para realizar la búsqueda.

- **guardarPuntuacion()**: Guarda la puntuación del jugador (basada en la cantidad de movimientos) en un archivo de texto llamado `puntuacion.txt`.

- **listarTopCincoPuntuaciones()**: Lee el archivo `puntuacion.txt` y devuelve las cinco mejores puntuaciones almacenadas.

### Función Principal

La función principal se encarga de inicializar la pantalla, configurar el color y manejar la interacción entre el jugador y el laberinto. Se realizan los siguientes pasos:

1. Inicializar `ncurses` y configurar la interfaz visual.
2. Crear un laberinto con un tamaño predefinido.
3. Iniciar un jugador en la posición de entrada del laberinto.
4. Permitir que el jugador se mueva dentro del laberinto utilizando las teclas de dirección.
5. Mostrar el rastro del jugador y la información de su posición.
6. Ejecutar un algoritmo para resolver el laberinto (por ejemplo, `solverBacktracking` o `solverManoIzquierda`).
7. Guardar la puntuación final del jugador y mostrar las mejores puntuaciones.

## 8. **Instrucciones para Ejecutar**

1. Compila el código con el siguiente comando:

   ```bash
   g++ laberinto.cpp -o output/laberinto -lncurses && ./output/laberinto
   ```
