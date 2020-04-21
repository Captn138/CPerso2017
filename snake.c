#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

void initBoard(int nbLin, int nbCol, int board[nbLin][nbCol]){
	for(int k=0; k<nbLin; k++){
		for(int l=0; l<nbCol; l++){
			board[k][l]=0;
		}
	}
}

void initSnake(int nbLin, int nbCol, int board[nbLin][nbCol], int x[], int y[]){
	x[0]=nbLin/2;
	y[0]=nbCol/2;
	x[1]=-1;
	y[1]=-1;
}

void cpr(){
	printf("\n\n\n\nSnake game\n\n     Copyright Mickael Almeida 2018\n\n");
}

void initFood(int nbLin, int nbCol, int board[nbLin][nbCol], int size){
	size--;
	int food=rand()%size, counter=0, flag=0;
	for(int k=0; k<nbLin; k++){
		for(int l=0; l<nbCol; l++){
			counter++;
			if(counter>=food && board[k][l]==0 && flag==0){
				board[k][l]=2;
				flag=1;
			}
		}
	}
}

int parameters(int*pnbLin, int*pnbCol){
	int wei, a, b;
	printf("%c[2J", 0x1B);
	printf("%c[%d;%dH", 0x1B, 1, 1);
	printf("Which type of controls do you prefrer?\n 0 = q-d\n 1 = z-q-s-d\n");
	do{
		a=scanf("%d", &wei);
	}while(a!=1 || wei<0 || wei>1);
	printf("\n\nWhich size of the board do you prefer?\n 0 = 10x10\n 1 = 15x15\n 2 = 20x20\n 3 = 25x25\n 4 = 30x30\n");
	do{
		a=scanf("%d", &b);
	}while(a!=1 || b<0 || b>4);
	printf("\n\nThe game will be played using ");
	if(wei==0){
		printf("q-d");
	}else{
		printf("z-q-s-d");
	}
	printf(" on a board of size ");
	if(b==0){
		printf("10x10");
		*pnbLin=10;
	}else if(b==1){
		printf("15x15");
		*pnbLin=15;
	}else if(b==2){
		printf("20x20");
		*pnbLin=20;
	}else if(b==3){
		printf("25x25");
		*pnbLin=25;
	}else if(b==4){
		printf("30x30");
		*pnbLin=30;
	}
	*pnbCol=*pnbLin;
	printf("\n\n");
	usleep(1000000);
	printf("3...\n");
	usleep(1000000);
	printf("2...\n");
	usleep(1000000);
	printf("1...\n");
	usleep(1000000);
	return wei;
}

void initGame(int nbLin, int nbCol, int board[nbLin][nbCol], int x[], int y[], int size){
	initBoard(nbLin, nbCol, board);
	initSnake(nbLin, nbCol, board, x, y);
	initFood(nbLin, nbCol, board, size);
}

void showBoard(int nbLin,  int nbCol, int board[nbLin][nbCol]){
	printf("%c[2J", 0x1B);
	printf("%c[%d;%dH", 0x1B, 1, 1);
	usleep(300000);
	for(int k=0; k<nbCol+2; k++){
		printf("+ ");
	}
	printf("\n");
	for(int k=0; k<nbLin; k++){
		printf("+");
		for(int l=0; l<nbCol; l++){
			int a=board[k][l];
			if(a==0){
				printf("  ");
			}else if(a==1){
				printf(" \033[31;01mo\033[00m");
			}else if(a==2){
				printf(" \033[36;01mo\033[00m");
			}else if(a==-1){
				printf(" \033[33;01mx\033[00m");
			}
		}
		printf(" +\n");
	}
	for(int k=0; k<nbCol+2; k++){
		printf("+ ");
	}
	printf("\n");
}

char key_pressed(){
	struct termios oldterm, newterm;
	int oldfd;
	char c, result = 0;
	tcgetattr (STDIN_FILENO, &oldterm);
	newterm = oldterm;
	newterm.c_lflag &= ~(ICANON | ECHO);
	tcsetattr (STDIN_FILENO, TCSANOW, &newterm);
	oldfd = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl (STDIN_FILENO, F_SETFL, oldfd | O_NONBLOCK);
	c = getchar();
	tcsetattr (STDIN_FILENO, TCSANOW, &oldterm);
	fcntl (STDIN_FILENO, F_SETFL, oldfd);
	if (c != EOF) {
		ungetc(c, stdin);
		result = getchar();
	}
	return result;
}

void catchInstructions(int*dir, int wei){
	char order=key_pressed();
	if(wei==1){
		if(order=='z' && *dir!=3){
			*dir=1;
		}else if(order=='q' && *dir!=2){
			*dir=0;
		}else if(order=='s' && *dir!=1){
			*dir=3;
		}else if(order=='d' && *dir!=0){
			*dir=2;
		}
	}else if(wei==0){
		if(order=='q'){
			*dir=(*dir+3)%4;
		}else if(order=='d'){
			*dir=(*dir+1)%4;
		}
	}
}

