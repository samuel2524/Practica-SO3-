
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_STACK 1024
#define MAX_LINE  2048
#define VIEW_SIZE 8

typedef struct {
    double values[MAX_STACK];
    int size;               // cantidad de elementos en la pila
} Stack;

/* ===== MANEJO DE PILA ===== */

static void init_stack(Stack *s) {
    s->size = 0;
}

static int push(Stack *s, double v) {
    if (s->size >= MAX_STACK) return 0;
    s->values[s->size++] = v;
    return 1;
}

static int pop(Stack *s, double *out) {
    if (s->size <= 0) return 0;
    *out = s->values[--s->size];
    return 1;
}

static int peek(const Stack *s, double *out) {
    if (s->size <= 0) return 0;
    *out = s->values[s->size - 1];
    return 1;
}

static void clear(Stack *s) {
    s->size = 0;
}

/* ===== VISUALIZACIÓN ===== */

static void show_stack(const Stack *s) {
    printf("\n====== ESTADO DE LA PILA ======\n");

    for (int pos = VIEW_SIZE; pos >= 1; pos--) {
        double val = 0.0;

        if (pos <= s->size) {
            val = s->values[s->size - pos];
        }

        printf("Slot %d -> %.6f\n", pos, val);
    }

    printf("================================\n");
}

/* ===== UTILIDADES ===== */

static int parse_value(const char *txt, double *out) {
    char *end;
    double v = strtod(txt, &end);
    if (end == txt || *end != '\0') return 0;
    *out = v;
    return 1;
}

/* ===== OPERACIONES ===== */

static void binary_op(Stack *s, char op) {
    double b, a;

    if (!pop(s, &b) || !pop(s, &a)) {
        printf("⚠️  Error: operandos insuficientes\n");
        return;
    }

    double r = 0.0;

    switch (op) {
        case '+': r = a + b; break;
        case '-': r = a - b; break;
        case '*': r = a * b; break;
        case '/':
            if (b == 0) {
                printf("⚠️  Error: división por cero\n");
                push(s, a);
                push(s, b);
                return;
            }
            r = a / b;
            break;
        default:
            push(s, a);
            push(s, b);
            return;
    }

    push(s, r);
    printf("✔ Resultado parcial: %g\n", r);
}

static void unary_op(Stack *s, const char *cmd) {
    double a;

    if (!pop(s, &a)) {
        printf("⚠️  Error: pila vacía\n");
        return;
    }

    double r = 0.0;

    if (strcmp(cmd, "sqrt") == 0) {
        if (a < 0) {
            printf("⚠️  Error: raíz negativa\n");
            push(s, a);
            return;
        }
        r = sqrt(a);
    }
    else {
        double rad = a * M_PI / 180.0;

        if (strcmp(cmd, "sin") == 0) r = sin(rad);
        else if (strcmp(cmd, "cos") == 0) r = cos(rad);
        else if (strcmp(cmd, "tan") == 0) r = tan(rad);
        else {
            push(s, a);
            return;
        }
    }

    push(s, r);
    printf("✔ Resultado parcial: %g\n", r);
}

static void power_op(Stack *s) {
    double exp, base;

    if (!pop(s, &exp) || !pop(s, &base)) {
        printf("⚠️  Error: pila insuficiente\n");
        return;
    }

    double r = pow(base, exp);
    push(s, r);
    printf("✔ Resultado parcial: %g\n", r);
}

/* ===== INTERFAZ ===== */

static void help(void) {
    printf("\n--- CALCULADORA RPN ---\n");
    printf("Ingrese números y operaciones en notación RPN\n\n");
    printf("Operadores:  +  -  *  /\n");
    printf("Funciones :  sqrt  sin  cos  tan  pow\n");
    printf("Trigonometría en GRADOS\n\n");
    printf("Comandos:\n");
    printf("  s  -> mostrar pila\n");
    printf("  p  -> ver tope\n");
    printf("  c  -> limpiar pila\n");
    printf("  h  -> ayuda\n");
    printf("  q  -> salir\n");
    printf("-----------------------\n");
}

/* ===== MAIN ===== */

int main(void) {
    Stack stack;
    init_stack(&stack);

    help();

    char line[MAX_LINE];

    while (1) {
        printf("\nRPN >>> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        line[strcspn(line, "\r\n")] = '\0';
        if (*line == '\0') continue;

        char *ctx;
        char *tk = strtok_r(line, " \t", &ctx);

        while (tk) {

            if (strcmp(tk, "q") == 0) return 0;
            else if (strcmp(tk, "h") == 0) help();
            else if (strcmp(tk, "c") == 0) {
                clear(&stack);
                printf("✔ Pila reiniciada\n");
            }
            else if (strcmp(tk, "s") == 0) show_stack(&stack);
            else if (strcmp(tk, "p") == 0) {
                double t;
                if (peek(&stack, &t)) printf("Tope actual: %g\n", t);
                else printf("Pila vacía\n");
            }
            else if (
                strcmp(tk, "sin") == 0 ||
                strcmp(tk, "cos") == 0 ||
                strcmp(tk, "tan") == 0 ||
                strcmp(tk, "sqrt") == 0
            ) {
                unary_op(&stack, tk);
            }
            else if (strcmp(tk, "pow") == 0) {
                power_op(&stack);
            }
            else if (strlen(tk) == 1 && strchr("+-*/", tk[0])) {
                binary_op(&stack, tk[0]);
            }
            else {
                double num;
                if (parse_value(tk, &num)) {
                    push(&stack, num);
                } else {
                    printf("⚠️  Entrada inválida: %s\n", tk);
                }
            }

            tk = strtok_r(NULL, " \t", &ctx);
        }
    }

    return 0;
}
