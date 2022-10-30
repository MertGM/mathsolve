#include <stdio.h>
#include <stdlib.h>


#define Token_Multiply   42
#define Token_Plus       43
#define Token_Minus      45
#define Token_Division   47

static int mathTokens[] = {Token_Multiply, Token_Plus, 0, Token_Minus, 0, Token_Division};
#define MathTokensLength 6


#define Token_Precedence_Division 3
#define Token_Precedence_Multiply 2
#define Token_Precedence_Plus     1
#define Token_Precedence_Minus    1

static int Tokens_Precedence[] = {Token_Precedence_Multiply, Token_Precedence_Plus, 0, Token_Precedence_Minus, 0, Token_Precedence_Division};


typedef struct tree {
    char *value;
    struct tree *left;
    struct tree *right;
} tree;


// Note: could also use built-in strtoi function

int stoi(char *input) {
    int output = 0;
    int digit = 0;
    int sign = 1;
    for (char *tmp = input; *tmp != '\0'; ++tmp) {
        digit = (int)(*tmp);
        if (digit >= 48 && digit <= 57) {
            output += digit%48;
            output *= 10;
        } else if (digit == Token_Minus) {
            sign = -1;
        } else {
            break;
        }
    }
    // Remove trailing 0.
    output /= 10 * sign;

    return output;
}


char * getline(int n) {
    int MAX_STRING_LENGTH = 201; // Null terminator character included.
    if (n == 0) {
        n = MAX_STRING_LENGTH;
    } else if (n > MAX_STRING_LENGTH) {
        printf("Input length exceeds string length of %i\n", MAX_STRING_LENGTH);
        return NULL;
    }

    char *string = malloc(n);
    if (string == NULL) {
        printf("Not enough memory.\n");
        return NULL;
    }

    int i = 0;
    char c;

    while ((c = getchar()) != '\n') {
        string[i] = c;
        if (i++ > n) {
            printf("Exceeded maximum string length of %i.\n", n-1);
            return NULL;
        }
    }

    string[i] = '\0';
    return string;
}


int isMathOperator(char character) {
    int index = (int)character - Token_Multiply;
    if (index >= 0 && index < MathTokensLength) {
        if (mathTokens[index]) {
            return 1;
        }

        // Not a math operator character.
        return 0;
    }

    return 0;
}

int isDecimal(int value) {
    return value >= 48 && value <= 57;
}

char * MultiDigitStrCopy(char *string, char *dest, int count) {
    int index = 0;
    if (dest == NULL) {
        dest = malloc(21);
        if (dest == NULL) {
            printf("Not enough memory.\n");
            return 0;
        }
    }
    while (isDecimal((int)(*string)) && index < count) {
        if (*string == '\0') {
            break;
        }
        *dest++ = *string++;
        index++;
    }
    *dest = '\0';
    return string;
}


float Add(float a, float b) {
    return a + b;
}

float Subtract(float a, float b) {
    return a - b;
}

float Multiply(float a, float b) {
    return a * b;
}

float Divide(float a, float b) {
    if (b == 0) {
        printf("Math error: Division by zero.\n");
        return 0;
    }
    return a / b;
}


typedef float (*MathOperation)(float, float);
MathOperation add = Add;
MathOperation subtract = Subtract;
MathOperation multiply = Multiply;
MathOperation divide = Divide;

float MathEval(MathOperation MathOperations[], char *op, float a, float b) {
    int opvalue = ((int)op[0]) % Token_Multiply;
    if (op[1] ==  '*') {
        // @Future pow operator '**'
        opvalue += MathTokensLength;
    }

    return MathOperations[opvalue](a, b);
}


int SeekTokenOperator(char *string, tree *node) {
    int i = 0;
    while (*string != '\0') {
        if (isMathOperator(*string)) {
            tree *n = malloc(sizeof(tree));
            if (n == NULL) {
                printf("Not enough memory.\n");
                return 0;
            }
            n->value = malloc(2);
            if (n->value == NULL) {
                printf("Not enough memory.\n");
                return 0;
            }
            n->value[0] = *string;
            n->value[1] = '\0';
            n->left = NULL;
            n->right = NULL;
            node->right = n;
            return i+1;
        } else if (*string == ' ') {
            string++;
            i++;
            continue;
        } else {
            // Found unexpected symbol, despite that, parse expression that is valid on return.
            printf("Parse warning: Found unexpected symbol, subexpression will get ignored.\n");
            return 0;
        }
    }
    // Found no operator chaining a sub-expression.
    return 0;
}