void calculateNextPos(int x[], int y[], int nextPosSnake[], int*dir){
	if(*dir==0){
		nextPosSnake[0]=x[0];
		nextPosSnake[1]=y[0]-1;
	}else if(*dir==1){
		nextPosSnake[0]=x[0]-1;
		nextPosSnake[1]=y[0];
	}else if(*dir==2){
		nextPosSnake[0]=x[0];
		nextPosSnake[1]=y[0]+1;
	}else if(*dir==3){
		nextPosSnake[0]=x[0]+1;
		nextPosSnake[1]=y[0];
	}
}

int checkForWallCollision(int nbLin, int nbCol, int nextPosSnake[]){
	int check=0;
	if(nextPosSnake[0]<0 || nextPosSnake[0]>=nbLin || nextPosSnake[1]<0 || nextPosSnake[1]>=nbCol){
		check=1;
	}
	return check;
}

int checkForBeamCollision(int nbLin, int nbCol, int board[nbLin][nbCol], int nextPosSnake[]){
	int check = 0;
	if(board[nextPosSnake[0]][nextPosSnake[1]]==1){
		check = 1;
	}
	return check;
}

int moveSnake(int nbLin, int nbCol, int board[nbLin][nbCol], int x[], int y[], int nextPosSnake[], int size, int snake){
	int index;
	for(int k=0; k<size; k++){
		if(x[k]==-1){
			index=k-1;
			break;
		}
	}
	if(board[nextPosSnake[0]][nextPosSnake[1]]==0){ //normal case
		board[x[index]][y[index]]=0; //cleaning the last box
		if(index>0){
			for(int k=index; k>0; k--){ //moving the boxes
				x[k]=x[k-1];
				y[k]=y[k-1];
			}
		}
		x[0]=nextPosSnake[0]; //reinitializing the first boxes
		y[0]=nextPosSnake[1];
		board[nextPosSnake[0]][nextPosSnake[1]]=1; //adding the new box
	}else if(board[nextPosSnake[0]][nextPosSnake[1]]==2){ //case in which we grow up
		snake++;
		for(int k=index+2; k>0; k--){ //moving all the boxes of 1 in the array
			x[k]=x[k-1];
			y[k]=y[k-1];
		}
		x[0]=nextPosSnake[0]; //reinitializing the first boxes
		y[0]=nextPosSnake[1];
		board[nextPosSnake[0]][nextPosSnake[1]]=1; //adding the new box
		initFood(nbLin, nbCol, board, size);
	}
	return snake;
}

int gamuStartu(int nbLin, int nbCol, int board[nbLin][nbCol], int x[], int y[], int size, int wei, int*snake){
	int flag=0, nextPosSnake[2], collision=0, win, dir=0, *pdir=&dir;
	showBoard(nbLin, nbCol, board);
	do{
		catchInstructions(pdir, wei);
		calculateNextPos(x, y, nextPosSnake, pdir);
		if(checkForWallCollision(nbLin, nbCol, nextPosSnake)==1){
			flag=1;
			board[x[0]][y[0]]=-1;
			collision=1;
		}
		if(checkForBeamCollision(nbLin, nbCol, board, nextPosSnake)==1){
			flag=1;
			board[nextPosSnake[0]][nextPosSnake[1]]=-1;
			collision=1;
		}
		if(collision==0){
			int value=*snake;
			*snake=moveSnake(nbLin, nbCol, board, x, y, nextPosSnake, size, value);
			if(*snake==size-1){
				flag=1;
				win=1;
			}
		}else{
			win=0;
		}
		showBoard(nbLin, nbCol, board);
	}while(flag==0);
	return win;
}

void endOfGame(int win, int*snake){
	printf("\n\nGAME OVER\n\n");
	if(win==1){
		printf("YOU WIN!\n\n");
	}else{
		printf("YOU LOSE!\n\n");
	}
	printf("Score: %d\n\n", *snake);
}

int main(){
	srand(time(NULL));
	int NBLIN, NBCOL, WEI, SNAKE=1, *PNBLIN=&NBLIN, *PNBCOL=&NBCOL, *PSNAKE=&SNAKE;
	WEI=parameters(PNBLIN, PNBCOL);
	int BOARD[NBLIN][NBCOL], SIZE=(NBLIN*NBCOL)+1, X[SIZE], Y[SIZE], WIN;
	initGame(NBLIN, NBCOL, BOARD, X, Y, SIZE);
	WIN=gamuStartu(NBLIN, NBCOL, BOARD, X, Y, SIZE, WEI, PSNAKE);
	endOfGame(WIN, PSNAKE);
	cpr();
	return 0;
}
