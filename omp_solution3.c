#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define ROW_COUNT 1000
#define COL_COUNT 1000
#define MAX_PAL_SIZE 6

char plnd_matrix[ROW_COUNT][COL_COUNT];

void plndr_search(int thrd_count);

bool chk_plndrm(char* str, int len) {
    int begin = 0;
    int end = len - 1;
    while (begin < end) {
        if (str[begin] != str[end]) {
            return false;
        }
        begin++;
        end--;
    }
    return true;
}

int main() {
    srand(time(NULL));

    for (int i = 0; i < ROW_COUNT; i++) {
        for (int j = 0; j < COL_COUNT; j++) {
            plnd_matrix[i][j] = (rand() % 26) + 'a'; 
        }
    }

    for (int thrd_count = 1; thrd_count <= 8; thrd_count++) {
        plndr_search(thrd_count);
    }

    return 0;
}

void plndr_search(int thrd_count) {
    double start_time = omp_get_wtime();
    int palindrome_count[MAX_PAL_SIZE + 1] = {0};
    double time_taken[MAX_PAL_SIZE + 1] = {0};

    #pragma omp parallel for num_threads(thrd_count) collapse(2)
    for (int i = 0; i < ROW_COUNT; i++) {
        for (int j = 0; j < COL_COUNT; j++) {
            // Horizontal Search
            for (int k = 0; k <= MAX_PAL_SIZE; k++) {
                if (j + k >= COL_COUNT) break;
                char sub_str[MAX_PAL_SIZE + 1];
                strncpy(sub_str, &plnd_matrix[i][j], k + 1);
                sub_str[k + 1] = '\0';
                double st = omp_get_wtime();
                if (chk_plndrm(sub_str, k + 1)) {
                    #pragma omp atomic
                    palindrome_count[k]++;
                    double et = omp_get_wtime();
                    time_taken[k] += et - st;
                }
            }

            // Vertical Search
            for (int k = 0; k <= MAX_PAL_SIZE; k++) {
                if (i + k >= ROW_COUNT) break;
                char sub_str[MAX_PAL_SIZE + 1];
                for (int l = 0; l <= k; l++) {
                    sub_str[l] = plnd_matrix[i + l][j];
                }
                sub_str[k + 1] = '\0';
                double st = omp_get_wtime();
                if (chk_plndrm(sub_str, k + 1)) {
                    #pragma omp atomic
                    palindrome_count[k]++;
                    double et = omp_get_wtime();
                    time_taken[k] += et - st;
                }
            }
            
        }
    }

    double end_time = omp_get_wtime();

    for (int i = 3; i <= MAX_PAL_SIZE; i++) {
        printf("%d palindromes of size %d found in %.6f s. using %d threads.\n", palindrome_count[i], i, time_taken[i], thrd_count);
    }
    printf("*******************************************************************\n");
}