int ParseExpression(char *string, tree *node) {
    while (*string != '\0') {
        if (*string == ' ') {
            string++;
        } else if (isMathOperator(*string)) {
            if (node->left == NULL) {
                // Negative sign
                if ((int)(*string) == Token_Minus && isDecimal((int)(*(string+1)))) {
                    tree *n  = malloc(sizeof(tree));
                    if (n == NULL) {
                        printf("Not enough memory.\n");
                        return 0;
                    }
                    n->left = NULL;
                    n->right = NULL;
                    n->value = malloc(22);
                    if (n->value == NULL) {
                        printf("Not enough memory.\n");
                        return 0;
                    }
                    n->value[0] = *string++;
                    char *movedString = MultiDigitStrCopy(string, n->value+1, 20);
                    string = movedString;
                    node->left = n;
                } else {
                    printf("Parse error: expected 'number', instead got 'operator'.\n");
                    return 0;
                }
            } else if (node->value == NULL) {
                node->value = malloc(2);
                if (node->value == NULL) {
                    printf("Not enough memory.\n");
                    return 0;
                }
                node->value[0] = *string++;
                node->value[1] = '\0';
            } else if (node->right == NULL) {
                // Negative sign
                if ((int)(*string) == Token_Minus && isDecimal((int)(*(string+1)))) {
                    tree *n  = malloc(sizeof(tree));
                    if (n == NULL) {
                        printf("Not enough memory.\n");
                        return 0;
                    }
                    n->left = NULL;
                    n->right = NULL;
                    n->value = malloc(22);
                    if (n->value == NULL) {
                        printf("Not enough memory.\n");
                        return 0;
                    }
                    n->value[0] = *string++;
                    char *movedString = MultiDigitStrCopy(string, n->value+1, 20);
                    string = movedString;

                    // Check for subsequent operator.
                    int movedIndex = 0;
                    if (movedIndex = SeekTokenOperator(string, node)) {
                        string += movedIndex;
                        node->right->left = n;
                        node = node->right;
                    } else {
                        node->right = n;
                        return 1;
                    }
                } else {
                    printf("Parse error: Expected 'number' at the start of expression, instead got 'operator'.\n");
                    return 0;
                }
            }
        } else if (isDecimal((int)(*string))) {
            if (node->left == NULL) {
                tree *n = malloc(sizeof(tree));
                if (n == NULL) {
                    printf("Not enough memory.\n");
                    return 0;
                }
                // For now, 20-digit numbers are the maximum, and also unused memory does not get freed or reallocated.
                n->value = malloc(21);
                if (n->value == NULL) {
                    printf("Not enough memory.\n");
                    return 0;
                }
                n->left = NULL;
                n->right = NULL;
                char *movedString = MultiDigitStrCopy(string, n->value, 20);
                node->left = n;
                string = movedString;
            } else if (node->value == NULL) {
                printf("Expected 'number operator', instead got 'number number.'\n");
                return 0;
            } else if (node->right == NULL) {
                tree *n = malloc(sizeof(tree));
                if (n == NULL) {
                    printf("Not enough memory.\n");
                    return 0;
                }
                n->value = malloc(21);
                if (n->value == NULL) {
                    printf("Not enough memory.\n");
                    return 0;
                }
                n->left = NULL;
                n->right = NULL;
                char *movedString = MultiDigitStrCopy(string, n->value, 20);
                string = movedString;

                // Check for subsequent operator.
                int movedIndex = 0;
                if (movedIndex = SeekTokenOperator(string, node)) {
                    string += movedIndex;
                    node->right->left = n;
                    node = node->right;
                } else {
                    node->right = n;
                    return 1;
                }
            }
        } else {
            printf("Received an invalid symbol, only numeric and alphabetic characters are allowed.\n");
            return 0;
        }
    }

    // Trailing operator found, subexpression will get removed.
    if (node->right == NULL && node->value != NULL) {
        free(node->value);
        node->value = node->left->value;
        return 1;
    }

    return 0;
}


int hasPrecedence(char a, char b) {
    return Tokens_Precedence[((int)a) - Token_Multiply] > Tokens_Precedence[((int)b) - Token_Multiply];
}

