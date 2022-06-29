#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool is_operator(char operator) {
    return operator== '*' || operator== '/' || operator== '+' || operator== '-';
}

char currentOperand[10];

int depth = 0;
int lastOperand = 0;

typedef struct {
    int depth;
    int name;
    char value[10];
} operand;

operand operands[20];

void createOperand() {
    if (strlen(currentOperand) != 0) {
        operand op;
        op.name = lastOperand + 1;
        op.depth = depth;
        strcpy(op.value, currentOperand);

        operands[lastOperand] = op;

        memset(currentOperand, 0, sizeof currentOperand);

        lastOperand++;
    }
}

void createOperation(char operation) {
    operand op;
    op.name = lastOperand + 1;
    op.depth = depth;

    char operation_string[] = {operation};

    strcpy(op.value, operation_string);

    operands[lastOperand] = op;
    lastOperand++;
}

void createLink(FILE *fileptr, int name1, int name2) {
    fprintf(fileptr, "\tn%d -> n%d ;\n", name1, name2);
}

void createElement(FILE *fileptr, operand op) {
    fprintf(fileptr, "\tn%d ;\n\tn%d [label=\"%s\"] ;\n", op.name, op.name, op.value);
}

int main(int argc, char *argv[]) {
    char input[] = "(+ (356a) (4))\n";

    int current = 0;
    while (current < strlen(input)) {
        switch (input[current]) {
            case ' ':
                break;
            case '(':
                depth++;
                break;
            case ')':
                createOperand();
                depth--;
                break;
            default:
                if (is_operator(input[current])) {
                    createOperation(input[current]);
                } else {
                    strncat(currentOperand, &input[current], 1);
                }
        }

        current++;
    }

    FILE *file;
    file = fopen(argv[1], "w");

    if (file == NULL) {
        printf("Error while opening file\n\nUsage: ./prog [name of file outputted]\n\n");
    } else {
        fprintf(file, "%s", "digraph D {\n");

        int i = 0;
        bool ended = false;
        while (!ended && i < sizeof operands / 20) {
            if (operands[i].depth == 0) {
                ended = true;
                break;
            }

            createElement(file, operands[i]);
            // printf("Element name: n%d, depth: %d, value: %s\n", operands[i].name, operands[i].depth, operands[i].value);

            if (i > 0) {
                if (operands[i].depth == operands[i - 1].depth) {
                    createLink(file, operands[i - 2].name, operands[i].name);
                } else if (operands[i].depth > operands[i - 1].depth) {
                    createLink(file, operands[i - 1].name, operands[i].name);
                } else if (operands[i].depth < operands[i - 1].depth) {
                    for (int j = i - 1; j > 0; j--) {
                        if (operands[j].depth == operands[i].depth) {
                            createLink(file, operands[j - 1].name, operands[i].name);
                            break;
                        }
                    }
                }
            }

            i++;
        }

        fprintf(file, "%s", "}");
        fclose(file);
    }
}
