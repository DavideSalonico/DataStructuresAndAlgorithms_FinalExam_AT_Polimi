#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_INPUT 10000
#define INF 2147483647

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

int aggiungi_stazione(int);
list_node * demolisci_stazione(list_node * root, int k);
int aggiungi_auto(int, int);
void rottama_auto(int, int);
void pianifica_percorso(int, int, int);

void printStations();

list_node * findStation(int k);
list_node * findSuccessor(list_node * root, list_node * node);
list_node * findPredecessor(list_node * root, list_node * node);


node_t * deleteNode(node_t * root, int k);
node_t * insertCar(node_t * root, int autonomy);

void inOrderPrint(node_t *pNode);

node_t * freeTree(node_t *);

void update_cost_matrix(int);
//void checkMaximums();

list_node * stations = NULL;
int * station_array;
int * maximum_array;
int stationNumber = 0;
int ** cost_matrix;

int countNodes(list_node* curr);

int main() {
    setbuf(stdout, NULL);
    //TODO: alla creazione di una stazione mettere le macchine in blocchi + passare il puntatore della stazione direttamente
    //TODO: matrice solo diagonale?

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
            else{
                while(curr != NULL && curr->kilometer >= fine){
                    station_array[n] = curr->kilometer;
                    maximum_array[n] = curr->max_autonomy;
                    if(curr->kilometer == inizio){
                        inizio_ind = n;
                    }
                    else if(curr->kilometer == fine){
                        fine_ind = n;
                    }
                    n++;

                    curr = findPredecessor(stations, curr);
                }
            }
\
            cost_matrix = (int**)malloc(n * sizeof(int*));
            for (int j = 0; j < n; j++) {
                cost_matrix[j] = (int*)malloc(n * sizeof(int));
            }

            pianifica_percorso(inizio_ind, fine_ind, n);


            if (cost_matrix != NULL) {
                for (int i = 0; i < n; i++) {
                    free(cost_matrix[i]);
                }
                free(cost_matrix);
            }
            free(station_array);
            free(maximum_array);
        }
        else if(!strcmp(token, "stampa\n")){
            printStations();
        }
        /*else if(!strcmp(token, "max\n")){
            checkMaximums();
        }
         */
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
        //printf("rottamata\n");
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

void print_reverse_path(int start, int end, const int *pred, int n) {
    // Store the stations in a stack
    int stack[n];
    int top = -1;

    int current = end;
    while (current != start) {
        stack[++top] = current;
        current = pred[current];
    }
    stack[++top] = start;

    // Print the stations in reverse order
    printf("%d ", station_array[stack[top]]);
    for (int i = top - 1; i > 0; i--) {
        printf("%d ", station_array[stack[i]]);
    }
    printf("%d\n", station_array[stack[0]]);
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

void pianifica_percorso(int inizio_ind, int fine_ind, int n){
    update_cost_matrix(n);

    int count, mindistance, nextnode = -1;

    if(inizio_ind == -1 || fine_ind == -1){
        printf("nessun percorso\n");
        return;
    }

    //Initialization
    int distance[n], pred[n], visited[n];
    for(int i = 0; i < n; i++){
        distance[i] = cost_matrix[inizio_ind][i];
        pred[i] = inizio_ind;
        visited[i] = 0;
    }
    distance[inizio_ind] = 0;
    visited[inizio_ind] = 1;

    count = 1;
    while(count<n-1) {
        mindistance = INF;
        for (int i = 0; i < n; i++)
            if (distance[i] < mindistance && !visited[i]) {
                mindistance = distance[i];
                nextnode = i;
            }

        //check if a better path exists through nextnode
        if(nextnode != -1)
            visited[nextnode]=1;
        else{
            printf("nessun percorso\n");
            return;
        }
        for(int i=0;i<n;i++)
            if(!visited[i] && mindistance != INF && cost_matrix[nextnode][i] != INF) {
                if (mindistance + cost_matrix[nextnode][i] < distance[i]) {
                    distance[i] = mindistance + cost_matrix[nextnode][i];
                    pred[i] = nextnode;
                }
                else if (mindistance + cost_matrix[nextnode][i] == distance[i])
                    pred[i] = station_array[pred[i]] <= station_array[nextnode] ? pred[i] : nextnode;
            }
        count++;
    }

    if (distance[fine_ind] == INF){
        printf("nessun percorso\n");
        return;
    }

    print_reverse_path(inizio_ind, fine_ind, pred, n);
}

void update_cost_matrix(int n){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(i == j)
                cost_matrix[i][j] = 0;
            else if(maximum_array[i] >= abs(station_array[i] - station_array[j]))
                cost_matrix[i][j] = 1;
            else
                cost_matrix[i][j] = INF;
        }
    }
}

/*void checkMaximums(){
    list_node * curr = stations;

    if(curr != NULL){
        if(curr->max_autonomy != maximum(curr->cars))
            printf("ERROREEEEE: (stazione %d) Massimo salvato: %d, massimo reale: %d\n", curr->kilometer, curr->max_autonomy, maximum(curr->cars));

        curr = curr->next;
    }
}
 */

