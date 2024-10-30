#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>

using namespace std;


struct Libro {
    char titulo[50];
    char autor[50];
    int anioEdicion;
    char editorial[50];
    char isbn[13];
    int numPaginas;
    int copias;
    bool prestado;
    Libro* siguiente; //puntero a un siguiente libro
};


struct Solicitud {
    char nombreLector[50];
    char dni[15];
    char tituloLibro[50];
    bool enEspera;
    Solicitud* siguiente;
};

// Función para cargar los libros desde "biblioteca.txt"
Libro* cargarLibrosDesdeArchivo() {
    ifstream archivo("biblioteca.txt");
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo biblioteca.txt para leer." << endl;
        return nullptr;
    }

    Libro* cabeza = nullptr;
    Libro* ultimo = nullptr;
    char linea[200];
    while (archivo.getline(linea, sizeof(linea))) {
        Libro* nuevo = new Libro;
        sscanf(linea, "%[^,],%[^,],%d,%[^,],%[^,],%d,%d,%d", nuevo->titulo, nuevo->autor, &nuevo->anioEdicion,
               nuevo->editorial, nuevo->isbn, &nuevo->numPaginas, &nuevo->copias, (int*)&nuevo->prestado);
        nuevo->siguiente = nullptr;

        if (cabeza == nullptr) cabeza = nuevo;
        else ultimo->siguiente = nuevo;

        ultimo = nuevo;
    }
    archivo.close();
    return cabeza;
}

// Guarda un libro en el archivo "biblioteca.txt" y todos los datos estan separados por comas
void guardarLibroEnArchivo(Libro* libro) {
    ofstream archivo("biblioteca.txt", ios::app);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo biblioteca.txt para escribir." << endl;
        return;
    }

    archivo << libro->titulo << "," << libro->autor << "," << libro->anioEdicion << ","
            << libro->editorial << "," << libro->isbn << "," << libro->numPaginas << ","
            << libro->copias << "," << libro->prestado << endl;

    archivo.close();
}

// guarda en "libros_prestados.txt" los libros prestados o los usuarios que estan en la lista de espera  de un libro
void guardarLibroPrestadoEnArchivo(Libro* libro, const Solicitud* solicitud) {
    ofstream archivo("libros_prestados.txt", ios::app);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo libros_prestados.txt para escribir." << endl;
        return;
    }

    archivo << libro->titulo << "," << libro->autor << "," << libro->anioEdicion << ","
            << libro->editorial << "," << libro->isbn << "," << libro->numPaginas << ","
            << solicitud->nombreLector << "," << solicitud->dni;

    if (solicitud->enEspera) archivo << ",EN_ESPERA";

    archivo << endl;
    archivo.close();
}

// Función para buscar un libro por su titulo título
Libro* buscarLibro(Libro* cabeza, const char* titulo) {
    while (cabeza != nullptr) {
        if (strcmp(cabeza->titulo, titulo) == 0) return cabeza;
        cabeza = cabeza->siguiente;
    }
    return nullptr;
}

// Muestra todos los libros de la biblioteca, recorre los nodos y imprime los datos hasta llegar al que tenga el puntero a null
void mostrarLibrosDisponibles(Libro* cabeza) {
    bool disponibles = false;
    while (cabeza != nullptr) {
        if (!cabeza->prestado) {
            cout << "Titulo: " << cabeza->titulo << ", Autor: " << cabeza->autor
                 << ", Anio: " << cabeza->anioEdicion << ", Copias: " << cabeza->copias << endl;
            disponibles = true;
        }
        cabeza = cabeza->siguiente;
    }
    if (!disponibles) cout << "No hay libros disponibles." << endl;
}

