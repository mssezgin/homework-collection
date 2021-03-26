#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *mainseq;

int is_one_formula_on_both_sides(char *sequent) {

    char *l, *arrow, *r;
    int temp;

    arrow = strchr(sequent, '#');
    *arrow = ',';

    while (sequent < arrow) {
        r = arrow + 1;

        while (*r) {
            temp = 1, l = sequent;

            while (*l != ',') {
                if (*l++ != *r++) {
                    temp = 0;
                    break;
                }
            }

            if (temp) {
                *arrow = '#';
                return 1;
            }

            while (*r && *r++ != ',');
        }

        while (*sequent != ',') sequent++;
        sequent++;
    }

    *arrow = '#';

    return 0;
}

int are_all_individual(char *sequent) {

    if (*sequent != '#') sequent++;

    while (*sequent) {
        if (*sequent != ',' && *sequent != '#') return 0;
        sequent += 2;
    }

    return 1;
}

char *remove_implications(char *sequent) {

    char *s, *n, *new;
    char *left, *imp;
    int removed = 0, closed_para = 0;

    while ((imp = strchr(sequent, '>')) != NULL) {

        removed++;
        left = imp - 1;

        do {
            if (*left == '(') {
                if (closed_para) {
                    closed_para--;
                }
                else {
                    left++;
                    break;
                }
            }
            else if (*left == ')') {
                closed_para++;
            }
            left--;

        } while (1);

        if (*left == '-') {

            new = malloc(strlen(sequent) * sizeof(char));
            s = sequent, n = new;

            while (s < left) {
                *n = *s;
                n++, s++;
            }
            s++;
            while (s < imp) {
                *n = *s;
                n++, s++;
            }
            *n = '|';
            n++, s++;
            while ((*n++ = *s++));
        }
        else {

            new = malloc((strlen(sequent) + 2) * sizeof(char));
            s = sequent, n = new;

            while (s < left) {
                *n = *s;
                n++, s++;
            }
            *n = '-';
            n++;
            while (s < imp) {
                *n = *s;
                n++, s++;
            }
            *n = '|';
            n++, s++;
            while ((*n++ = *s++));
        }

        free(sequent);
        sequent = new;
    }

    return sequent;
}

char *remove_ands_on_left(char *sequent) {

    char *left, *mid, *arrow;
    char *s, *n, *new;
    int removed = 0, opened_para = 0;
    
    left = mid = sequent;
    arrow = strchr(sequent, '#');
    *arrow = ',';

    while (left < arrow) {

        if (*left == '(') {

            while (*++mid == '-');

            if (*mid == '(') {

                opened_para = 1;
                mid++;

                while (opened_para) {

                    if (*mid == '(') {
                        opened_para++;
                    }
                    else if (*mid == ')') {
                        opened_para--;
                    }
                    mid++;
                }
            }
            else {
                mid++;
            }

            if (*mid == '&') {
                removed++;
                *left = '.';
                *mid = ',';
                while (*++mid != ',');
                *(mid - 1) = '.';
                mid++;
                left = mid;
            }
            else {
                while (*++mid != ',');
                mid++;
                left = mid;
            }
        }
        else {
            while (*++mid != ',');
            mid++;
            left = mid;
        }
    }

    *arrow = '#';

    if (removed) {

        new = malloc((strlen(sequent) - 2 * removed + 1) * sizeof(char));
        s = sequent;
        n = new;

        while ((*n = *s++)) {
            if (*n != '.') n++;
        }

        free(sequent);
        sequent = new;
    }

    return sequent;
}

char *remove_ors_on_right(char *sequent) {

    char *left, *mid, *arrow;
    char *s, *n, *new;
    int removed = 0, opened_para = 0, length;

    arrow = strchr(sequent, '#');
    left = mid = arrow + 1;
    length = strlen(sequent);
    arrow = sequent + length;
    *arrow = ',';

    while (left < arrow) {

        if (*left == '(') {

            while (*++mid == '-');

            if (*mid == '(') {

                opened_para = 1;
                mid++;

                while (opened_para) {

                    if (*mid == '(') {
                        opened_para++;
                    }
                    else if (*mid == ')') {
                        opened_para--;
                    }
                    mid++;
                }
            }
            else {
                mid++;
            }

            if (*mid == '|') {
                removed++;
                *left = '.';
                *mid = ',';
                while (*++mid != ',');
                *(mid - 1) = '.';
                mid++;
                left = mid;
            }
            else {
                while (*++mid != ',');
                mid++;
                left = mid;
            }
        }
        else {
            while (*++mid != ',');
            mid++;
            left = mid;
        }
    }

    *arrow = '\0';

    if (removed) {

        new = malloc((length - 2 * removed + 1) * sizeof(char));
        s = sequent;
        n = new;

        while ((*n = *s++)) {
            if (*n != '.') n++;
        }

        free(sequent);
        sequent = new;
    }

    return sequent;
}

