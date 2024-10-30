import os

class Libro:
    def __init__(self, titulo, autor, anio_edicion, editorial, isbn, num_paginas, copias, prestado=False):
        self.titulo = titulo
        self.autor = autor
        self.anio_edicion = anio_edicion
        self.editorial = editorial
        self.isbn = isbn
        self.num_paginas = num_paginas
        self.copias = copias
        self.prestado = prestado
        self.siguiente = None

class Solicitud:
    def __init__(self, nombre_lector, dni, titulo_libro, en_espera=False):
        self.nombre_lector = nombre_lector
        self.dni = dni
        self.titulo_libro = titulo_libro
        self.en_espera = en_espera
        self.siguiente = None

# Cargar libros desde el archivo
def cargar_libros_desde_archivo():
    if not os.path.exists("biblioteca.txt"):
        print("No se pudo abrir el archivo biblioteca.txt para leer.")
        return None

    cabeza = None
    ultimo = None
    with open("biblioteca.txt", "r") as archivo:
        for linea in archivo:
            datos = linea.strip().split(',')
            nuevo = Libro(
                titulo=datos[0],
                autor=datos[1],
                anio_edicion=int(datos[2]),
                editorial=datos[3],
                isbn=datos[4],
                num_paginas=int(datos[5]),
                copias=int(datos[6]),
                prestado=bool(int(datos[7]))
            )

            if cabeza is None:
                cabeza = nuevo
            else:
                ultimo.siguiente = nuevo

            ultimo = nuevo

    return cabeza

# Guardar un libro en el archivo
def guardar_libro_en_archivo(libro):
    with open("biblioteca.txt", "a") as archivo:
        archivo.write(f"{libro.titulo},{libro.autor},{libro.anio_edicion},"
                      f"{libro.editorial},{libro.isbn},{libro.num_paginas},"
                      f"{libro.copias},{int(libro.prestado)}\n")

# Guardar libros prestados o usuarios en espera en el archivo
def guardar_libro_prestado_en_archivo(libro, solicitud):
    with open("libros_prestados.txt", "a") as archivo:
        archivo.write(f"{libro.titulo},{libro.autor},{libro.anio_edicion},"
                      f"{libro.editorial},{libro.isbn},{libro.num_paginas},"
                      f"{solicitud.nombre_lector},{solicitud.dni}")
        
        if solicitud.en_espera:
            archivo.write(",EN_ESPERA")
        
        archivo.write("\n")

# Buscar un libro por título
def buscar_libro(cabeza, titulo):
    while cabeza is not None:
        if cabeza.titulo == titulo:
            return cabeza
        cabeza = cabeza.siguiente
    return None

# Mostrar libros disponibles
def mostrar_libros_disponibles(cabeza):
    disponibles = False
    while cabeza is not None:
        if not cabeza.prestado:
            print(f"Titulo: {cabeza.titulo}, Autor: {cabeza.autor}, "
                  f"Anio: {cabeza.anio_edicion}, Copias: {cabeza.copias}")
            disponibles = True
        cabeza = cabeza.siguiente
    if not disponibles:
        print("No hay libros disponibles.")

# Obtener un número entero positivo
def obtener_opcion_menu(mensaje):
    while True:
        try:
            valor = int(input(mensaje))
            if valor >= 0:
                return valor
            else:
                print("Por favor, ingrese un número entero positivo o 0 para salir.")
        except ValueError:
            print("Entrada no válida. Por favor, ingrese un número entero.")

# Obtener una cadena no vacía
def obtener_cadena(mensaje, max_len):
    while True:
        entrada = input(mensaje).strip()
        if len(entrada) > 0 and len(entrada) <= max_len:
            return entrada
        print("Entrada no válida. Ingrese una cadena no vacía.")

