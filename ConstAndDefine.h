#pragma once

//Const
#define MAX_LOADSTRING 100
#define RAND 350
#define GAMEOVERX 270
#define GAMEOVERY 230



//Setting
#define WM_SENDLEVEL		           WM_USER + 100
#define WM_SENDHERONAME		           WM_USER + 101
#define IDTIMER_SPEED  1


//Define
char szSpeed_T[100] = "SPEED";
char szCurrentSpeed[100] = "Speed";
char szHeadPosX_T[100] = "HEAD X";
char szHeadPosX[100] = "Head X";
char szHeadPosY_T[100] = "HEAD Y";
char szHeadPosY[100] = "Head Y";
char szBodyLength_T[100] = "SNAKE'S LENGTH";
char szBodyLength[100] = "The Length Of Snake";

char szHeroName_T[100] = "HEROES";
char szChampionName[100] = "Champion";
char szSecondName[100] = "Runner-up";
char szThirdName[100] = "Second Runner-up";

char szList_HeroS[100] = "SCORES";
char szChampionScore[100] = "First Place";
char szSecondScore[100] = "Second Place";
char szThirdScore[100] = "Third Place";

RECT rMoveRange = { 65, 85, 437, 437 };//Game Range