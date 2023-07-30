#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    struct list_node * next;
    struct list_node * prev;
} list_node;

int aggiungi_stazione(int);
void demolisci_stazione(int);
int aggiungi_auto(int, int);
void rottama_auto(int, int);
void pianifica_percorso(int, int);

void printStations();


node_t * deleteNode(node_t * root, int k);
node_t * insertCar(node_t * root, int autonomy);

void inOrderPrint(node_t *pNode);
void printCost();

void update_cost_matrix();
void update_station();
void update_car();
void checkMaximums();

list_node * stations = NULL;
int * station_array;
int * maximum_array;
int stationNumber = 0;
int dim = 0;
int ** cost_matrix;

//TODO: IDEA di tenere in memoria anche le distanze calcolate?
//TODO: IDEA non cancellare la matrice delle distanze ogni vol

int main() {
    setbuf(stdout, NULL);
    //struttura dati per memorizzare le macchine disponibili  --> BINARY TREE
    //struttura dati per memorizzare le stazioni (albero? bilanciato?)  --> DOUBLE LINKED LIST da trasformare in grafo

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
            demolisci_stazione(kilometer);
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
            pianifica_percorso(inizio, fine);
        }
        else if(!strcmp(token, "stampa\n")){
            printStations();
        }
        else if(!strcmp(token, "stampa-c\n")){
            printCost();
        }
        else if(!strcmp(token, "max\n")){
            checkMaximums();
        }
        else{
            printf("Comando di input non riconosciuto \n");
        }

        if(input == NULL)
            finished = 1;
    }
    for (int j = 0; j < stationNumber; j++) {
        free(cost_matrix[j]);
    }
    free(cost_matrix);
    free(station_array);
    free(maximum_array);
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

    while (curr->next != NULL && curr->kilometer != kilometer) {
        curr = curr->next;
    }
    if (curr != NULL && curr->kilometer == kilometer) {
        curr->cars = deleteNode(curr->cars, autonomy);
        int max_autonomy = maximum(curr->cars);
        if (curr->max_autonomy != max_autonomy) {
            curr->max_autonomy = max_autonomy;
            //update_cost_matrix();
        }
    } else {
        printf("non rottamata\n");
    }
}

void checkMaximums(){
    list_node * curr = stations;

    if(curr != NULL){
        if(curr->max_autonomy != maximum(curr->cars))
            printf("ERROREEEEE: (stazione %d) Massimo salvato: %d, massimo reale: %d\n", curr->kilometer, curr->max_autonomy, maximum(curr->cars));

        curr = curr->next;
    }
}



int aggiungi_stazione(int k) {
    list_node *curr = NULL;

    list_node *new = (list_node *)malloc(sizeof(list_node));
    new->kilometer = k;
    new->cars = NULL;
    new->max_autonomy = 0;
    new->next = new->prev = NULL;

    if (stations == NULL)
        stations = new;
    else if (stations->kilometer > k) {
        new->next = stations;
        new->next->prev = new;
        stations = new;
    }
    else {
        curr = stations;

        while (curr->next != NULL && curr->next->kilometer <= new->kilometer)
            curr = curr->next;

        if(curr->kilometer == k){
            free(new);
            printf("non aggiunta\n");
            return 1;
        }

        new->next = curr->next;

        if (curr->next != NULL)
            new->next->prev = new;

        curr->next = new;
        new->prev = curr;
    }

    stationNumber++;
    printf("aggiunta\n");
    //update_cost_matrix();
    return 0;
}