// Función para obtener un número entero positivo o 0
int obtenerOpcionMenu(const string& mensaje) {
    int valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor && valor >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        } else {
            cout << "Entrada no valida. Por favor, ingrese un numero entero positivo o 0 para salir." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Función para obtener caracteres 
void obtenerCadena(const string& mensaje, char* destino, int maxLen) {
    while (true) {
        cout << mensaje;
        cin.getline(destino, maxLen);
        if (strlen(destino) > 0) break;
        cout << "Entrada no valida. Ingrese una cadena no vacia." << endl;
    }
}

// Agrega un nuevo libro y guarda en  "biblioteca.txt"
Libro* agregarLibro(Libro* cabeza) {
    Libro* nuevo = new Libro;
    obtenerCadena("Ingrese el titulo del libro: ", nuevo->titulo, 50);
    obtenerCadena("Ingrese el autor del libro: ", nuevo->autor, 50);
    nuevo->anioEdicion = obtenerOpcionMenu("Ingrese el año de edicion: ");
    obtenerCadena("Ingrese la editorial del libro: ", nuevo->editorial, 50);
    obtenerCadena("Ingrese el ISBN del libro: ", nuevo->isbn, 13);
    nuevo->numPaginas = obtenerOpcionMenu("Ingrese el numero de paginas: ");
    nuevo->copias = obtenerOpcionMenu("Ingrese la cantidad de copias: ");
    nuevo->prestado = false;
    nuevo->siguiente = cabeza;
    
    guardarLibroEnArchivo(nuevo);
    
    cout << "Libro agregado exitosamente." << endl;
    return nuevo;
}

// pedir libro y añadir a lista de espera si ya esta en prestamo
void solicitarLibro(Libro* cabeza) { 
    Solicitud* nuevaSolicitud = new Solicitud;
    obtenerCadena("Ingrese el nombre del lector: ", nuevaSolicitud->nombreLector, 50);
    obtenerCadena("Ingrese el DNI del lector: ", nuevaSolicitud->dni, 15);
    obtenerCadena("Ingrese el titulo del libro: ", nuevaSolicitud->tituloLibro, 50);
    nuevaSolicitud->enEspera = false;

    Libro* libro = buscarLibro(cabeza, nuevaSolicitud->tituloLibro);
    if (!libro) {
        cout << "El libro no existe en la biblioteca." << endl;
    } else if (libro->prestado) {
        nuevaSolicitud->enEspera = true;
        cout << "El libro ya esta prestado. Se agregara a la lista de espera." << endl;
        guardarLibroPrestadoEnArchivo(libro, nuevaSolicitud);
    } else {
        libro->prestado = true;
        guardarLibroPrestadoEnArchivo(libro, nuevaSolicitud);
        cout << "Libro prestado exitosamente." << endl;
    }

    delete nuevaSolicitud;
}

// Devuelve el libro prestado y actualiza los archivos
void devolverLibro(Libro* cabeza) {
    char titulo[50];
    obtenerCadena("Ingrese el titulo del libro a devolver: ", titulo, 50);
    Libro* libro = buscarLibro(cabeza, titulo);

    if (!libro || !libro->prestado) {
        cout << "El libro no se encuentra en prestamo o no existe en la biblioteca." << endl;
        return;
    }

    libro->prestado = false;

    ifstream archivoLectura("libros_prestados.txt");
    ofstream archivoTemporal("libros_prestados_temp.txt");
    bool usuarioEnEsperaNotificado = false;

    if (!archivoLectura.is_open() || !archivoTemporal.is_open()) {
        cout << "Error al abrir archivos para actualizar prestamos." << endl;
        return;
    }

    char linea[200];
    while (archivoLectura.getline(linea, sizeof(linea))) {
        if (strstr(linea, titulo) == nullptr) {
            archivoTemporal << linea << endl;
        } else if (!usuarioEnEsperaNotificado && strstr(linea, "(EN_ESPERA)") != nullptr) {
            char nombreLector[50];
            char dni[15];
            sscanf(linea, "%*[^,],%*[^,],%*d,%*[^,],%*[^,],%*d,%[^,],%s", nombreLector, dni);
            cout << "El libro '" << titulo << "' ahora está disponible. Notificando a " << nombreLector 
                 << " (DNI: " << dni << ")..." << endl;
            usuarioEnEsperaNotificado = true;
        } else {
            archivoTemporal << linea << endl;
        }
    }

    archivoLectura.close();
    archivoTemporal.close();
    remove("libros_prestados.txt");
    rename("libros_prestados_temp.txt", "libros_prestados.txt"); //el txt temporal pasa a ser el principal y se borra el anterior

    cout << "Libro devuelto exitosamente." << endl;
}

// Borra el contenido de los txt
void limpiarArchivos() {
    ofstream archivoBiblioteca("biblioteca.txt", ios::trunc);
    ofstream archivoPrestamos("libros_prestados.txt", ios::trunc);

    if (archivoBiblioteca.is_open() && archivoPrestamos.is_open()) {
        cout << "Todos los archivos han sido limpiados." << endl;
    } else {
        cout << "Error al limpiar los archivos." << endl;
    }
}


int main() {
    Libro* biblioteca = cargarLibrosDesdeArchivo();
    int opcion;
    do {
        cout << "\nMenu de Biblioteca:\n";
        cout << "1. Agregar libro\n";
        cout << "2. Solicitar libro\n";
        cout << "3. Devolver libro\n";
        cout << "4. Mostrar libros disponibles\n";
        cout << "5. Limpiar archivos\n";
        cout << "0. Salir\n";
        opcion = obtenerOpcionMenu("Seleccione una opcion: ");

        switch (opcion) {
            case 1: biblioteca = agregarLibro(biblioteca); break;
            case 2: solicitarLibro(biblioteca); break;
            case 3: devolverLibro(biblioteca); break;
            case 4: mostrarLibrosDisponibles(biblioteca); break;
            case 5: limpiarArchivos(); break;
            case 0: cout << "Saliendo del sistema." << endl; break;
            default: cout << "Opcion no valida." << endl;
        }
    } while (opcion != 0);

    return 0;
}

