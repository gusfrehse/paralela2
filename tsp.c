/* WSCAD - 9th Marathon of Parallel Programming 
 * Simple Brute Force Algorithm for the 
 * Traveling-Salesman Problem
 * Author: Emilio Francesquini - francesquini@ic.unicamp.br
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#include <mpi.h>

#define ROOT 0

//#define DBG(x, ...) fprintf(stderr, "id %d: " x "\n", my_rank __VA_OPT__(,) __VA_ARGS__);
#define DBG(x, ...) 

int my_rank, n_procs;

int min_distance;
int nb_towns;

typedef struct {
    int to_town;
    int dist;
} d_info;

d_info **d_matrix;
int *dist_to_origin;

void tsp (int depth, int current_length, int *path, char *present_towns) {
    int i;
    if (current_length >= min_distance) return;
    if (depth == nb_towns) {
        current_length += dist_to_origin[path[nb_towns - 1]];
        if (current_length < min_distance) {
            min_distance = current_length;
            //fprintf(stderr, "%d: found path: ", my_rank);
            //for (int i = 0; i < nb_towns; i++)
            //    fprintf(stderr, "%d ", path[i]);
            //fprintf(stderr, "\n");
        }
    } else {
        int town, me, dist;
        me = path[depth - 1];

        for (i = 0; i < nb_towns; i++) {
            town = d_matrix[me][i].to_town;
            if (!present_towns[town]) {
                path[depth] = town;
                present_towns[town] = 1;
                dist = d_matrix[me][i].dist;
                tsp (depth + 1, current_length + dist, path, present_towns);
                present_towns[town] = 0;
            }
        }
    }
}

void greedy_shortest_first_heuristic(int *x, int *y) {
    int i, j, k, dist;
    int *tempdist;

    tempdist = (int*) malloc(sizeof(int) * nb_towns);
    //Could be faster, albeit not as didactic.
    //Anyway, for tractable sizes of the problem it
    //runs almost instantaneously.
    for (i = 0; i < nb_towns; i++) {
        for (j = 0; j < nb_towns; j++) {
            int dx = x[i] - x[j];
            int dy = y[i] - y[j];
            tempdist [j] = dx * dx + dy * dy;
        }
        for (j = 0; j < nb_towns; j++) {
            int tmp = INT_MAX;
            int town = 0;
            for (k = 0; k < nb_towns; k++) {
                if (tempdist [k] < tmp) {
                    tmp = tempdist [k];
                    town = k;
                }
            }
            tempdist [town] = INT_MAX;
            d_matrix[i][j].to_town = town;
            dist = (int) sqrt (tmp);
            d_matrix[i][j].dist = dist;
            if (i == 0)
                dist_to_origin[town] = dist;
        }
    }

    free(tempdist);
}

void init_tsp() {
    int i, st;
    int *x, *y;

    DBG("init tsp");

    min_distance = INT_MAX;

    if (my_rank == ROOT) {
        DBG("root is scanning nb_towns");
        st = scanf("%u", &nb_towns);
        if (st != 1)
            MPI_Abort(MPI_COMM_WORLD, 1);
    }

    DBG("starting broadcast of nb_towns == %u", nb_towns);
    MPI_Bcast(&nb_towns, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    DBG("bcasted nb_towns == %d", nb_towns);
 
    d_matrix = (d_info**) malloc (sizeof(d_info*) * nb_towns);
    for (i = 0; i < nb_towns; i++)
        d_matrix[i] = (d_info*) malloc (sizeof(d_info) * nb_towns);
    dist_to_origin = (int*) malloc(sizeof(int) * nb_towns);
   
    x = (int*) malloc(sizeof(int) * nb_towns);
    y = (int*) malloc(sizeof(int) * nb_towns);
    
    DBG("A here is x and y: %p %p", x, y);
    DBG("A here is x[0] and y[0]: %d %d", x[0], y[0]);
    DBG("A here is x[nb_towns - 1] and y[nb_towns - 1]: %d %d", x[nb_towns - 1], y[nb_towns - 1]);

    if (my_rank == ROOT) {
        for (i = 0; i < nb_towns; i++) {
            st = scanf("%u %u", x + i, y + i);
            if (st != 2)
                MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Bcast(x, nb_towns, MPI_INT, ROOT, MPI_COMM_WORLD);
    DBG("bcasted x");

    MPI_Bcast(y, nb_towns, MPI_INT, ROOT, MPI_COMM_WORLD);
    DBG("bcasted y");

    DBG("inside init_tsp: nb_towns = %d", nb_towns);
    DBG("B here is x and y: %p %p", x, y);
    DBG("B here is x[0] and y[0]: %d %d", x[0], y[0]);
    DBG("B here is x[nb_towns - 1] and y[nb_towns - 1]: %d %d", x[nb_towns - 1], y[nb_towns - 1]);
    //for (int i = 0; i < nb_towns; i++)
    //    fprintf(stderr, " (%d, %d)", x[i], y[i]);
    //fprintf(stderr, "\n");

    DBG("running heuristic");
    greedy_shortest_first_heuristic(x, y);
    
    DBG("run heuristic");

    free(x);
    free(y);
}

int run_tsp() {
    DBG("run_tsp");
    char *present_towns = calloc(nb_towns, sizeof(char));
    int *path = calloc(nb_towns, sizeof(int));

    for (int i = my_rank + 1; i < nb_towns; i += n_procs)
    {
        path[0] = 0;
        path[1] = i;

        present_towns[0] = 1;
        present_towns[i] = 1;

        tsp(2, dist_to_origin[i], path, present_towns);

        present_towns[0] = 0;
        present_towns[i] = 0;
    }

    free(present_towns);
    free(path);

    for (int i = 0; i < nb_towns; i++)
        free(d_matrix[i]);
    free(d_matrix);

    return min_distance;
}

int main (int argc, char **argv) {
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    int num_instances, st, local_result, min_result;

    if (my_rank == ROOT) {
        DBG("root is scanning num_instances");
        st = scanf("%u", &num_instances);
        if (st != 1)
            MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    DBG("starting broadcast of num_instances == %u", num_instances);
    MPI_Bcast(&num_instances, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    DBG("bcasted num_instances == %d", num_instances);

    while(num_instances-- > 0) {
        DBG("while start");
        init_tsp();

        local_result = run_tsp();

        DBG("got %d", local_result);

        // find out the shortest of the shortest paths
        MPI_Reduce(&local_result,
                   &min_result,
                   1,
                   MPI_INT,
                   MPI_MIN,
                   ROOT,
                   MPI_COMM_WORLD);

        DBG("reduced: %d", min_result);

        if (my_rank == ROOT)
            printf("%d\n", min_result);
    }

    MPI_Finalize();
    return 0;
}