int FixupTree(tree *node) {
    if (node->right != NULL) {
        if (node->right->right != NULL) {
            if (hasPrecedence(node->value[0], node->right->value[0])) {
                tree *tmp = node->left;
                node->left = malloc(sizeof(tree));
                if (node->left == NULL) {
                    printf("Not enough memory.\n");
                    return 0;
                }
                node->left->value = node->value;
                node->left->left = tmp;
                node->left->right = node->right->left;
                node->right->left = NULL;
                node->value = node->right->value;
                node->right = node->right->right;
            }
            FixupTree(node->right);
        }
    }

    return 1;
}

void AdvanceTree(tree **node, int advance) {
    for (int i = 0; i < advance; i++) {
        *node = (*node)->right;
    }
}

float EvalInner(tree *node, MathOperation MathOperations[], int *evalAhead) {
    if (isDecimal((int)(node->value[0])) || isDecimal((int)(node->value[1]))) {
        return stoi(node->value);
    }
    (*evalAhead)++;
    if (!isDecimal((int)node->left->value[0]) && !isDecimal((int)node->left->value[1])) {
        // left node has operator, evalAhead will get incremented twice, 
        // so decrement once to prevent counting the inner binary operations.
        (*evalAhead)--;
    }

    if (!isDecimal((int)node->right->value[0]) && !isDecimal((int)node->right->value[1])) {
        if (hasPrecedence(node->right->value[0], node->value[0])) {
            return EvalInner(node->left, MathOperations, evalAhead);
        }
    }

    return MathEval(MathOperations, node->value, EvalInner((node->left), MathOperations, evalAhead),
                                                 EvalInner((node->right), MathOperations, evalAhead));
}

float Eval(tree *node) {
    MathOperation MathOperations[] = {multiply, add, NULL, subtract, NULL, divide};

    int evalAhead = 0;
    // left, starting value
    float acc = EvalInner(node->left, MathOperations, &evalAhead);
    // Don't advance, only get starting value.
    evalAhead = 0;

    while (node->right) {
        if (node->right) {
            if (node->right->right) {
                if (!hasPrecedence(node->right->value[0], node->value[0])) {
                    // Parse left to right.
                    acc = MathEval(MathOperations, node->value, acc, EvalInner(node->right->left,
                                                                     MathOperations, &evalAhead));
                    // node->right->left could be a decimal and thus not increment evalAhead,
                    // despite that parsing left to right always advances by 1, so passing constant 1 is also fine.
                    AdvanceTree(&node, 1);
                    evalAhead = 0;
                    continue;
                } else {
                    // Parse right precedence first.
                    acc = MathEval(MathOperations, node->value, acc, EvalInner(node->right,
                                                                     MathOperations, &evalAhead));
                    // Advance evalAhead + 1 because we eval the current node value binary operation aswell.
                    AdvanceTree(&node, evalAhead+1);
                    evalAhead = 0;
                    continue;
                }
            }
        }

        acc = MathEval(MathOperations, node->value, acc, EvalInner(node->right, MathOperations, &evalAhead));
        // evalAhead increment doesn't work for last or the only first binary operation, 
        // because node->right has no operator, only a decimal value, so instead manually advance by 1
        AdvanceTree(&node, 1);
    }

    return acc;
}

float Parse(char *string, int *error) {
    tree node;
    node.value = NULL;
    node.left = NULL;
    node.right = NULL;

    if (ParseExpression(string, &node)) {
        if (FixupTree(&node)) {
            float output = Eval(&node);
            if (error != NULL) {
                *error = 0;
            }
            return output;
        }
    }

    // Parsing failed.
    if (error != NULL) {
        *error = 1;
    }
    return 0;
}


int main(char argc, char **argv) {
    char *input;
    // Get expression via command-line argument.
    // Note: if your expression contains white spaces, pass it as a string.
    if (argc >= 2) {
        input = argv[1];
    } else {
        printf("Math expression: ");
        input = getline(0);
    }
    if (input == NULL || *input == '\0') {
        printf("input is void.\n");
        return 0;
    }

    int parseError = 0;
    float result = Parse(input, &parseError);
    if (!parseError) {
        printf("Result: %f\n", result);
    } else {
        printf("Parsing expression failed.\n");
    }

    return 0;
}
