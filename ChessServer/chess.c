#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/shm.h>
#include <sys/ipc.h>

#define Absol(X) ((X) > 0) ? (X) : (-(X))
#define Higher(X, Y) (X > Y) ? (X) : (Y)
#define Lower(X, Y) (X > Y) ? (Y) : (X)

#define SHM_KEY 1573

int oppo_end = 0;

//type, team
typedef struct Piece
{
	char type;
	int team;
} Piece;

typedef struct GamePid
{
	char input[50];
	int game1;
	int game2;
	int login[2];
} GamePid;


void print_board(Piece board[8][8])
{
	for (int j = 0; j < 8; j++)
	{
		printf(" [%c]\t", 'A' + j);
	}
	printf("\n\n");

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j].team == 0)
			{
				printf("[    ]\t");
			}
			else
				printf("[ %c%d ]\t", board[i][j].type, board[i][j].team);
		}

		printf("[%d]\n", 1 + i);
		printf("\n");
	}
}


int main(int argc, char* argv[])
{
	int winFlag = 0;
	int team = 0;
	int shmid;
	key_t key = SHM_KEY;
	GamePid *game;
	shmid = shmget(key, 1024, IPC_CREAT|0777);
	if(shmid == -1)
	{
		perror("shmget");
	}

	int player = 0;
	system("stty erase ^H");	

	game = (GamePid *)shmat(shmid, NULL, 0);
	strcpy(game->input, "");
	system("clear");
	
	if (argc == 2)
	{
		if(!strcmp(argv[1], "1"))
		{
			player = 1;
			game->game1 = 0;
			game->login[0] = 1;
			printf("PLAYER 1 LogIn\n");
		}
		else if(!strcmp(argv[1], "2"))
		{
			player = 2;
			game->game2 = 0;
			game->login[1] = 1;
			printf("PLAYER 2 LogIn\n");
	
		}
		else if(!strcmp(argv[1], "RESET"))
		{
			game->game1 = 0;
			game->game2 = 0;
			game->login[0] = 0;
			game->login[1] = 0;
			strcpy(game->input, "");
			shmdt(game);
			return 0;
		}
		
		else 
		{
			fprintf(stderr, "USE: CHESS 1\n");
			return 0;
		}
	}
	else 
	{
		fprintf(stderr, "USE: CHESS 1\n");
		return 0;
	}
	
	while(1)
	{
		if(game->login[0] == 1 && game->login[1] == 1)
		{
			break;
		}
	}
	
	printf("Game Start\n");
	 //first piece is white color, white = 1; break = 2;

	//set my piece
	Piece chessboard[8][8] =
	{
		{ {'R',2 }, {'N',2 },{'B',2 },{'Q',2 },{'K',2 },{'B',2 },{'N',2 },{'R',2 } },
		{ {'P',2 }, {'P',2 },{'P',2 },{'P',2 },{'P',2 },{'P',2 },{'P',2 },{'P',2 } },
		{ {'0',0 }, {'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 } },
		{ {'0',0 }, {'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 } },
		{ {'0',0 }, {'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 } },
		{ {'0',0 }, {'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 },{'0',0 } },
		{ {'P',1 }, {'P',1 },{'P',1 },{'P',1 },{'P',1 },{'P',1 },{'P',1 },{'P',1 } },
		{ {'R',1 }, {'N',1 },{'B',1 },{'Q',1 },{'K',1 },{'B',1 },{'N',1 },{'R',1 } }
	};
	game->game1 = 1;
	game->game2 = 0;	
	if(player == 2)
	{
		print_board(chessboard);
		printf("wait other Player\n");
		while(1)
		{
			if(game->game2 == 1 && game->game1 == 0) break;
		}
		
	}

	team = 1;

	printf("Input ex: MOVE P1 E7 E6\n\n");	

	while (team != 0) 
	{
		print_board(chessboard);

		char command[20] = " ";	//입력값을 받을 변수
		char piece[10] = "";
		char from[10] = "";
		char to[10] = "";
		int cmd_count = 0;
		

		printf(" NOW: < Player %d> ", team);
		printf(" CMD ex: \'MOVE\' \'TYPE\' \'FROM\' \'TO\' \n");
		printf(" Input Command: ");

		//입력: move P1 A1 A2
		if(team == player)
			cmd_count = scanf("%s %s %s %s", command, piece, from, to);
		//플레이어마다 입력 변경
		else if(team != player)
			cmd_count = sscanf(game->input, "%s %s %s %s", command, piece, from, to);

		system("clear");
		printf("Player %d, %s, %s, %s\n",team, command, from, to);

		//move를 입력했을 경우
		if (!strcmp(command, "MOVE")) 
		{
			//piece[10]을 인식해서 Piece 자료형으로 변환
			Piece temp = { piece[0],  (int)piece[1]-'0'};

			//from[10]을 인식해서 인덱스값으로 변환
			int from_col = (int)from[0] - 'A';
			int from_row = (int)from[1] - '1';
			
			//to[10]을 인식해서 인덱스값으로 변환
			int to_col = (int)to[0] - 'A';
			int to_row = (int)to[1] - '1';

			//말을 두어도 되는지에 대한 스위치
			int canPlace = 0;

			if (team != chessboard[from_row][from_col].team)
			{
				printf("Opposite team's Piece is not able to place.\n");
				continue;
			}

			if (team == chessboard[to_row][to_col].team)
			{
				printf("There's our team's piece where you're gonna place it.\n");
				continue;
			}

			//from에 있는 값이 piece에 입력한 값과 같으면
			if (chessboard[from_row][from_col].team == temp.team
				&& chessboard[from_row][from_col].type == temp.type)
			{
				//말의 종류에 따라 실행
				switch (temp.type)
				{
				case 'P': //폰이라면
					if (chessboard[to_row][to_col].team == 0)
					{
						if (team == 1)
						{
							if ((from_col == to_col) && ((from_row - to_row) == 1)) // 열이 같고 앞으로만 이동
							{
								printf("\nPawn is able to place\n");
							}

							else
							{
								printf("\nPawn is not able to place\n");
								continue;
							}
						}
						else if (team == 2)
						{
							if ((from_col == to_col) && ((to_row - from_row) == 1)) // 열이 같고 앞으로만 이동
							{
								printf("\nPawn is able to place\n");
							}

							else
							{
								printf("\nPawn is not able to place\n");
								continue;
							}
						}
						else
						{
							printf("Team is not (1 or 2)\n");
							continue;
						}
						
					}
					else if (chessboard[to_row][to_col].team != 0)
					{
						if (team == 1)
						{
							if ((Absol(from_col - to_col) == 1) && ((from_row - to_row) == 1)) // 열이 1개 차이나고 앞으로만 이동
							{
								printf("\nPawn is able to place\n");
							}
							else
							{
								printf("\nPawn is not able to place 1\n");
								continue;
							}
						}
						else if (team == 2)
						{
							if ((Absol(from_col - to_col) == 1) && ((to_row - from_row) == 1)) // 열이 1개 차이나고 앞으로만 이동
							{
								printf("\nPawn is able to place\n");
							}
							else
							{
								printf("\nPawn is not able to place 1\n");
								continue;
							}
						}
						else
						{
							printf("Team is not (1 or 2)\n");
							continue;
						}
					}
					else continue;
					break;

				case 'R': //룩이라면
					canPlace = 1;
					if (from_col == to_col)
					{
						if (from_row > to_row)
						{
							for (int i = to_row + 1; i < from_row; i++)
							{
								if (chessboard[i][from_col].team == 0)
								{

								}
								else
								{
									canPlace = 0;
								}
							}
						}

						else
						{
							for (int i = from_row + 1; i < to_row; i++)
							{
								if (chessboard[i][from_col].team == 0)
								{

								}
								else
								{
									canPlace = 0;
								}
							}
						}
					}
					else if (from_row == to_row)
					{
						if (from_col > to_col)
						{
							for (int i = to_col + 1; i < from_col; i++)
							{
								if (chessboard[from_row][i].team == 0)
								{

								}
								else
								{
									canPlace = 0;
								}
							}
						}

						else
						{
							for (int i = from_col + 1; i < to_col; i++)
							{
								if (chessboard[from_row][i].team == 0)
								{

								}
								else
								{
									canPlace = 0;
								}
							}
						}
					}
					else canPlace = 0;

					if (canPlace)
					{
						printf("\nRook is able to place\n");
					}
					else
					{
						printf("\nRook is not able to place\n");
						continue;
					}
					break;

				case 'B':
					canPlace = 1;
					if (Absol(from_col - to_col) == Absol(from_row - to_row))
					{
						if (from_col < to_col && from_row < to_row)
						{
							for (int i = 1; i < Absol(from_col - to_col); i++)
							{
								if (chessboard[from_row + i][from_col + i].team == 0)
								{

								}

								else
								{
									canPlace = 0;
								}
							}
						}

						else if (from_col < to_col && from_row > to_row)
						{
							for (int i = 1; i < Absol(from_col - to_col); i++)
							{
								if (chessboard[from_row - i][from_col + i].team == 0)
								{

								}

								else
								{
									canPlace = 0;
								}
							}
						}

						else if (from_col > to_col && from_row < to_row)
						{
							for (int i = 1; i < Absol(from_col - to_col); i++)
							{
								if (chessboard[from_row + i][from_col - i].team == 0)
								{

								}

								else
								{
									canPlace = 0;
								}
							}
						}

						else if (from_col > to_col && from_row > to_row)
						{
							for (int i = 1; i < Absol(from_col - to_col); i++)
							{
								if (chessboard[from_row - i][from_col - i].team == 0)
								{

								}

								else
								{
									canPlace = 0;
								}
							}
						}
					}
					else canPlace = 0;

					if (canPlace)
					{
						printf("\nBishop is able to place\n");
					}
					else
					{
						printf("\nBishop is not able to place\n");
						continue;
					}
					break;

				case 'N':
					if ((Absol(from_col - to_col) + Absol(from_row - to_row)) == 3
						&& Higher(Absol(from_col - to_col), Absol(from_row - to_row)) < 3)
					{
						printf("\nKnight is able to place\n");
					}
					else
					{
						printf("\nKnight is not able to place\n");
						continue;
					}
					break;
				
				case 'Q':
					canPlace = 1;
					if (Absol(from_col - to_col) == Absol(from_row - to_row))
					{
						if (from_col < to_col && from_row < to_row)
						{
							for (int i = 1; i < Absol(from_col - to_col); i++)
							{
								if (chessboard[from_row + i][from_col + i].team == 0)
								{

								}

								else
								{
									canPlace = 0;
								}
							}
						}

						else if (from_col < to_col && from_row > to_row)
						{
							for (int i = 1; i < Absol(from_col - to_col); i++)
							{
								if (chessboard[from_row - i][from_col + i].team == 0)
								{

								}

								else
								{
									canPlace = 0;
								}
							}
						}

						else if (from_col > to_col && from_row < to_row)
						{
							for (int i = 1; i < Absol(from_col - to_col); i++)
							{
								if (chessboard[from_row + i][from_col - i].team == 0)
								{

								}

								else
								{
									canPlace = 0;
								}
							}
						}

						else if (from_col > to_col && from_row > to_row)
						{
							for (int i = 1; i < Absol(from_col - to_col); i++)
							{
								if (chessboard[from_row - i][from_col - i].team == 0)
								{

								}

								else
								{
									canPlace = 0;
								}
							}
						}
					}
					else if (from_col == to_col)
					{
						if (from_row > to_row)
						{
							for (int i = to_row + 1; i < from_row; i++)
							{
								if (chessboard[i][from_col].team == 0)
								{

								}
								else
								{
									canPlace = 0;
								}
							}
						}

						else
						{
							for (int i = from_row + 1; i < to_row; i++)
							{
								if (chessboard[i][from_col].team == 0)
								{

								}
								else
								{
									canPlace = 0;
								}
							}
						}
					}
					else if (from_row == to_row)
					{
						if (from_col > to_col)
						{
							for (int i = to_col + 1; i < from_col; i++)
							{
								if (chessboard[from_row][i].team == 0)
								{

								}
								else
								{
									canPlace = 0;
								}
							}
						}

						else
						{
							for (int i = from_col + 1; i < to_col; i++)
							{
								if (chessboard[from_row][i].team == 0)
								{

								}
								else
								{
									canPlace = 0;
								}
							}
						}
					}
					else canPlace = 0;

					if (canPlace)
					{
						printf("\nQueen is able to place\n");
					}
					else
					{
						printf("\nQueen is not able to place\n");
						continue;
					}
					break;

				case 'K':
					if(Higher(Absol(from_col - to_col), Absol(from_row - to_row)) < 2)
						printf("\nKing is able to place\n");
					else
					{
						printf("\nKing is not able to place\n");
						continue;
					}
					break;
				default:
					printf("Not Reconizing Commad\n");
					continue;
					break;
				}

				if (chessboard[to_row][to_col].type == 'K')
				{
					if (chessboard[to_row][to_col].team != team)
					{
						winFlag = team;
						break;
					}
				}

				//place piece
				chessboard[to_row][to_col].team = temp.team;
				chessboard[to_row][to_col].type = temp.type;
				chessboard[from_row][from_col].team = 0;
				chessboard[from_row][from_col].type = '0';
			}
		}

		else
		{
			printf("command error\n");
			continue;
		}
		
		if(winFlag != 0)
		{
			system("clear");
			print_board(chessboard);
			if(winFlag!=0)
			{
				if(winFlag == 1)
				{
					printf("\n\n PLAYER 1 WIN\n\n");
				}
				if(winFlag == 2)
				{
					printf("\n\n PLAYER 2 WIN\n\n");
				}
				printf("\nends in 5 seconds\n");
				sleep(5);
				return 0;
			}
		}

		//success command
		printf("Player %d: Lately Command: %s %s %s\n",team, command, from, to);
		
		sprintf(game->input, "%s %s %s %s", command, piece, from, to);
			
		//팀을 바꾸고 다른 프로세스의 루프를 해제,
		if(player == team)
		{ 
			printf("team Chainge\n");
			if(team == 1)
			{
				game->game1 = 0;
				game->game2 = 1;

			}
			else if(team == 2)
			{
				game->game1 = 1;
				game->game2 = 0;
			}
			else fprintf(stderr, "What team are you on?\n");
			
			print_board(chessboard);
			printf("Wait Other Player\n"); 
			
			while(1)
			{
				if(team == 1)
				{
					
					if(game->game1 == 1) break;
					
				}

				else if(team == 2)
				{
				
					if(game->game2 == 1) break;
			
				}
			}
		}
		
		if (team == 1) team = 2;
		else if (team == 2) team = 1;
		else fprintf(stderr, "WHay?");
		
	}
	shmdt(game);
}



