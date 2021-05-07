#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//define global variables
long long int nullNodesCounter = 0;
long long int * playerSkills;
long long int excitementSum = 0;

typedef struct Node Node; //struct to hold to info for each node

Node * createNode(long int tableNum); //create nodes for the tree
Node * insertNode(Node * root, long int tableNum); //insert nodes into the tree

void sumOfPlayersSkills(Node * root); //simulation function to do the post order and calculate the excitement

Node * findSmallest(Node * root); //find the smallest node
long long int findBiggest(long long int player1, long long int player2); //find the biggest skill level from two players

Node* removeNode(Node * root, long int tableNum); //remove the nodes and free any memory allocation

typedef struct Node {
    long int tableNum; //the table number OR a player's skill level
    Node * l, * r; //to reach the different players at each table in the binary search tree
    long long int winner; //to store the winner of a round in the tournament
} Node;

Node * createNode(long int tableNum) {
    Node * ret = calloc(1, sizeof(Node));
    ret->tableNum = tableNum;
    ret->r = ret->l = NULL;
    return ret;
}

//return the root of the tree
Node * insertNode(Node * root, long int tableNum) {
    if(root == NULL) {
        return createNode(tableNum); //if there's no root, then need to make one
    }

    if(root->tableNum < tableNum) { //root is smaller than our target
        root->r = insertNode(root->r, tableNum); //insert to the right
    }
    else { //root is larger than our target
        root->l = insertNode(root->l, tableNum); //insert to the left
    }

    return root;
}

void sumOfPlayersSkills(Node * root) {
    long long int player1, player2; //the two players at each table
    long long int excitementDif = 0; //the difference between the players' skill levels

    if(root == NULL) { //if there are no tables, then the excitement stays 0
        return;
    }

    //going down a level
    sumOfPlayersSkills(root->l);
    //coming up a level

    //going down a level
    sumOfPlayersSkills(root->r);
    //coming up a level

    if((root->l == NULL) && (root->r == NULL)) { //if the table is a leaf in the BST
        player1 = playerSkills[(root->tableNum) - 1];
        nullNodesCounter++;

        player2 = playerSkills[root->tableNum];
        nullNodesCounter++;

        root->winner = findBiggest(player1, player2);

        excitementDif = abs(player1 - player2); //find the difference between the players' skill levels

        excitementSum += excitementDif; //add the difference to the overall excitement of the tournament
    }
    else if((root->l == NULL) || (root->r == NULL)) { //if the table only has one child
        if(root->l == NULL) {
            player1 = playerSkills[(root->tableNum) - 1];
            nullNodesCounter++;

            player2 = root->r->winner;
        }
        else if(root->r == NULL) {
            player1 = root->l->winner;

            player2 = playerSkills[root->tableNum];
            nullNodesCounter++;
        }

        root->winner = findBiggest(player1, player2);

        excitementDif = abs(player1 - player2);

        excitementSum += excitementDif;
    }
    else if((root->l != NULL) && (root->r != NULL)) { //if the table has two children
        player1 = root->l->winner;
        player2 = root->r->winner;

        root->winner = findBiggest(player1, player2);

        excitementDif = abs(player1 - player2);

        excitementSum += excitementDif;
    }

    //beginning the process of moving up
}

//method to find the smallest node in the tree
Node * findSmallest(Node * root) {
    //if there is a smaller node (left child)
    //then find the smallest in that left subtree

    if(root->l != NULL) {
        return findSmallest(root->l);
    }

    //no smaller node (no left child)

    return root; //base case after recursive call
}

//method to find the greater player skill between two competitors in the tree
long long int findBiggest(long long int player1, long long int player2) {
    if(player1 > player2) { //if the 1st player has a greater skill level than the second
        return player1;
    }
    else if(player2 > player1) { //if the 2nd player has a greater skill level than the first
        return player2;
    }
}

Node * removeNode(Node * root, long int tableNum) {
    if(root == NULL)
        return root;
    if(root->tableNum == tableNum) {
        if(root->r == NULL && root->l == NULL) { //if the node has 0 children
            free(root);

            return NULL;
        }

        if(root->r == NULL || root->l == NULL) { //if the node has 1 child
            Node * child = root->r;
            if (child == NULL) child = root->l;
            free(root);
            return child;
        }

        //if the node has 2 children
        //find the min in the larger sub (replacement)
        Node * smallest = findSmallest(root->r);

        //swap values
        int tmp = root->tableNum;
        root->tableNum = smallest->tableNum;
        smallest->tableNum = tmp;

        //remove the tableNum from that right subtree
        root->r = removeNode(root->r, tableNum);

        //root is still the same (ideally)
        return root;
    }

    if(root->tableNum < tableNum) {
        root->r = removeNode(root->r, tableNum);
    }

    if(root->tableNum > tableNum) {
        root->l = removeNode(root->l, tableNum);
    }

    return root;
}

int main() {
    int i; //looping variable
    long int numPlayers; //the number of players
    long int* tableOrder; //to store the order in which the tables should be added to the tree

    Node * root = NULL;

    scanf("%ld", &numPlayers);

    tableOrder = (long int*) calloc(numPlayers, sizeof(long int));
    playerSkills = (long long int*) calloc(numPlayers, sizeof(long long int));

    for(i = 0; i < numPlayers - 1; i++) { //take in the values that correspond to the tables' activation order
        scanf("%ld", &tableOrder[i]);
    }

    for(i = 0; i < numPlayers; i++) { //take in the skill levels of each player in the tournament
        scanf("%lli", &playerSkills[i]);
    }

    for(i = numPlayers - 1; i >= 0; i--) { //create the BST with the tables in their activation order, which should be the same as their post-order
        if(tableOrder[i] != 0) {
            root = insertNode(root, tableOrder[i]);
        }
    }

    sumOfPlayersSkills(root); //calculate the excitement level of the overall tournament

    printf("\n%lli\n", excitementSum); //print the excitement level of the overall tournament

    for(i = 0; i < numPlayers - 1; i++) { //free the tree
        removeNode(root, tableOrder[i]);
    }

    free(playerSkills); //remove the memory allocation for the skill levels
    free(tableOrder); //remove the memory allocation for the table activation order

    return 0;
}