int aggiungi_auto(int kilometer, int autonomy){
    list_node * curr = stations;

    while(curr->next != NULL && curr->kilometer != kilometer){
        curr = curr->next;
    }
    if(curr->kilometer == kilometer){
        if(curr->max_autonomy < autonomy){
            curr->max_autonomy = autonomy;
            //update_cost_matrix();
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

node_t * freeTree(node_t *);

void demolisci_stazione(int k) {
    list_node* curr;

    if (stations == NULL)
        return;
    else {
        curr = stations;

        while (curr != NULL) {
            if (curr->kilometer == k) {
                if (curr->prev != NULL)
                    curr->prev->next = curr->next;
                else
                    stations = curr->next; // Update stations pointer if we delete the first element

                if (curr->next != NULL)
                    curr->next->prev = curr->prev;

                curr->cars = freeTree(curr->cars);
                free(curr);
                stationNumber--;
                printf("demolita\n");
                //update_cost_matrix();
                return;
            }

            curr = curr->next;
        }

        printf("non demolita\n");
    }
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


void printStations(){
    list_node * curr = stations;
    printf("[\n");
    while (curr != NULL) {
        printf("%d: ", curr->kilometer);
        inOrderPrint(curr->cars);
        printf("\n");
        curr = curr->next;
    }
    printf("]\n");
}

void print_reverse_path(int start, int end, const int *pred) {
    // Store the stations in a stack
    int stack[stationNumber];
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

void printCost(){
    for(int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
            printf("\t%d ", cost_matrix[i][j]);
        }
        printf("\n");
    }
}

void pianifica_percorso(int inizio, int fine){
    update_cost_matrix();
    int inizio_ind = -1, fine_ind = -1, count, mindistance, nextnode = -1;

    for(int i = 0; i < stationNumber; i++){
        if(station_array[i] == inizio)
            inizio_ind = i;
        if(station_array[i] == fine){
            fine_ind = i;
        }
        if(inizio_ind != -1 && fine_ind != -1)
            break;
    }

    if(inizio_ind == -1 || fine_ind == -1){
        printf("nessun percorso\n");
        return;
    }

    //Initialization
    int distance[stationNumber], pred[stationNumber], visited[stationNumber];
    for(int i = 0; i < stationNumber; i++){
        distance[i] = cost_matrix[inizio_ind][i];
        pred[i] = inizio_ind;
        visited[i] = 0;
    }
    distance[inizio_ind] = 0;
    visited[inizio_ind] = 1;

    count = 1;
    while(count<stationNumber-1) {
        mindistance = INF;
        for (int i = 0; i < stationNumber; i++)
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
        for(int i=0;i<stationNumber;i++)
            if(!visited[i] && mindistance != INF && cost_matrix[nextnode][i] != INF) {
                if (mindistance + cost_matrix[nextnode][i] < distance[i]) {
                    distance[i] = mindistance + cost_matrix[nextnode][i];
                    pred[i] = nextnode;
                }
                else if (mindistance + cost_matrix[nextnode][i] == distance[i])
                    pred[i] = station_array[pred[i]] < station_array[nextnode] ? pred[i] : nextnode;
            }
        count++;
    }

    if (distance[fine_ind] == INF){
        printf("nessun percorso\n");
        return;
    }

    print_reverse_path(inizio_ind, fine_ind, pred);
}

void update_cost_matrix(){
    //Deallocate previously allocated memory
    if (cost_matrix != NULL) {
        for (int i = 0; i < dim; i++) {
            free(cost_matrix[i]);
        }
        free(cost_matrix);
    }

    free(station_array);
    free(maximum_array);

    station_array = (int*)malloc(stationNumber * sizeof(int));
    maximum_array = (int*)malloc(stationNumber * sizeof(int));
    list_node * curr = stations;
    int k = 0;
    while(curr != NULL){
        station_array[k] = curr->kilometer;
        maximum_array[k] = curr->max_autonomy;
        curr = curr->next;
        k++;
    }

    cost_matrix = (int**)malloc(k * sizeof(int*));
    for (int j = 0; j < k; j++) {
        cost_matrix[j] = (int*)malloc(k * sizeof(int));
    }

    for(int i = 0; i < k; i++){
        for(int j = 0; j < k; j++){
            if(i == j)
                cost_matrix[i][j] = 0;
            else if(maximum_array[i] >= abs(station_array[i] - station_array[j]))
                cost_matrix[i][j] = 1;
            else
                cost_matrix[i][j] = INF;
        }
    }

    dim = stationNumber;
}