#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_INPUT 10000

typedef struct node{
    int aut;
    int quantity;
    struct node * left;
    struct node * right;
} node_t;

typedef struct list_node{
    int kilometer;
    int max_autonomy;
    node_t * cars;
    struct list_node * left;
    struct list_node * right;
} list_node;

typedef struct{
    int * arr;
    int head;
    int tail;
    int max_size;
} Queue;

int aggiungi_stazione(int);
list_node * demolisci_stazione(list_node * root, int k);
int aggiungi_auto(int, int);
void rottama_auto(int, int);
void pianifica_percorso_list(int, int, int);

void printStations();
list_node * findStation(int k);
list_node * findSuccessor(list_node * root, list_node * node);
list_node * findPredecessor(list_node * root, list_node * node);
node_t * deleteNode(node_t * root, int k);
node_t * insertCar(node_t * root, int autonomy);
void inOrderPrint(node_t *pNode);
node_t * freeTree(node_t *);
int countNodes(list_node* curr);
void print_reverse_path(const int *, int);

void build_adjList(int n);
void printAdj(int n);

void pushQueue(Queue * , int);
int popQueue(Queue *);
bool isEmpty(Queue);
void printQueue(Queue *queue);

list_node * stations = NULL;
int * station_array;
int * maximum_array;
int stationNumber = 0;
int * adjList;

int main() {
    setbuf(stdout, NULL);
    //TODO: alla creazione di una stazione mettere le macchine in blocchi + passare il puntatore della stazione direttamente
    //TODO: liste di adiacenza + BFS

    char *input = (char *)malloc(MAX_INPUT * sizeof(char));

    int finished = 0;
    while(!finished){
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            break;
        }

        char * token = strtok(input, " ");
        if(!strcmp(token, "aggiungi-stazione")){
            //Aggiungo la stazione al kilometro giusto
            token = strtok(NULL, " ");
            int kilometer = atoi(token);
            int esito = aggiungi_stazione(kilometer);

            //Ogni macchina da input viene aggiunta alla stazione giusta
            if(!esito){
                token = strtok(NULL, " ");
                int car_num = atoi(token);
                for(int i=0; i < car_num; i++){
                    token = strtok(NULL, " ");
                    int car_autonomy = atoi(token);
                    aggiungi_auto(kilometer, car_autonomy);
                }
            }

        }
        else if(!strcmp(token, "demolisci-stazione")){
            int kilometer = atoi(strtok(NULL, " "));
            stations = demolisci_stazione(stations, kilometer);
        }
        else if(!strcmp(token, "aggiungi-auto")){
            int kilometer = atoi(strtok(NULL, " "));
            int autonomy = atoi(strtok(NULL, " "));
            int res = aggiungi_auto(kilometer, autonomy);
            res == 0 ? printf("aggiunta\n") : printf("non aggiunta\n");
        }
        else if(!strcmp(token, "rottama-auto")){
            int kilometer = atoi(strtok(NULL, " "));
            int autonomy = atoi(strtok(NULL, " "));
            rottama_auto(kilometer, autonomy);
        }
        else if(!strcmp(token, "pianifica-percorso")){
            int inizio = atoi(strtok(NULL, " "));
            int fine = atoi(strtok(NULL, " "));

            int numNodes = countNodes(stations);
            station_array = (int*) malloc(numNodes * sizeof(int));
            maximum_array = (int*)malloc(numNodes * sizeof(int));

            //Aggiorno strutture dati station_array e maximum_array
            list_node * curr = findStation(inizio);
            int inizio_ind = -1, fine_ind = -1, n = 0;
            bool asc = inizio < fine ? true : false;
            if(asc){
                while(curr != NULL && curr->kilometer <= fine){
                    station_array[n] = curr->kilometer;
                    maximum_array[n] = curr->max_autonomy;
                    if(curr->kilometer == inizio){
                        inizio_ind = n;
                    }
                    else if(curr->kilometer == fine){
                        fine_ind = n;
                    }
                    n++;

                    curr = findSuccessor(stations, curr);
                }
            }
            else {
                while (curr != NULL && curr->kilometer >= fine) {
                    station_array[n] = curr->kilometer;
                    maximum_array[n] = curr->max_autonomy;
                    if (curr->kilometer == inizio) {
                        inizio_ind = n;
                    } else if (curr->kilometer == fine) {
                        fine_ind = n;
                    }
                    n++;

                    curr = findPredecessor(stations, curr);
                }
            }
            adjList = (int *) malloc(n * sizeof(int));

            pianifica_percorso_list(inizio_ind, fine_ind, n);


            free(adjList);
            free(station_array);
            free(maximum_array);
        }
        else if(!strcmp(token, "stampa\n")){
            printStations();
        }
        else{
            printf("Comando di input non riconosciuto \n");
        }

        if(input == NULL)
            finished = 1;
    }
    free(input);
}

