#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#define TYPE(p) ((p)->type)
#define CHAR(p) ((p)->value.character)
#define VARIABLE(p) ((p)->value.variable_index)
#define NUMBER(p) ((p)->value.number)
#define START(p) ((p)->start)
#define PROBABILITY(p) ((p)->probability)
#define COUNT(p) ((p)->count)
#define NUMBERTYPE 11
#define FUNCTIONTYPE 12
#define OPERATORTYPE 13
#define VARIABLETYPE 14
#define NEGATE 1
#define SIN 2
#define COS 3
#define SQRT 4
#define LN 5


union tokenvalue {
    char character;
    int variable_index;
    double number;
};

struct token {
    char type;
    union tokenvalue value;
};

struct interval_info {
    double start;
    double probability;
};

struct experiment_info {
    double start;
    long int count;
};

                                            /* number of allocated elements */
char *formula, *operator_stack;             /* 201 for each */
struct token *output_queue;                 /* 201 */
int count_of_intervals;
long int count_of_experiments;
int i = 1, *indexes_of_variables;           /* 30 (at least 26) for the latter */
struct interval_info **variable_data;       /* count of variables * count of intervals */
double *interval_gaps;                      /* count of variables */
double *current_random_values;              /* count of variables */
long int max_rand = (long) RAND_MAX + 1;
double *evaluation_stack;                   /* 201 */
double *results;                            /* count of experiments */
struct experiment_info *experiment_data;    /* count of intervals */


void enqueue(char c, struct token *q) {

    switch (c) {

        case '+':
            TYPE(q) = OPERATORTYPE;
            CHAR(q) = '+';
            break;
        
        case '-':
            TYPE(q) = OPERATORTYPE;
            CHAR(q) = '-';
            break;
        
        case '*':
            TYPE(q) = OPERATORTYPE;
            CHAR(q) = '*';
            break;
        
        case '/':
            TYPE(q) = OPERATORTYPE;
            CHAR(q) = '/';
            break;
        
        case '^':
            TYPE(q) = OPERATORTYPE;
            CHAR(q) = '^';
            break;
        
        case NEGATE:
            TYPE(q) = FUNCTIONTYPE;
            CHAR(q) = NEGATE;
            break;
        
        case SIN:
            TYPE(q) = FUNCTIONTYPE;
            CHAR(q) = SIN;
            break;
        
        case COS:
            TYPE(q) = FUNCTIONTYPE;
            CHAR(q) = COS;
            break;
        
        case SQRT:
            TYPE(q) = FUNCTIONTYPE;
            CHAR(q) = SQRT;
            break;
        
        case LN:
            TYPE(q) = FUNCTIONTYPE;
            CHAR(q) = LN;
            break;
    }
}


void shunting_yard() {

    char *f = formula, *s;
    struct token *q = output_queue;

    s = operator_stack = calloc(201, sizeof(char));
    if (operator_stack == NULL) {
        printf("\ncalloc failed! try again.\n");
        return;
    }

    while (*f) {
        
        switch (*f) {

            case ' ':
                f++;
                break;

            case '(':
                *s++ = *f++;
                break;

            case ')':
                while (*--s != '(') {
                    enqueue(*s, q);
                    *s = 0;
                    q++;
                }
                *s-- = 0;
                if (operator_stack <= s && *s <= LN && NEGATE <= *s) {
                    enqueue(*s, q);
                    *s-- = 0;
                    q++;
                }
                s++, f++;
                break;

            case '+':
                while (operator_stack != s && *(s - 1) != '(') {
                    enqueue(*--s, q);
                    *s = 0;
                    q++;
                }
                *s++ = *f++;
                break;

            case '-':
                while (operator_stack != s && *(s - 1) != '(') {
                    enqueue(*--s, q);
                    *s = 0;
                    q++;
                }
                *s++ = *f++;
                break;

            case '*':
                while (operator_stack != s && *(s - 1) != '(' && *(s - 1) != '+' && *(s - 1) != '-') {
                    enqueue(*--s, q);
                    *s = 0;
                    q++;
                }
                *s++ = *f++;
                break;

            case '/':
                while (operator_stack != s && *(s - 1) != '(' && *(s - 1) != '+' && *(s - 1) != '-') {
                    enqueue(*--s, q);
                    *s = 0;
                    q++;
                }
                *s++ = *f++;
                break;

            case '^':
                *s++ = *f++;
                break;
            
            case '~':
                *s++ = NEGATE;
                f++;
                break;
            
            case 's':
                if (*++f == 'i') {
                    *s++ = SIN;
                    f += 2;
                }
                else {
                    *s++ = SQRT;
                    f += 3;
                }
                break;
            
            case 'c':
                *s++ = COS;
                f += 3;
                break;
            
            case 'l':
                *s++ = LN;
                f += 2;
                break;

            default:
                if ('A' <= *f && *f <= 'Z') {
                    TYPE(q) = VARIABLETYPE;
                    if (indexes_of_variables[*f - 'A'])
                        VARIABLE(q) = indexes_of_variables[*f - 'A'];
                    else
                        VARIABLE(q) = indexes_of_variables[*f - 'A'] = i++;
                    q++, f++;
                }
                else {
                    TYPE(q) = NUMBERTYPE;
                    sscanf(f, "%lf", &(NUMBER(q)));
                    q++;
                    while (isdigit(*f) || *f == '.') f++;
                }
                break;
        }
    }

    while (operator_stack < s) {
        enqueue(*--s, q++);
    }

    free(operator_stack);
}