char *remove_negatives(char *sequent) {
    
    char *arrow, *end;
    char *s, *n, *new;
    char *lefts[200], *rights[200];
    int removed = 0, dashes, length;
    int l = 0, r = 0;

    s = sequent;
    length = strlen(sequent);
    arrow = strchr(sequent, '#');
    end = sequent + length;
    *arrow = ',';
    *end = ',';

    if (s == arrow) s++;

    while (s < arrow) {

        if (*s == '-') {
            
            dashes = 1;
            while (*++s == '-') dashes++;

            removed += dashes;

            if (dashes % 2) {
                rights[r++] = s;
            }
            else {
                lefts[l++] = s;
            }

            while (*s++ != ',');
            *(s - 1) = '\0';

        }
        else {

            lefts[l++] = s;
            while (*s++ != ',');
            *(s - 1) = '\0';
        }
    }

    while (s < end) {

        if (*s == '-') {
            
            dashes = 1;
            while (*++s == '-') dashes++;

            removed += dashes;

            if (dashes % 2) {
                lefts[l++] = s;
            }
            else {
                rights[r++] = s;
            }

            while (*s++ != ',');
            *(s - 1) = '\0';

        }
        else {

            rights[r++] = s;
            while (*s++ != ',');
            *(s - 1) = '\0';
        }
    }

    new = malloc((length - removed + 1) * sizeof(char));
    n = new;

    if (!l) n++;

    while (l--) {

        s = lefts[l];
        while ((*n++ = *s++));
        *(n - 1) = ',';
    }

    *(n - 1) = '#';

    if (!r) n++;

    while (r--) {

        s = rights[r];
        while ((*n++ = *s++));
        *(n - 1) = ',';
    }

    *(n - 1) = '\0';

    free(sequent);
    sequent = new;

    return sequent;
}

char **remove_ors_on_left(char *sequent) {

    char *left, *mid, *right, *arrow;
    char *s, *n0, *n1, **news;
    int opened_para = 0, length;
    
    length = strlen(sequent);
    left = mid = sequent;
    right = 0;
    arrow = strchr(sequent, '#');
    *arrow = ',';

    while (left < arrow) {

        if (*left == '(') {

            while (*++mid == '-');

            if (*mid == '(') {

                opened_para = 1;
                mid++;

                while (opened_para) {

                    if (*mid == '(') {
                        opened_para++;
                    }
                    else if (*mid == ')') {
                        opened_para--;
                    }
                    mid++;
                }
            }
            else {
                mid++;
            }

            if (*mid == '|') {
                right = mid;
                while (*++right != ',');
                right--;
                break;
            }
            else {
                while (*mid++ != ',');
                left = mid;
            }
        }
        else {
            while (*mid++ != ',');
            left = mid;
        }
    }

    *arrow = '#';

    news = calloc(2, sizeof(char *));

    if (right) {

        n0 = news[0] = malloc((length - (right - mid) - 1) * sizeof(char));
        n1 = news[1] = malloc((length - (mid - left) - 1) * sizeof(char));
        s = sequent;
        *left = *mid = *right = '\0';

        while ((*n0++ = *n1++ = *s++));
        n0--, n1--;
        while ((*n0++ = *s++));
        while ((*n1++ = *s++));
        n0--, n1--;
        while ((*n0++ = *n1++ = *s++));

        free(sequent);
    }
    else {
        news[0] = sequent;
    }

    return news;
}

char **remove_ands_on_right(char *sequent) {

    char *left, *mid, *right, *arrow;
    char *s, *n0, *n1, **news;
    int opened_para = 0, length;
    
    length = strlen(sequent);
    right = 0;
    arrow = strchr(sequent, '#');
    left = mid = arrow + 1;
    arrow = sequent + length;
    *arrow = ',';

    while (left < arrow) {

        if (*left == '(') {

            while (*++mid == '-');

            if (*mid == '(') {

                opened_para = 1;
                mid++;

                while (opened_para) {

                    if (*mid == '(') {
                        opened_para++;
                    }
                    else if (*mid == ')') {
                        opened_para--;
                    }
                    mid++;
                }
            }
            else {
                mid++;
            }

            if (*mid == '&') {
                right = mid;
                while (*++right != ',');
                right--;
                break;
            }
            else {
                while (*mid++ != ',');
                left = mid;
            }
        }
        else {
            while (*mid++ != ',');
            left = mid;
        }
    }

    *arrow = '\0';

    news = calloc(2, sizeof(char *));

    if (right) {

        n0 = news[0] = malloc((length - (right - mid) - 1) * sizeof(char));
        n1 = news[1] = malloc((length - (mid - left) - 1) * sizeof(char));
        s = sequent;
        *left = *mid = *right = '\0';

        while ((*n0++ = *n1++ = *s++));
        n0--, n1--;
        while ((*n0++ = *s++));
        while ((*n1++ = *s++));
        n0--, n1--;
        while ((*n0++ = *n1++ = *s++));

        free(sequent);
    }
    else {
        news[0] = sequent;
    }

    return news;
}

char evaluator(char *sequent) {

    char **subsequents, *subsequent0, *subsequent1;

    sequent = remove_implications(sequent);

    while (1) {

        if (is_one_formula_on_both_sides(sequent)) return 'T';

        if (are_all_individual(sequent)) return 'F';

        sequent = remove_negatives(sequent);
        sequent = remove_ands_on_left(sequent);
        sequent = remove_ors_on_right(sequent);

        subsequents = remove_ors_on_left(sequent);

        if (subsequents[1]) {

            subsequent0 = subsequents[0], subsequent1 = subsequents[1];
            free(subsequents);

            if (evaluator(subsequent0) == 'T' && evaluator(subsequent1) == 'T') return 'T';
            else return 'F';
        }
        else {
            free(subsequents);
        }

        subsequents = remove_ands_on_right(sequent);

        if (subsequents[1]) {

            subsequent0 = subsequents[0], subsequent1 = subsequents[1];
            free(subsequents);

            if (evaluator(subsequent0) == 'T' && evaluator(subsequent1) == 'T') return 'T';
            else return 'F';
        }
        else {
            free(subsequents);
        }
    }
}

int main () {

    char *m, c;

    m = mainseq = malloc(256 * sizeof(char));

    while ((c = getchar()) != EOF) *m++ = c;

    if (*(m - 1) == '\n') *(m - 1) = '\0';
    else *m++ = '\0';

    mainseq = realloc(mainseq, (m - mainseq) * sizeof(char));

    printf("%c", evaluator(mainseq));

    return 0;
}