int maximum(node_t * root){
    if(root == NULL){
        return 0;
    }

    node_t * curr = root;
    while(curr->right != NULL){
        curr = curr->right;
    }

    return curr->aut;
}

void rottama_auto(int kilometer, int autonomy) {
    list_node* curr = stations;

    while (curr != NULL && curr->kilometer != kilometer) {
        curr = kilometer >= curr->kilometer ? curr->right : curr->left;
    }
    if (curr != NULL && curr->kilometer == kilometer) {
        curr->cars = deleteNode(curr->cars, autonomy);
        int max_autonomy = maximum(curr->cars);
        if (curr->max_autonomy != max_autonomy) {
            curr->max_autonomy = max_autonomy;
        }
    } else {
        printf("non rottamata\n");
    }
}

int aggiungi_stazione(int k) {
    list_node *curr = NULL;

    list_node *new = (list_node *)malloc(sizeof(list_node));
    new->kilometer = k;
    new->cars = NULL;
    new->max_autonomy = 0;
    new->left = new->right = NULL;

    if (stations == NULL){
        stations = new;
        stationNumber++;
        printf("aggiunta\n");
        return 0;
    }
    else {
        curr = stations;

        while (curr != NULL) {
            if (k < curr->kilometer) {
                if (curr->left == NULL) {
                    curr->left = new;
                    stationNumber++;
                    printf("aggiunta\n");
                    return 0;
                } else
                    curr = curr->left;
            } else if (k > curr->kilometer) {
                if (curr->right == NULL) {
                    curr->right = new;
                    stationNumber++;
                    printf("aggiunta\n");
                    return 0;
                } else
                    curr = curr->right;
            } else {
                free(new);
                printf("non aggiunta\n");
                return 1;
            }
        }
    }
    return 1;
}


int aggiungi_auto(int kilometer, int autonomy){
    list_node * curr = stations;

    while(curr != NULL && curr->kilometer != kilometer){
        curr =  kilometer > curr->kilometer ? curr->right : curr->left;
    }
    if(curr == NULL)
        return 1;
    if(curr->kilometer == kilometer){
        if(curr->max_autonomy < autonomy){
            curr->max_autonomy = autonomy;
        }
        curr->cars = insertCar(curr->cars, autonomy);
        return 0;
    }
    else
        return 1;
}

node_t * newNode(int val){
    node_t * node = (node_t *) malloc(sizeof(node_t));
    node->aut = val;
    node->quantity = 1;
    node->left = NULL;
    node->right = NULL;

    return node;
}

node_t * insertCar(node_t * root, int autonomy){
    if(root == NULL)
        return newNode(autonomy);
    if(root->aut < autonomy)
        root->right = insertCar(root->right, autonomy);
    else if(root->aut > autonomy)
        root->left = insertCar(root->left, autonomy);
    else if(root->aut == autonomy)
        root->quantity++;

    return root;
}

void freeCarTree(node_t* root) {
    if (root != NULL) {
        freeCarTree(root->left);
        freeCarTree(root->right);
        free(root);
    }
}

list_node * demolisci_stazione(list_node * root, int k) {
    if (root == NULL) {
        printf("non demolita\n");
        return root;
    }

    if (root->kilometer > k) {
        root->left = demolisci_stazione(root->left, k);
    } else if (root->kilometer < k) {
        root->right = demolisci_stazione(root->right, k);
    } else {
        // Free the cars' memory
        freeCarTree(root->cars);

        if (root->left == NULL) {
            list_node * temp = root->right;
            free(root);
            printf("demolita\n");
            return temp;
        } else if (root->right == NULL) {
            list_node * temp = root->left;
            free(root);
            printf("demolita\n");
            return temp;
        }
        else {
            // Node has both left and right subtrees
            list_node * succParent = root;
            list_node * succ = root->right;
            while (succ->left != NULL) {
                succParent = succ;
                succ = succ->left;
            }

            if (succParent != root)
                succParent->left = succ->right;
            else
                succParent->right = succ->right;

            // Copy the data from successor node to the node to be deleted
            root->kilometer = succ->kilometer;
            root->cars = succ->cars;
            root->max_autonomy = succ->max_autonomy;

            free(succ); // Free the successor node
            printf("demolita\n");

        }
    }
    return root;
}


