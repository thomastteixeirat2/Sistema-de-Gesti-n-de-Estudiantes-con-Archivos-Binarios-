#include <stdio.h>
#include <string.h>
// -----------------------------
// Definiciones y utilitarios
// -----------------------------
typedef struct {
    int legajo;
    char nombre[40];
    float promedio;
    int activo; // 1=activo, 0=baja lógica
} Estudiante;
#define ARCHIVO_DATOS "data/estudiantes.dat"
// Trim del '\n' final de fgets
void trim_newline(char *s) {
    size_t n = strlen(s);
    if (n > 0 && s[n-1] == '\n') s[n-1] = '\0';
}
// Lee cadena segura
void leerCadena(const char *prompt, char *dest, size_t tam) {
    printf("%s", prompt);
    if (fgets(dest, (int)tam, stdin) != NULL) {
        trim_newline(dest);
    } else {
        // Si falla, deja string vacía
        if (tam > 0) dest[0] = '\0';
    }
}
// Limpia el salto de línea pendiente luego de scanf
void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}
// Crear archivo si no existe (modo binario)
void asegurarArchivo() {
    FILE *f = fopen(ARCHIVO_DATOS, "ab+");
    if (f) fclose(f);
}
// -----------------------------
// Operaciones
// -----------------------------
void registrarEstudiante() {
    FILE *f = fopen(ARCHIVO_DATOS, "ab+");
    if (!f) { printf("Error abriendo archivo.\n"); return; }
    Estudiante e;
    printf("Legajo: ");
    scanf("%d", &e.legajo);
    limpiarBuffer();
    leerCadena("Nombre y apellido: ", e.nombre, sizeof(e.nombre));
    printf("Promedio (0.00 - 10.00): ");
    scanf("%f", &e.promedio);
    limpiarBuffer();
    e.activo = 1;
    size_t escritos = fwrite(&e, sizeof(Estudiante), 1, f);
    if (escritos == 1) {
        printf("Estudiante registrado con exito.\n");
    } else {
        printf("Error al escribir en el archivo.\n");
    }
    fclose(f);
}
void listarEstudiantes() {
    FILE *f = fopen(ARCHIVO_DATOS, "rb");
    if (!f) { printf("Error abriendo archivo.\n"); return; }
    Estudiante e;
    int contador = 0;

    printf("\n--- LISTADO DE ESTUDIANTES ACTIVOS ---\n");
    while (fread(&e, sizeof(Estudiante), 1, f) == 1) {
        if (e.activo == 1) {
            printf("Legajo: %d | Nombre: %s | Promedio: %.2f\n", e.legajo, e.nombre, e.promedio);
            contador++;
        }
    }
    if (contador == 0) printf("(sin estudiantes activos)\n");
    fclose(f);
}
void buscarPorLegajo() {
    FILE *f = fopen(ARCHIVO_DATOS, "rb");
    if (!f) { printf("Error abriendo archivo.\n"); return; }
    int legajo;
    printf("Ingrese legajo a buscar: ");
    scanf("%d", &legajo);
    limpiarBuffer();
    Estudiante e;
    int encontrado = 0;
    while (fread(&e, sizeof(Estudiante), 1, f) == 1) {
        if (e.legajo == legajo && e.activo == 1) {
            printf("ENCONTRADO -> Legajo: %d | Nombre: %s | Promedio: %.2f\n",
            e.legajo, e.nombre, e.promedio);
            encontrado = 1;
            break;
        }
    }
    if (!encontrado) printf("No se encontro estudiante activo con legajo %d.\n", legajo);
    fclose(f);
}
// Opcional recomendado: modificar registro
void modificarEstudiante() {
    FILE *f = fopen(ARCHIVO_DATOS, "rb+");
    if (!f) { printf("Error abriendo archivo.\n"); return; }
    int legajo;
    printf("Ingrese legajo a modificar: ");
    scanf("%d", &legajo);
    limpiarBuffer();
    Estudiante e;
    long pos;
    int modificado = 0;
    while ((pos = ftell(f)) >= 0 && fread(&e, sizeof(Estudiante), 1, f) == 1) {
        if (e.legajo == legajo && e.activo == 1) {
            printf("Modificar (dejar vacio para no cambiar):\n");
            char buffer[40];
            leerCadena("Nuevo nombre y apellido: ", buffer, sizeof(buffer));
            if (strlen(buffer) > 0) strncpy(e.nombre, buffer, sizeof(e.nombre));
            printf("Nuevo promedio (-1 para no cambiar): ");
            float p;
            if (scanf("%f", &p) == 1 && p >= 0.0f) {
                e.promedio = p;
            }
            limpiarBuffer();
            // volver al inicio del registro y sobrescribir
            fseek(f, pos, SEEK_SET);
            size_t escritos = fwrite(&e, sizeof(Estudiante), 1, f);
            if (escritos == 1) {
                printf("Registro modificado.\n");
                modificado = 1;
            } else {
                printf("Error al sobrescribir registro.\n");
            }
            break;
        }
    }
    if (!modificado) printf("No se encontro estudiante activo con legajo %d.\n", legajo);
    fclose(f);
}
// Opcional recomendado: baja lógica
void bajaLogica() {
    FILE *f = fopen(ARCHIVO_DATOS, "rb+");
    if (!f) { printf("Error abriendo archivo.\n"); return; }
    int legajo;
    printf("Ingrese legajo a dar de baja: ");
    scanf("%d", &legajo);
    limpiarBuffer();
    Estudiante e;
    long pos;
    int bajado = 0;
    while ((pos = ftell(f)) >= 0 && fread(&e, sizeof(Estudiante), 1, f) == 1) {
        if (e.legajo == legajo && e.activo == 1) {
            e.activo = 0;
            fseek(f, pos, SEEK_SET);
            if (fwrite(&e, sizeof(Estudiante), 1, f) == 1) {
                printf("Baja logica realizada.\n");
                bajado = 1;
            } else {
                printf("Error al actualizar registro.\n");
            }
            break;
        }
    }
    if (!bajado) printf("No se encontro estudiante activo con legajo %d.\n", legajo);
    fclose(f);
}

// -----------------------------
// Menú principal
// -----------------------------
int main() {
    asegurarArchivo();
    int opcion;
    do {
        printf("\n=== SISTEMA DE GESTION DE ESTUDIANTES ===\n");
        printf("1) Registrar estudiante\n");
        printf("2) Buscar por legajo\n");
        printf("3) Listar estudiantes\n");
        printf("4) Modificar estudiante (opcional)\n");
        printf("5) Baja logica (opcional)\n");
        printf("0) Salir\n");
        printf("Seleccione una opcion: ");
        if (scanf("%d", &opcion) != 1) {
            limpiarBuffer();
            opcion = -1;
        }
        limpiarBuffer();
        switch (opcion) {
            case 1: registrarEstudiante(); break;
            case 2: buscarPorLegajo(); break;
            case 3: listarEstudiantes(); break;
            case 4: modificarEstudiante(); break;
            case 5: bajaLogica(); break;
            case 0: printf("Fin del programa.\n"); break;
            default: printf("Opcion invalida.\n"); break;
        }
    } while (opcion != 0);
    return 0;
}