# Agregar un nuevo libro y actualizar el archivo
def agregar_libro(cabeza):
    nuevo = Libro(
        titulo=obtener_cadena("Ingrese el título del libro: ", 50),
        autor=obtener_cadena("Ingrese el autor del libro: ", 50),
        anio_edicion=obtener_opcion_menu("Ingrese el año de edición: "),
        editorial=obtener_cadena("Ingrese la editorial del libro: ", 50),
        isbn=obtener_cadena("Ingrese el ISBN del libro: ", 13),
        num_paginas=obtener_opcion_menu("Ingrese el número de páginas: "),
        copias=obtener_opcion_menu("Ingrese la cantidad de copias: "),
        prestado=False
    )
    nuevo.siguiente = cabeza
    guardar_libro_en_archivo(nuevo)
    print("Libro agregado exitosamente.")
    return nuevo

# Solicitar libro y añadir a lista de espera si no está disponible
def solicitar_libro(cabeza):
    solicitud = Solicitud(
        nombre_lector=obtener_cadena("Ingrese el nombre del lector: ", 50),
        dni=obtener_cadena("Ingrese el DNI del lector: ", 15),
        titulo_libro=obtener_cadena("Ingrese el título del libro: ", 50)
    )

    libro = buscar_libro(cabeza, solicitud.titulo_libro)
    if libro is None:
        print("El libro no existe en la biblioteca.")
    elif libro.prestado:
        solicitud.en_espera = True
        print("El libro ya está prestado. Se agregará a la lista de espera.")
        guardar_libro_prestado_en_archivo(libro, solicitud)
    else:
        libro.prestado = True
        guardar_libro_prestado_en_archivo(libro, solicitud)
        print("Libro prestado exitosamente.")

# Devolver el libro prestado y actualizar los archivos
def devolver_libro(cabeza):
    titulo = obtener_cadena("Ingrese el título del libro a devolver: ", 50)
    libro = buscar_libro(cabeza, titulo)

    if libro is None or not libro.prestado:
        print("El libro no se encuentra en préstamo o no existe en la biblioteca.")
        return

    libro.prestado = False

    with open("libros_prestados.txt", "r") as archivo_lectura, open("libros_prestados_temp.txt", "w") as archivo_temporal:
        usuario_en_espera_notificado = False

        for linea in archivo_lectura:
            if titulo not in linea:
                archivo_temporal.write(linea)
            elif not usuario_en_espera_notificado and "(EN_ESPERA)" in linea:
                datos = linea.strip().split(',')
                nombre_lector, dni = datos[6], datos[7]
                print(f"El libro '{titulo}' ahora está disponible. Notificando a {nombre_lector} (DNI: {dni})...")
                usuario_en_espera_notificado = True
            else:
                archivo_temporal.write(linea)

    os.remove("libros_prestados.txt")
    os.rename("libros_prestados_temp.txt", "libros_prestados.txt")
    print("Libro devuelto exitosamente.")

# Limpiar archivos
def limpiar_archivos():
    with open("biblioteca.txt", "w"), open("libros_prestados.txt", "w"):
        pass
    print("Todos los archivos han sido limpiados.")

# Función principal con menú de opciones
def main():
    biblioteca = cargar_libros_desde_archivo()
    while True:
        print("\nMenu de Biblioteca:")
        print("1. Agregar libro")
        print("2. Solicitar libro")
        print("3. Devolver libro")
        print("4. Mostrar libros disponibles")
        print("5. Limpiar archivos")
        print("0. Salir")

        opcion = obtener_opcion_menu("Seleccione una opción: ")
        if opcion == 1:
            biblioteca = agregar_libro(biblioteca)
        elif opcion == 2:
            solicitar_libro(biblioteca)
        elif opcion == 3:
            devolver_libro(biblioteca)
        elif opcion == 4:
            mostrar_libros_disponibles(biblioteca)
        elif opcion == 5:
            limpiar_archivos()
        elif opcion == 0:
            print("Saliendo del sistema.")
            break
        else:
            print("Opción no válida.")

if __name__ == "__main__":
    main()