node_t* deleteNode(node_t* root, int k) {
    if (root == NULL) {
        printf("non rottamata\n");
        return root;
    }

    if (root->aut > k) {
        root->left = deleteNode(root->left, k);
    } else if (root->aut < k) {
        root->right = deleteNode(root->right, k);
    } else {
        if (root->quantity > 1) {
            root->quantity--;
            printf("rottamata\n");
        } else {
            if (root->left == NULL) {
                node_t* temp = root->right;
                free(root);
                printf("rottamata\n");
                return temp;
            } else if (root->right == NULL) {
                node_t* temp = root->left;
                free(root);
                printf("rottamata\n");
                return temp;
            }
            else {
                node_t* succParent = root;
                node_t* succ = root->right;
                while (succ->left != NULL) {
                    succParent = succ;
                    succ = succ->left;
                }

                if (succParent != root)
                    succParent->left = succ->right;
                else
                    succParent->right = succ->right;

                root->aut = succ->aut;
                root->quantity = succ->quantity;

                free(succ);
                printf("rottamata\n");
            }

        }
    }
    return root;
}


node_t * freeTree(node_t* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
    return NULL;
}

void inOrderPrint(node_t * node) {
    if(node == NULL)
        return;

    inOrderPrint(node->left);
    printf("(%d, %d) ", node->aut, node->quantity);
    inOrderPrint(node->right);
}

void recPrintStations(list_node* root){
    if(root != NULL){
        recPrintStations(root->left);
        printf("%d: ", root->kilometer);
        inOrderPrint(root->cars);
        printf("\n");
        recPrintStations(root->right);
    }
}

void printStations(){
    list_node * curr = stations;
    printf("[\n");
    recPrintStations(curr);
    printf("]\n");
}

list_node * findStation(int k){
    if(stations == NULL)
        return NULL;
    else{
        list_node * curr = stations;
        while(curr != NULL){
            if(curr->kilometer == k)
                return curr;
            curr = k > curr->kilometer ? curr->right : curr->left;
        }
    }
    return NULL;
}

list_node * findMinimum(list_node * root){
    if(root == NULL)
        return NULL;
    while(root->left != NULL){
        root = root->left;
    }

    return root;
}

list_node * findMaximum(list_node * root) {
    if (root == NULL)
        return NULL;

    while (root->right != NULL)
        root = root->right;

    return root;
}

list_node * findSuccessor(list_node * root, list_node * node) {
    if (node->right != NULL)
        return findMinimum(node->right);

    list_node * succ = NULL;

    while (root != NULL) {
        if (node->kilometer < root->kilometer) {
            succ = root;
            root = root->left;
        } else if (node->kilometer > root->kilometer) {
            root = root->right;
        } else {
            break;
        }
    }

    return succ;
}

list_node * findPredecessor(list_node * root, list_node * node) {
    if (node->left != NULL)
        return findMaximum(node->left);

    list_node * pred = NULL;

    while (root != NULL) {
        if (node->kilometer > root->kilometer) {
            pred = root;
            root = root->right;
        } else if (node->kilometer < root->kilometer) {
            root = root->left;
        } else {
            break;
        }
    }

    return pred;
}

void inOrderCount(list_node* curr, int * k){
    if(curr != NULL){
        inOrderCount(curr->left, k);
        station_array[*k] = curr->kilometer;
        maximum_array[*k] = curr->max_autonomy;
        (*k)++;
        inOrderCount(curr->right, k);
    }
}

// Helper function to calculate the number of nodes in the tree
int countNodes(list_node* curr) {
    if (curr == NULL) return 0;
    return countNodes(curr->left) + countNodes(curr->right) + 1;
}

void print_path(int inizio_ind, int fine_ind, int pred[]) {
    if (fine_ind == inizio_ind) {
        printf("%d", station_array[inizio_ind]);
        return;
    }
    print_path(inizio_ind, pred[fine_ind], pred);
    printf(" %d", station_array[fine_ind]);
}

void pushQueue(Queue* queue, int el) {
    if ((queue->tail + 1) % queue->max_size == queue->head) {
        printf("Queue full\n");
        printf("queue->max_size: %d\n", queue->max_size);
        printf("queue->head: %d queue->tail: %d\n", queue->head, queue->tail);
        printQueue(queue);
        exit(-1);
    }

    if (isEmpty(*queue))
        queue->head = queue->tail = 0;
    else
        queue->tail = (queue->tail + 1) % queue->max_size;

    queue->arr[queue->tail] = el;
}

