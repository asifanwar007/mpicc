#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

#define NUM_ROWS 1000
#define NUM_COLS 1000
#define MAX_PLND_SIZE 6

char plnd_matrix[NUM_ROWS][NUM_COLS];

void plndr_finder(int proc_rank, int num_procs);

bool chk_plndrm(char* str, int len) {
    int start = 0;
    int end = len - 1;
    while (start < end) {
        if (str[start] != str[end]) {
            return false;
        }
        start++;
        end--;
    }
    return true;
}

int main(int argc, char** argv) {
    srand(time(NULL));
    MPI_Init(&argc, &argv);

    int proc_rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            plnd_matrix[i][j] = (rand() % 26) + 'a'; 
        }
    }

    for (int num_procs = 1; num_procs <= 8; num_procs++) {
        plndr_finder(proc_rank, num_procs);
    }

    MPI_Finalize();
    return 0;
}

void plndr_finder(int proc_rank, int num_procs) {
    int plndrm_counts[MAX_PLND_SIZE + 1] = {0};
    double time_spent[MAX_PLND_SIZE + 1] = {0};

    int rows_per_proc = (NUM_ROWS + num_procs - 1) / num_procs;
    int start_row = proc_rank * rows_per_proc;
    int end_row = (proc_rank + 1) * rows_per_proc;
    if (end_row > NUM_ROWS) {
        end_row = NUM_ROWS;
    }

    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = proc_rank; j < NUM_COLS; j ++) {
            for (int k = 0; k <= MAX_PLND_SIZE; k++) {
                if (j + k >= NUM_COLS) break;
                char sub_str[MAX_PLND_SIZE + 1];
                strncpy(sub_str, &plnd_matrix[i][j], k + 1);
                sub_str[k + 1] = '\0';
                double st = MPI_Wtime();
                if (chk_plndrm(sub_str, k + 1)) {
                    plndrm_counts[k]++;
                    double et = MPI_Wtime();
                    time_spent[k] += et - st;
                }
            }

            for (int k = 0; k <= MAX_PLND_SIZE; k++) {
                if (i + k >= NUM_ROWS) break;
                char sub_str[MAX_PLND_SIZE + 1];
                for (int l = 0; l <= k; l++) {
                    sub_str[l] = plnd_matrix[i + l][j];
                }
                sub_str[k + 1] = '\0';
                double st = MPI_Wtime();
                if (chk_plndrm(sub_str, k + 1)) {
                    plndrm_counts[k]++;
                    double et = MPI_Wtime();
                    time_spent[k] += et - st;
                }
            }
        }
    }

    int global_counts[MAX_PLND_SIZE + 1];
    MPI_Reduce(plndrm_counts, global_counts, MAX_PLND_SIZE + 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    double global_time[MAX_PLND_SIZE + 1];
    MPI_Reduce(time_spent, global_time, MAX_PLND_SIZE + 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (proc_rank == 0) {
        for (int i = 3; i <= MAX_PLND_SIZE; i++) {
            printf("Found %d palindromes of size %d in %.6f s using %d processes.\n", global_counts[i], i, global_time[i], num_procs);
        }
        printf("************************************************************\n");
    }
}