void randomization() {

    int j;
    double r;
    struct interval_info *ip;

    for (j = 1; j < i; j++) {

        r = (double) rand();
        r /= max_rand;
        for (ip = variable_data[j]; PROBABILITY(ip) <= r; ip++);

        r = (double) rand();
        r /= max_rand;
        current_random_values[j] = START(ip) + r * interval_gaps[j];
    }
}


double postfix_evaluator() {

    double *s = evaluation_stack - 1;
    struct token *q = output_queue;

    while (TYPE(q)) {

        switch (TYPE(q)) {

            case NUMBERTYPE:
                *++s = NUMBER(q++);
                break;

            case FUNCTIONTYPE:

                switch (CHAR(q++)) {
                    
                    case NEGATE:
                        *s *= -1;
                        break;

                    case SIN:
                        *s = sin(*s);
                        break;

                    case COS:
                        *s = cos(*s);
                        break;

                    case SQRT:
                        *s = sqrt(*s);
                        break;

                    case LN:
                        *s = log(*s);
                        break;
                }
                break;

            case OPERATORTYPE:
                s--;

                switch (CHAR(q++)) {

                    case '+':
                        *s += *(s + 1);
                        break;
                    
                    case '-':
                        *s -= *(s + 1);
                        break;
                    
                    case '*':
                        *s *= *(s + 1);
                        break;
                    
                    case '/':
                        *s /= *(s + 1);
                        break;
                    
                    case '^':
                        *s = pow(*s, *(s + 1));
                        break;
                }
                break;

            case VARIABLETYPE:
                *++s = current_random_values[VARIABLE(q++)];
                break;
        }
    }

    return *s;
}


int main() {

    char *f;
    int j, k;
    char tempvar;
    double lower, upper, tempdbl, *r;
    long int n;


    /* scanning formula from input */

    f = formula = calloc(201, sizeof(char));
    output_queue = calloc(201, sizeof(struct token));
    indexes_of_variables = calloc(30, sizeof(int));
    if (formula == NULL || output_queue == NULL || indexes_of_variables == NULL) {
        printf("\ncalloc failed! try again.\n");
        return 1;
    }

    do {
        scanf("%c", f);
    } while (*f++ != '\n');
    *(f - 1) = '\0';

    /*-------------------------*/



    /* shunting-yard algorithm */

    shunting_yard();

    /*-------------------------*/


    free(formula);


    /* scanning other data from input */

    scanf("%d %ld\n", &count_of_intervals, &count_of_experiments);

    variable_data = malloc(i * sizeof(struct interval_info *));
    interval_gaps = malloc(i * sizeof(double));
    if (variable_data == NULL || interval_gaps == NULL) {
        printf("\nmalloc failed! try again.\n");
        return 1;
    }

    for (j = 1; j < i; j++) {

        if (scanf("%c %lf %lf ", &tempvar, &lower, &upper) == 3) {

            struct interval_info *ip;

            ip = *(variable_data + indexes_of_variables[tempvar - 'A']) = malloc(count_of_intervals * sizeof(struct interval_info));
            if (ip == NULL) {
                printf("\nmalloc failed! try again.\n");
                return 1;
            }

            upper = *(interval_gaps + indexes_of_variables[tempvar - 'A']) = (upper - lower) / count_of_intervals;

            START(ip) = lower;
            scanf("%lf ", &(PROBABILITY(ip)));

            for (k = 1, ip++; k < count_of_intervals; k++, ip++) {
                START(ip) = lower + k * upper;
                scanf("%lf ", &(PROBABILITY(ip)));
                PROBABILITY(ip) += PROBABILITY(ip - 1);
            }
        }
    }

    /*-------------------------*/


    free(indexes_of_variables);


    /* doing experiments */

    current_random_values = malloc(i * sizeof(double));
    r = results = malloc(count_of_experiments * sizeof(double));
    evaluation_stack = malloc(201 * sizeof(double));
    if (current_random_values == NULL || results == NULL || evaluation_stack == NULL) {
        printf("\nmalloc failed! try again.\n");
        return 1;
    }

    srand(time(NULL));

    randomization();
    lower = upper = *r++ = postfix_evaluator();

    for (n = count_of_experiments - 1; n; n--, r++) { /* n geri geri gelmesin mi */

        randomization();
        tempdbl = *r = postfix_evaluator();
        if (tempdbl < lower) lower = tempdbl;
        else if (tempdbl > upper) upper = tempdbl;
    }

    /*-------------------------*/


    free(output_queue);
    for (j = 1; j < i; j++) free(*(variable_data + j));
    free(variable_data);
    free(interval_gaps);
    free(current_random_values);
    free(evaluation_stack);


    /* sorting results */

    experiment_data = malloc(count_of_intervals * sizeof(struct experiment_info));
    if (experiment_data == NULL) {
        printf("\nmalloc failed! try again.\n");
        return 1;
    }

    tempdbl = (upper - lower) / count_of_intervals;

    START(experiment_data) = lower;
    COUNT(experiment_data) = 0;
    for (k = 1; k < count_of_intervals; k++) {
        START(experiment_data + k) = lower + k * tempdbl;
        COUNT(experiment_data + k) = 0;
    }

    for (n = count_of_experiments, r = results; n; n--, r++) {

        struct experiment_info *e;        

        for (e = experiment_data + count_of_intervals - 1; *r < START(e); e--);
        COUNT(e)++;
    }

    /*-------------------------*/



    /* normalizing and printing */

    printf("%.3f %.3f", lower, upper);

    for (k = 0; k < count_of_intervals; k++) printf(" %.3f", (double) COUNT(experiment_data + k) / count_of_experiments);
    printf("\n");

    /*-------------------------*/


    free(results);
    free(experiment_data);


    return 0;
}