int popQueue(Queue* queue) {
    if (isEmpty(*queue)) {
        printf("Queue empty\n");
        exit(-1);
    }

    int vertex = queue->arr[queue->head];

    if (queue->head == queue->tail)
        queue->head = queue->tail = -1;
    else
        queue->head = (queue->head + 1) % queue->max_size;

    return vertex;
}

bool isEmpty(Queue queue) {
    return (queue.head == -1);
}



void printQueue(Queue * queue) {
    printf("DEBUG: Printing queue\n");
    if (queue->head == queue->tail) {
        printf("Queue is empty.\n");
    } else {
        for (int i = queue->head; i < queue->tail; i++){
            printf("%d ", queue->arr[i]);
        }
        printf("\n");
    }
}

void pianifica_percorso_list(int inizio_ind, int fine_ind , int n) {

    if(inizio_ind == -1 || fine_ind == -1){
        printf("nessun percorso\n");
        return;
    }

    build_adjList(n);
    int * path = (int *) malloc(n * sizeof(int));
    int * distance = (int *) malloc(n* sizeof(int));
    for(int i = 0; i < n; i++){
        distance[i] = -1;
    }

    distance[0] = 0;
    path[0] = 0;

    Queue queue;
    queue.head = -1;
    queue.tail = -1;
    queue.max_size = n;
    queue.arr = (int *) malloc(queue.max_size * sizeof(int));

    pushQueue(&queue, 0);

    while(!isEmpty(queue)){
        int size = queue.tail - queue.head + 1;
        while(size-- > 0){
            //printf("DEBUG: size = %d\n", size+1);
            int vertex = popQueue(&queue);
            size--;
            //printf("DEBUG: Entering in for loop with vertex: %d\n", vertex);
            for(int i = vertex + 1; i <= adjList[vertex]; i++){
                if(distance[i] == -1){
                    distance[i] = distance[vertex] + 1;
                    path[i] = vertex;
                    //printf("DEBUG: Printing path [distance] array\n");
                    //for(int j = 0; j < n; j++){
                    //    printf("%d <- %d\t[%d]\n", j, path[j], distance[j]);
                    //}
                    pushQueue(&queue, i);
                    size++;
                    //printQueue(&queue);
                }
                else{
                    distance[i] = distance[i] <= distance[vertex] + 1 ? distance[i] : distance[vertex] + 1;
                    if(distance[i] == distance[vertex]+1){
                        //printf("-----CASO----\n");
                        if(station_array[vertex] < station_array[path[i]]){
                            //printf("-------CASO PAZZO-------\n");
                            path[i] = vertex;
                            //printf("DEBUG: Printing path [distance] array\n");
                            //for(int j = 0; j < n; j++){
                            //    printf("%d <- %d\t[%d]\n", j, path[j], distance[j]);
                            //}

                            //pushQueue(&queue, i);
                            //size++;

                            //printf("pushed %d in queue\n", i);
                            //printQueue(&queue);
                        }
                    }
                }
            }
        }
    }

    if (distance[fine_ind] == -1){
        printf("nessun percorso\n");
        return;
    }

    //printAdj(n);
    print_reverse_path(path, n);

    free(queue.arr);
    free(path);
    free(distance);
}


void build_adjList(int n) {
    for(int i = 0; i < n; i++){
        adjList[i] = -1;
        bool enoughClose = true;
        int j = i+1;
        while(enoughClose){
            if(j < n && maximum_array[i] >= abs(station_array[i] - station_array[j]))
                adjList[i] = j;
            j++;
            if(j >= n || maximum_array[i] < abs(station_array[i] - station_array[j]))
                enoughClose = false;
        }
    }
    //printAdj(n);
}

void printAdj(int n){
    for(int i = 0; i < n; i++){
        printf("%d [km %d, aut %d]: %d\n", i, station_array[i], maximum_array[i], adjList[i]);
    }
}


void print_reverse_path(const int *pred, int n) {
    // Store the stations in a stack
    int stack[n];
    int top = -1;

    int current = n-1;
    while (current != 0) {
        stack[++top] = current;
        current = pred[current];
    }
    stack[++top] = 0;

    // Print the stations in reverse order
    printf("%d ", station_array[stack[top]]);
    for (int i = top - 1; i > 0; i--) {
        printf("%d ", station_array[stack[i]]);
    }
    printf("%d\n", station_array[stack[0]]);
}
