#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// This project was made by Erol Cebeci

pthread_mutex_t mutex;
int N ; // Size of a row,column
int turn = 1; // Turn in order to control fairness
int isGameOn = 1; // Becomes 0 when game is over
char winner = 'e'; // The winner of the game, initially e
char ** board;

char playGame(char sign){
	// search board until an empty cell found
    int x = 0, y = 0, hasPlayed = 0;
    do
    {
        x = rand() % N;
        y = rand() % N;
        if(board[x][y] == ' '){
            board[x][y] = sign; // mark the cell 
            hasPlayed = 1;
            printf("Player %c played on: (%d,%d)\n",sign,x,y);
        }
              
    } while (!hasPlayed);

    //check rows 
    int hasWon = 1;
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            if(board[i][j] != sign)
                hasWon = 0;
        }
        if(hasWon)
            return sign;
        hasWon = 1;
    }

    //check columns
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            if(board[j][i] != sign)
                hasWon = 0;

        }
        if(hasWon)
            return sign;
        hasWon = 1;
    }

    //check diagonal left to right
    for(int i = 0; i < N; i++)
    {
        if(board[i][i] != sign)
            hasWon = 0;
    }
    if(hasWon)
        return sign;
	hasWon = 1;
    
    //check diagonal rigt to left
    for(int i = 0; i < N; i++)
    {
        if(board[i][N-i-1] != sign)
            hasWon = 0;
    }
    if(hasWon)
        return sign;
        
    //check fullness
    int isFull = 1;
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            if(board[i][j] == ' ')
                isFull = 0;
        }
    }
    
    if(isFull)
        isGameOn = 0;
    
    return 'e';
    
}


void *playX(void * null){
    while(1){
        pthread_mutex_lock(&mutex); // Critical Region Starts from here
        if(turn != 1)
            pthread_mutex_unlock(&mutex); // Check shared variable turn, exit critical region if not permitted
       else if(!isGameOn){
            pthread_mutex_unlock(&mutex); // Exit critical region if game finished
            pthread_exit(NULL);
        }else{
            winner = playGame('X');
            if(winner == 'X' || !isGameOn){
                turn = 2;
                isGameOn = 0;
                pthread_mutex_unlock(&mutex); // If X/O wins or game ended  exit critical region and terminate thread
                pthread_exit(NULL);
                break;
            }else{
                turn = 2;
                pthread_mutex_unlock(&mutex); // pass the turn and exit the critical region
            }
        }
    }
}

// Copy of PlayX function with updated marks and turn numbers
void *playO(void * null){
    while(1){
        pthread_mutex_lock(&mutex);
        if(turn != 2)
            pthread_mutex_unlock(&mutex);
       else if(!isGameOn){
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }else{
            winner = playGame('O');
            if(winner == 'O' || !isGameOn){
                turn = 1;
                isGameOn = 0;
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
                break;
            }else{
                turn = 1;
                pthread_mutex_unlock(&mutex);
            }
        }
    }
}


int main(int argc, char *argv[]) {
    srand(time(NULL)); // get board size from commandline
    N = atoi(argv[1]);

    board = (char **)malloc(sizeof(char*) * N); // allocate memory for the board
    for(int i = 0; i < N; i++)
    {
        board[i] = (char *) malloc(N);
        for(int j = 0; j < N; j++)
        {
            board[i][j] = ' ';
        }
    }


    printf("Board Size : %dx%d\n", N,N);

    // define thread handles and initiate them
    pthread_t p1,p2;
 	pthread_create(&p1, NULL, playX, NULL); 
 	pthread_create(&p2, NULL, playO, NULL) ;
   
   // wait for both threads to join main
   pthread_join(p1, NULL);
   pthread_join(p2, NULL);


   // After threads join, we can directly access the shared resources because there is no possiblity of race condition
    printf("\nGame end\n");

    if(winner == 'X') // Check who won the game
        printf("Winner is X\n");

    else if(winner == 'O')
        printf("Winner is O\n");
    else
        printf("It is a tie\n");


   //prints board
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            printf("[%c]", board[i][j]);
        }
        printf("\n");
    }

    return 0;
}