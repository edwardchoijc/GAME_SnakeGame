// SnakeGame.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SnakeGame.h"
#include "ConstAndDefine.h"


// Global Variables:
HINSTANCE hInst;                                // current instance
HWND ghWnd;                                     
TCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

char szFileName[100];
char szCurrentDirt[100];
char szScore[10] = "0";
char szTempName[100] = "???";
UINT wCurrentScore = 0;

static enum Level {level1 = 1, level2, level3, level4, level5, 
                   level6, level7, level8, level9, level10};


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    AddRecord(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    Armory(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    Difficulty(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    Help(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    Info(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SNAKEGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SNAKEGAME));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMEICON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SNAKEGAME);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GAMEICON));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_VISIBLE | WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU,
      100, 100, 800, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ghWnd = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Image
	  //Normal
	static HICON hIconHead, hIconFood, hIconBody;

	  //Special
	    //Body
	static HICON hIconBodyVertical, hIconBodyLevel;
	    //Head
	static HICON hIconHeadUp, hIconHeadDown, hIconHeadLeft, hIconHeadRight;
	static HICON hIconHeadUp_Dead, hIconHeadDown_Dead, hIconHeadLeft_Dead, hIconHeadRight_Dead;

	  //Score
	static HICON hIconScore0, hIconScore1, hIconScore2, hIconScore3, hIconScore4,
		         hIconScore5, hIconScore6, hIconScore7, hIconScore8, hIconScore9;
	  //Others
	static HBITMAP hBmpBackground, hBmpGameOver;

	//Struct
	static enum Direct { right, left, up, down } CurrentDirection = right;
	static struct FoodPos
	{
		UINT	i;
		FoodPos	*pNext;
		Direct	CurrentDirection;
	}	*pFoodInHead, *pFoodInBody;

	//Point
	static POINT poFoodPos = { 190, 190 };
	static POINT poSnakePos[100] = { 100, 95 };
	
	//Bool
	static BOOL bPause    = FALSE;
	static BOOL bAddBody  = FALSE;
	static BOOL bEatFood  = FALSE;
	static BOOL bSpace    = FALSE;
	static BOOL bGameOver = FALSE;

	//Others
	static UINT wBodyLength = 5;
	UINT i=0, j=0;
	UINT wStep = 13;
	static UINT wSpeed = 500;


    switch (message)
    {
	case WM_CREATE:
	    {
			//Load the image of the body
			hIconBody         = LoadIcon(hInst, MAKEINTRESOURCE(IDI_BODY));
			hIconBodyLevel    = LoadIcon(hInst, MAKEINTRESOURCE(IDI_BODYLEVEL));
			hIconBodyVertical = LoadIcon(hInst, MAKEINTRESOURCE(IDI_BODYVERTICAL));

			//Load the image of the head
			hIconHeadUp         = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HEADUP));
			hIconHeadDown       = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HEADDOWN));
			hIconHeadLeft       = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HEADLEFT));
			hIconHeadRight      = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HEADRIGHT));
			hIconHeadUp_Dead    = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HEADUP_DEAD));
			hIconHeadDown_Dead  = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HEADDOWN_DEAD));
			hIconHeadLeft_Dead  = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HEADLEFT_DEAD));
			hIconHeadRight_Dead = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HEADRIGHT_DEAD));

			//Load the image of food
			hIconFood = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPLE));

			//Load the image of gameover 
			hBmpGameOver = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_GAMEOVER));

			//Load the image of background
			hBmpBackground = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BACKGROUND));

			//Load the image of Score
			hIconScore0 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCORE0));
			hIconScore1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCORE1));
			hIconScore2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCORE2));
			hIconScore3 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCORE3));
			hIconScore4 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCORE4));
			hIconScore5 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCORE5));
			hIconScore6 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCORE6));
			hIconScore7 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCORE7));
			hIconScore8 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCORE8));
			hIconScore9 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCORE9));

			//Initial the chain of shitinbody
			pFoodInHead = NULL;
			pFoodInBody = pFoodInHead;

			//Find the ini file
			GetCurrentDirectoryA(sizeof(szCurrentDirt), szCurrentDirt);
			strcpy(szFileName, szCurrentDirt);
			strcat(szFileName, "\\snake.ini");

			WIN32_FIND_DATAA sTemp;
			HANDLE hFile = FindFirstFileA(szFileName, &sTemp);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				if (!WritePrivateProfileStringA(szSpeed_T, szCurrentSpeed, "500", szFileName))
					MessageBoxA(hWnd, "错误", "打不开配置文件", MB_OK);
				if (!WritePrivateProfileStringA(szHeadPosX_T, szHeadPosX, "100", szFileName))
					MessageBoxA(hWnd, "错误", "打不开配置文件", MB_OK);
				if (!WritePrivateProfileStringA(szHeadPosY_T, szHeadPosY, "95", szFileName))
					MessageBoxA(hWnd, "错误", "打不开配置文件", MB_OK);
				if (!WritePrivateProfileStringA(szBodyLength_T, szBodyLength, "5", szFileName))
					MessageBoxA(hWnd, "错误", "打不开配置文件", MB_OK);

				//Write hero
				if (!WritePrivateProfileStringA(szHeroName_T, szChampionName, "???", szFileName))
					MessageBoxA(hWnd, "错误", "打不开配置文件", MB_OK);
				if (!WritePrivateProfileStringA(szHeroName_T, szSecondName, "???", szFileName))
					MessageBoxA(hWnd, "错误", "打不开配置文件", MB_OK);
				if (!WritePrivateProfileStringA(szHeroName_T, szThirdName, "???", szFileName))
					MessageBoxA(hWnd, "错误", "打不开配置文件", MB_OK);
				if (!WritePrivateProfileStringA(szList_HeroS, szChampionScore, "0", szFileName))
					MessageBoxA(hWnd, "错误", "打不开配置文件", MB_OK);
				if (!WritePrivateProfileStringA(szList_HeroS, szSecondScore, "0", szFileName))
					MessageBoxA(hWnd, "错误", "打不开配置文件", MB_OK);
				if (!WritePrivateProfileStringA(szList_HeroS, szThirdScore, "0", szFileName))
					MessageBoxA(hWnd, "错误", "打不开配置文件", MB_OK);
			}

			char szTemp[100];

			//Get Speed from ini;
			GetPrivateProfileStringA(szSpeed_T, szCurrentSpeed, "0", szTemp, sizeof(szTemp), szFileName);
			wSpeed = atoi(szTemp);

			//Get Position of Snake's head at the first time from ini;
			GetPrivateProfileStringA(szHeadPosX_T, szHeadPosX, "0", szTemp, sizeof(szTemp), szFileName);
			poSnakePos[0].x = atoi(szTemp);
			GetPrivateProfileStringA(szHeadPosY_T, szHeadPosY, "0", szTemp, sizeof(szTemp), szFileName);
			poSnakePos[0].y = atoi(szTemp);

			//Get the length of snake;
			GetPrivateProfileStringA(szBodyLength_T, szBodyLength, "0", szTemp, sizeof(szTemp), szFileName);
			wBodyLength = atoi(szTemp);

			//Initial the Score
			strcpy(szScore, "0");
			wCurrentScore = 0;

			//Initial the body of the snake
			for (i = 1; i <= wBodyLength; i++)
			{
				poSnakePos[i].x = poSnakePos[0].x - i * 13;
				poSnakePos[i].y = poSnakePos[0].y;
			}
	     }
		break;
	case WM_SENDLEVEL:
	    {
			UINT wLevel = (UINT)wParam;
			switch (wLevel)
			{
			case level1:
				wSpeed = 500;
				break;
			case level2:
				wSpeed = 300;
				break;
			case level3:
				wSpeed = 200;
				break;
			case level4:
				wSpeed = 160;
				break;
			case level5:
				wSpeed = 120;
				break;
			case level6:
				wSpeed = 100;
				break;
			case level7:
				wSpeed = 80;
				break;
			case level8:
				wSpeed = 50;
				break;
			case level9:
				wSpeed = 30;
				break;
			case level10:
				wSpeed = 10;
				break;
			}

			char szTemp[100];
			if (!WritePrivateProfileStringA(szSpeed_T, szCurrentSpeed, itoa(wSpeed, szTemp, 10), szFileName))
				MessageBoxA(hWnd, "错误", "打不开配置文件", MB_OK);
	    }
		break;
	case WM_TIMER:
	    {
			RECT ClientRect;
			GetClientRect(hWnd, &ClientRect);

			if (wParam == IDTIMER_SPEED && bGameOver == FALSE && bPause == FALSE)
		{
			HMENU hMenu = GetMenu(hWnd);
			HMENU hMenuSub = GetSubMenu(hMenu, 0);

			//Whether the game is over?
				//When the snake bump into the wall, game over
				if ((poSnakePos[0].x - rMoveRange.left <= 1) || (poSnakePos[0].y - rMoveRange.top <= 1) || (rMoveRange.right - poSnakePos[0].x < 1) || (rMoveRange.bottom - poSnakePos[0].y < 1))
				{
					KillTimer(hWnd, IDTIMER_SPEED);
					bGameOver = TRUE;
					PlaySound((LPCTSTR)IDR_GAMEOVER, NULL, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
					EnableMenuItem(hMenuSub, IDM_PAURES, MF_BYCOMMAND | MF_GRAYED);

					//Whether break the record?
					char szFirstScore[100], szSecondScore[100], szThirdScore[100];
					char szFirstName[100], szSecondName[100], szThirdName[100];
					char szTemp[100];
					UINT wFirst = 0, wSecond = 0, wThird = 0;

					GetPrivateProfileStringA(szList_HeroS, szChampionScore, "0", szFirstScore, sizeof(szFirstScore), szFileName);
					wFirst = atoi(szFirstScore);
					GetPrivateProfileStringA(szList_HeroS, szSecondScore, "0", szSecondScore, sizeof(szSecondScore), szFileName);
					wSecond = atoi(szSecondScore);
					GetPrivateProfileStringA(szList_HeroS, szThirdScore, "0", szThirdScore, sizeof(szThirdScore), szFileName);
					wThird = atoi(szThirdScore);

					GetPrivateProfileStringA(szHeroName_T, szChampionName, "???", szFirstName, sizeof(szFirstName), szFileName);
					GetPrivateProfileStringA(szHeroName_T, szSecondName, "???", szSecondName, sizeof(szSecondName), szFileName);
					GetPrivateProfileStringA(szHeroName_T, szThirdName, "???", szThirdName, sizeof(szThirdName), szFileName);

					if (wCurrentScore * 100 > wFirst)
					{
						if (DialogBox(hInst, (LPCTSTR)IDD_ADDRECORD, hWnd, (DLGPROC)AddRecord) == IDOK)
						{
							//Set the second's record to third
							WritePrivateProfileStringA(szHeroName_T, szThirdName, szSecondName, szFileName);
							WritePrivateProfileStringA(szList_HeroS, szThirdScore, szSecondScore, szFileName);

							//Set the first's recod to second
							WritePrivateProfileStringA(szHeroName_T, szSecondName, szFirstName, szFileName);
							WritePrivateProfileStringA(szList_HeroS, szSecondScore, szFirstScore, szFileName);

							//Set  the new record to first place
							WritePrivateProfileStringA(szList_HeroS, szChampionScore, itoa(wCurrentScore * 100, szTemp, 10), szFileName);
							WritePrivateProfileStringA(szHeroName_T, szChampionName, szTempName, szFileName);

							SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_ARMORY, 0);
						}
					}
					else if (wCurrentScore * 100 > wSecond)
					{
						if (DialogBox(hInst, (LPCTSTR)IDD_ADDRECORD, hWnd, (DLGPROC)AddRecord) == IDOK)
						{
							//Set the second's record to third
							WritePrivateProfileStringA(szHeroName_T, szThirdName, szSecondName, szFileName);
							WritePrivateProfileStringA(szList_HeroS, szThirdScore, szSecondScore, szFileName);

							//Set  the new record to Second place
							WritePrivateProfileStringA(szList_HeroS, szSecondScore, itoa(wCurrentScore * 100, szTemp, 10), szFileName);
							WritePrivateProfileStringA(szHeroName_T, szSecondName, szTempName, szFileName);

							SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_ARMORY, 0);
						}
					}
					else if (wCurrentScore * 100 > wThird)
					{
						if (DialogBox(hInst, (LPCTSTR)IDD_ADDRECORD, hWnd, (DLGPROC)AddRecord) == IDOK)
						{
							//Set  the new record to Third place
							WritePrivateProfileStringA(szList_HeroS, szThirdScore, itoa(wCurrentScore * 100, szTemp, 10), szFileName);
							WritePrivateProfileStringA(szHeroName_T, szThirdName, szTempName, szFileName);

							SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_ARMORY, 0);
						}
					}

					RECT rt;
					rt.left = 190;
					rt.right = 659;
					rt.top = 190;
					rt.bottom = 380;
					InvalidateRect(hWnd, &rt, FALSE);

					rt.left = poSnakePos[1].x - 10;
					rt.right = rt.left + 50;
					rt.top = poSnakePos[1].y - 10;
					rt.bottom = rt.top + 50;
					InvalidateRect(hWnd, &rt, FALSE);

					RECT rt1;
					rt1.left = GAMEOVERX;
					rt1.top = GAMEOVERY;
					rt1.bottom = rt1.top + 87;
					rt1.right = rt1.left + 270;
					InvalidateRect(hWnd, &rt1, FALSE);
				}

			//After ate, the snake's body should change
			if (bAddBody == TRUE)
			{
				//Add the length of body
				wBodyLength += 1;

				//delete 1st element
				FoodPos* pTemp;
				pTemp = pFoodInHead;
				pFoodInHead = pFoodInHead->pNext;
				pFoodInBody = pFoodInHead;
				delete(pTemp);

				bAddBody = FALSE;
			}

			//Then this is how the body move
			for (i = wBodyLength; i > 0; i--)
				poSnakePos[i] = poSnakePos[i-1];

			//The operation of each direction to move on
			switch (CurrentDirection)
			{
			case right:
				poSnakePos[0].x += wStep;
				break;
			case left:
				poSnakePos[0].x -= wStep;
				break;
			case up:
				poSnakePos[0].y -= wStep;
				break;
			case down:
				poSnakePos[0].y += wStep;
				break;
			default:
				break;
			}

			//When the snake bump into itself, game over
			for (i = 1; i <= wBodyLength; i++)
			{
				if (poSnakePos[0].x == poSnakePos[i].x && poSnakePos[0].y == poSnakePos[i].y)
				{
					KillTimer(hWnd, IDTIMER_SPEED);
					bGameOver = TRUE;
					PlaySound((LPCTSTR)IDR_GAMEOVER, NULL, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
					EnableMenuItem(hMenuSub, IDM_PAURES, MF_BYCOMMAND | MF_GRAYED);

					//Whether break the record?
					char szFirstScore[100], szSecondScore[100], szThirdScore[100];
					char szFirstName[100], szSecondName[100], szThirdName[100];
					char szTemp[100];
					UINT wFirst = 0, wSecond = 0, wThird = 0;

					GetPrivateProfileStringA(szList_HeroS, szChampionScore, "0", szFirstScore, sizeof(szFirstScore), szFileName);
					wFirst = atoi(szFirstScore);
					GetPrivateProfileStringA(szList_HeroS, szSecondScore, "0", szSecondScore, sizeof(szSecondScore), szFileName);
					wSecond = atoi(szSecondScore);
					GetPrivateProfileStringA(szList_HeroS, szThirdScore, "0", szThirdScore, sizeof(szThirdScore), szFileName);
					wThird = atoi(szThirdScore);

					GetPrivateProfileStringA(szHeroName_T, szChampionName, "???", szFirstName, sizeof(szFirstName), szFileName);
					GetPrivateProfileStringA(szHeroName_T, szSecondName, "???", szSecondName, sizeof(szSecondName), szFileName);
					GetPrivateProfileStringA(szHeroName_T, szThirdName, "???", szThirdName, sizeof(szThirdName), szFileName);

					if (wCurrentScore * 100 > wFirst)
					{
						if (DialogBox(hInst, (LPCTSTR)IDD_ADDRECORD, hWnd, (DLGPROC)AddRecord) == IDOK)
						{
							//Set the second's record to third
							WritePrivateProfileStringA(szHeroName_T, szThirdName, szSecondName, szFileName);
							WritePrivateProfileStringA(szList_HeroS, szThirdScore, szSecondScore, szFileName);

							//Set the first's recod to second
							WritePrivateProfileStringA(szHeroName_T, szSecondName, szFirstName, szFileName);
							WritePrivateProfileStringA(szList_HeroS, szSecondScore, szFirstScore, szFileName);

							//Set  the new record to first place
							WritePrivateProfileStringA(szList_HeroS, szChampionScore, itoa(wCurrentScore * 100, szTemp, 10), szFileName);
							WritePrivateProfileStringA(szHeroName_T, szChampionName, szTempName, szFileName);

							SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_ARMORY, 0);
						}
					}
					else if (wCurrentScore * 100 > wSecond)
					{
						if (DialogBox(hInst, (LPCTSTR)IDD_ADDRECORD, hWnd, (DLGPROC)AddRecord) == IDOK)
						{
							//Set the second's record to third
							WritePrivateProfileStringA(szHeroName_T, szThirdName, szSecondName, szFileName);
							WritePrivateProfileStringA(szList_HeroS, szThirdScore, szSecondScore, szFileName);

							//Set  the new record to Second place
							WritePrivateProfileStringA(szList_HeroS, szSecondScore, itoa(wCurrentScore * 100, szTemp, 10), szFileName);
							WritePrivateProfileStringA(szHeroName_T, szSecondName, szTempName, szFileName);

							SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_ARMORY, 0);
						}
					}
					else if (wCurrentScore * 100 > wThird)
					{
						if (DialogBox(hInst, (LPCTSTR)IDD_ADDRECORD, hWnd, (DLGPROC)AddRecord) == IDOK)
						{
							//Set  the new record to Third place
							WritePrivateProfileStringA(szList_HeroS, szThirdScore, itoa(wCurrentScore * 100, szTemp, 10), szFileName);
							WritePrivateProfileStringA(szHeroName_T, szThirdName, szTempName, szFileName);

							SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_ARMORY, 0);
						}
					}

					RECT rt;
					rt.left = 190;
					rt.right = 659;
					rt.top = 190;
					rt.bottom = 380;
					InvalidateRect(hWnd, &rt, FALSE);

					rt.left = poSnakePos[1].x - 10;
					rt.right = rt.left + 50;
					rt.top = poSnakePos[1].y - 10;
					rt.bottom = rt.top + 50;
					InvalidateRect(hWnd, &rt, FALSE);

					RECT rt1;
					rt1.left = GAMEOVERX;
					rt1.top = GAMEOVERY;
					rt1.bottom = rt1.top + 87;
					rt1.right = rt1.left + 270;
					InvalidateRect(hWnd, &rt1, FALSE);
					break;
				}
			}

			if (bGameOver == TRUE)
				break;

			//Decide whether the food should be refresh
			if (abs(poSnakePos[1].x - poFoodPos.x) < 7 && abs(poSnakePos[1].y - poFoodPos.y) < 7)
			{
				bEatFood = TRUE;
				//PlaySound((LPCTSTR)IDR_EAT, NULL, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT | SND_NOWAIT);

				//After snake ate food, its body is different
				if (pFoodInHead == NULL)
				{
					pFoodInHead = new FoodPos[1];
					pFoodInBody = pFoodInHead;
					pFoodInBody->i = 0;
					pFoodInBody->CurrentDirection = CurrentDirection;
					pFoodInBody->pNext = NULL;
				}
				else
				{
					//Food that eaten move from head to tail
					pFoodInBody = pFoodInHead;

					while (pFoodInBody->pNext != NULL)
					{
						pFoodInBody = pFoodInBody->pNext;
					}

					pFoodInBody->pNext = new FoodPos[1];
					pFoodInBody = pFoodInBody->pNext;
					pFoodInBody->i = 0;
					pFoodInBody->CurrentDirection = CurrentDirection;
					pFoodInBody->pNext = NULL;
				}

				//Increase the score
				UINT wLevel;
				switch (wSpeed)
				{
				case 500:
				{
					wLevel = level1;
				}
				break;
				case 300:
				{
					wLevel = level2;
				}
				break;
				case 200:
				{
					wLevel = level3;
				}
				break;
				case 160:
				{
					wLevel = level4;
				}
				break;
				case 120:
				{
					wLevel = level5;
				}
				break;
				case 100:
				{
					wLevel = level6;
				}
				break;
				case 80:
				{
					wLevel = level7;
				}
				break;
				case 50:
				{
					wLevel = level8;
				}
				break;
				case 30:
				{
					wLevel = level9;
				}
				break;
				case 10:
				{
					wLevel = level10;
				}
				break;
				}
				wCurrentScore += wLevel;
				itoa(wCurrentScore, szScore, 10);

				//Update level?
				if (wLevel == level1 && wCurrentScore / wLevel >= 10)   wLevel++;
				if (wLevel == level2 && wCurrentScore / wLevel >= 17)   wLevel++;
				if (wLevel == level3 && wCurrentScore / wLevel >= 23)   wLevel++;
				if (wLevel == level4 && wCurrentScore / wLevel >= 28)   wLevel++;
				if (wLevel == level5 && wCurrentScore / wLevel >= 32)   wLevel++;
				if (wLevel == level6 && wCurrentScore / wLevel >= 35)   wLevel++;
				if (wLevel == level7 && wCurrentScore / wLevel >= 37)   wLevel++;
				if (wLevel == level8 && wCurrentScore / wLevel >= 39)   wLevel++;
				if (wLevel == level9 && wCurrentScore / wLevel >= 40)   wLevel++;
			}

			//Every part of the body will follow the change of the head
			pFoodInBody = pFoodInHead;
			while (pFoodInBody != NULL)
			{
				pFoodInBody->i++;
				pFoodInBody = pFoodInBody->pNext;
			}

			if (pFoodInHead != NULL)
				if (pFoodInHead->i == wBodyLength)
					bAddBody = TRUE;

			//Refresh the food pos
			if (bEatFood == TRUE)
			{
				srand(time(0));
				poFoodPos.x = rand() % RAND + 70;
				poFoodPos.y = rand() % RAND + 90;
				bEatFood = FALSE;

				RECT rt;
				rt.left = poFoodPos.x;
				rt.top = poFoodPos.y;
				rt.right = poFoodPos.x + 32;
				rt.bottom = poFoodPos.y + 32;
				InvalidateRect(hWnd, &rt, FALSE);
			}

			//Decide the minisized rectange to refresh
			RECT rt, rt1;
			POINT poMax, poMin;

			poMin = poSnakePos[0];
			poMax = poMin;

			for (i = 0; i <= wBodyLength; i++)
			{
				if (poSnakePos[i].x < poMin.x)
					poMin.x = poSnakePos[i].x;
				if (poSnakePos[i].x > poMin.x)
					poMax.x = poSnakePos[i].x;
				if (poSnakePos[i].y < poMin.y)
					poMin.y = poSnakePos[i].y;
				if (poSnakePos[i].y > poMin.y)
					poMax.y = poSnakePos[i].y;
			}
			rt.left = poMin.x - 30;
			rt.right = poMax.x + 32 + 30;
			rt.top = poMin.y - 30;
			rt.bottom = poMax.y + 32 + 30;

			InvalidateRect(hWnd, &rt, FALSE);
			InvalidateRect(hWnd, &rMoveRange, FALSE);

			rt1.left = 550;
			rt1.top = 55;
			rt1.right = 750;
			rt1.bottom = 95;
			InvalidateRect(hWnd, &rt1, FALSE);

			UpdateWindow(hWnd);
		}
	    }
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDM_START:
			    {
					//Start to play the music
					PlaySound((LPCTSTR)IDR_BGM, NULL, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT | SND_LOOP);

					char szTemp[100];

					//Get Speed from ini;
					GetPrivateProfileStringA(szSpeed_T, szCurrentSpeed, "0", szTemp, sizeof(szTemp), szFileName);
					wSpeed = atoi(szTemp);

					//Get Position of Snake's head at the first time from ini;
					GetPrivateProfileStringA(szHeadPosX_T, szHeadPosX, "0", szTemp, sizeof(szTemp), szFileName);
					poSnakePos[0].x = atoi(szTemp);
					GetPrivateProfileStringA(szHeadPosY_T, szHeadPosY, "0", szTemp, sizeof(szTemp), szFileName);
					poSnakePos[0].y = atoi(szTemp);

					//Get the length of snake;
					GetPrivateProfileStringA(szBodyLength_T, szBodyLength, "0", szTemp, sizeof(szTemp), szFileName);
					wBodyLength = atoi(szTemp);

					//Initial the Score
					strcpy(szScore, "0");
					wCurrentScore = 0;

					bPause = FALSE;
					bGameOver = FALSE;

					HMENU hMenu, hMenuSub;
					hMenu = GetMenu(hWnd);
					hMenuSub = GetSubMenu(hMenu, 0);

					ModifyMenuA(hMenuSub, IDM_PAURES, MF_BYCOMMAND | MF_STRING, IDM_PAURES, "Pause(&P)       Space");
					ModifyMenuA(hMenuSub, IDM_START, MF_BYCOMMAND | MF_STRING, IDM_START, "Restart(&R)         Enter");

					//Initial the body of the snake
					for (i = 1; i <= wBodyLength; i++)
					{
						poSnakePos[i].x = poSnakePos[0].x - i * 13;
						poSnakePos[i].y = poSnakePos[0].y;
					}

					while (pFoodInHead != NULL)
					{
						FoodPos *pFoodAte;
						pFoodAte = pFoodInHead;
						pFoodInHead = pFoodInHead->pNext;
						delete(pFoodAte);
					}

					CurrentDirection = right;
					SetTimer(hWnd, IDTIMER_SPEED, wSpeed, NULL);

					InvalidateRect(hWnd, NULL, FALSE);
					UpdateWindow(hWnd);
			    }
                break;
			case IDM_PAURES:
			    {
					//Change the menu--game's state
					HMENU hMenu, hMenuSub;
					hMenu = GetMenu(hWnd);
					hMenuSub = GetSubMenu(hMenu, 0);

					if (bGameOver == FALSE)
					{
						if (bPause == FALSE)
							ModifyMenuA(hMenuSub, IDM_PAURES, MF_BYCOMMAND | MF_STRING, IDM_PAURES, "Resume");
						else
							ModifyMenuA(hMenuSub, IDM_PAURES, MF_BYCOMMAND | MF_STRING, IDM_PAURES, "Pause");

						//Simulate the input of Space
						SendMessage(hWnd, WM_KEYDOWN, VK_SPACE, 0);
						SendMessage(hWnd, WM_KEYUP, VK_SPACE, 0);
					}
			    }
				break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDM_ARMORY:
			    {
					//Whether the game pause?
					//If not, first pause the game
					if (bPause == FALSE && bGameOver == FALSE)
					{
						SendMessage(hWnd, WM_KEYDOWN, VK_SPACE, 0);
						SendMessage(hWnd, WM_KEYUP, VK_SPACE, 0);
						bSpace = TRUE;
					}

					DialogBox(hInst, (LPCTSTR)IDD_ARMORY, hWnd, (DLGPROC)Armory);

					//Then resume the game
					if (bSpace == TRUE && bGameOver == FALSE)
					{
						SendMessage(hWnd, WM_KEYDOWN, VK_SPACE, 0);
						SendMessage(hWnd, WM_KEYUP, VK_SPACE, 0);
						bSpace = FALSE;
					}
			    }
				break;
			case IDM_DIFFICULTY:
			    {
					//Whether the game pause?
					//If not, first pause the game
					if (bPause == FALSE && bGameOver == FALSE)
					{
						SendMessage(hWnd, WM_KEYDOWN, VK_SPACE, 0);
						SendMessage(hWnd, WM_KEYUP, VK_SPACE, 0);
						bSpace = TRUE;
					}

					if (DialogBox(hInst, (LPCTSTR)IDD_DIFFICULTY, hWnd, (DLGPROC)Difficulty) == IDOK)
						SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_START, 0);
					else
					{
						//Then resume the game
						if (bSpace == TRUE && bGameOver == FALSE)
						{
							SendMessage(hWnd, WM_KEYDOWN, VK_SPACE, 0);
							SendMessage(hWnd, WM_KEYUP, VK_SPACE, 0);
							bSpace = FALSE;
						}
					}
			    }
				break;
			case IDM_HELP:
			    {
					//Whether the game pause?
					//If not, first pause the game
					if (bPause == FALSE && bGameOver == FALSE)
					{
						SendMessage(hWnd, WM_KEYDOWN, VK_SPACE, 0);
						SendMessage(hWnd, WM_KEYUP, VK_SPACE, 0);
						bSpace = TRUE;
					}

					DialogBox(hInst, (LPCTSTR)IDD_HELP, hWnd, (DLGPROC)Help);

					//Then resume the game
					if (bSpace == TRUE && bGameOver == FALSE)
					{
						SendMessage(hWnd, WM_KEYDOWN, VK_SPACE, 0);
						SendMessage(hWnd, WM_KEYUP, VK_SPACE, 0);
						bSpace = FALSE;
					}
			    }
				break;
			case IDM_ABOUT:
			   {
				   //Whether the game pause?
				   //If not, first pause the game
				   if (bPause == FALSE && bGameOver == FALSE)
				   {
					   SendMessage(hWnd, WM_KEYDOWN, VK_SPACE, 0);
					   SendMessage(hWnd, WM_KEYUP, VK_SPACE, 0);
					   bSpace = TRUE;
				   }

				   DialogBox(hInst, (LPCTSTR)IDD_INFO, hWnd, (DLGPROC)Info);

				   //Then resume the game
				   if (bSpace == TRUE && bGameOver == FALSE)
				   {
					   SendMessage(hWnd, WM_KEYDOWN, VK_SPACE, 0);
					   SendMessage(hWnd, WM_KEYUP, VK_SPACE, 0);
					   bSpace = FALSE;
				   }
			    }
			   break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_KEYDOWN:
	    {
			if (wParam == VK_RETURN)//Start the game
			{
				SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_START, 0);
				break;
			}

			if (bGameOver == FALSE)
			{
				switch (wParam)
				{
				case VK_SPACE://Pause and Resume   
				    {
						HMENU hMenu, hMenuSub;
						hMenu = GetMenu(hWnd);
						hMenuSub = GetSubMenu(hMenu, 0);

						if (bPause == FALSE)
						{
							ModifyMenuA(hMenuSub, IDM_PAURES, MF_BYCOMMAND | MF_STRING, IDM_PAURES, "Resume(&R)      Space");
							PlaySound(NULL, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_LOOP);
							bPause = TRUE;
						}
						else
						{
							ModifyMenuA(hMenuSub, IDM_PAURES, MF_BYCOMMAND | MF_STRING, IDM_PAURES, "Pause(&P)       Space");
							PlaySound((LPCTSTR)IDR_BGM, NULL, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT | SND_LOOP);
							bPause = FALSE;
						}
				    }
					break;
				case VK_UP:
				    {
						if (bPause == TRUE || CurrentDirection == down)
							MessageBeep(0);
						else
							CurrentDirection = up;
				    }
					break;
				case VK_DOWN:
				    {
						if (bPause == TRUE || CurrentDirection == up)
							MessageBeep(0);
						else
							CurrentDirection = down;
				    }
					break;
				case VK_LEFT:
				    {
						if (bPause == TRUE || CurrentDirection == right)
							MessageBeep(0);
						else
							CurrentDirection = left;
				    }
					break;
				case VK_RIGHT:
				    {
						if (bPause == TRUE || CurrentDirection == left)
							MessageBeep(0);
						else
							CurrentDirection = right;
				    }
				    break;
				default:  
					break;
				}
			}
	    }
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			HDC hdcmem = CreateCompatibleDC(hdc);
			BITMAP bm;

            // Draw the background pic
			SelectObject(hdcmem, hBmpBackground);
			GetObject(hBmpBackground, sizeof(bm), (LPSTR)&bm);
			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcmem, 0, 0, SRCCOPY);
			DeleteDC(hdcmem);

			//Draw the food
			DrawIcon(hdc, poFoodPos.x, poFoodPos.y, hIconFood);

			//Draw the head
			    //First judge the direction
			if (CurrentDirection == right)
			{
				if (bGameOver == TRUE)
					hIconHead = hIconHeadRight_Dead;
				else
					hIconHead = hIconHeadRight;
			}
			if (CurrentDirection == down)
			{
				if (bGameOver == TRUE)
					hIconHead = hIconHeadDown_Dead;
				else
					hIconHead = hIconHeadDown;
			}
			if (CurrentDirection == left)
			{
				if (bGameOver == TRUE)
					hIconHead = hIconHeadLeft_Dead;
				else
					hIconHead = hIconHeadLeft;
			}
			if (CurrentDirection == up)
			{
				if (bGameOver == TRUE)
					hIconHead = hIconHeadUp_Dead;
				else
					hIconHead = hIconHeadUp;
			}
			    //Then draw it
			DrawIcon(hdc, poSnakePos[0].x, poSnakePos[0].y, hIconHead);

			//Draw the body of snake
			    //Normal body
			for (i = 1; i <= wBodyLength; i++)
				DrawIcon(hdc, poSnakePos[i].x, poSnakePos[i].y, hIconBody);
			    //The part of the body where food in is different
			pFoodInBody = pFoodInHead;
			while (pFoodInBody != NULL)
			{
				if (pFoodInBody->CurrentDirection == right || pFoodInBody->CurrentDirection == left)
					DrawIcon(hdc, poSnakePos[pFoodInBody->i].x, poSnakePos[pFoodInBody->i].y, hIconBodyLevel);
				if (pFoodInBody->CurrentDirection == up || pFoodInBody->CurrentDirection == down)
					DrawIcon(hdc, poSnakePos[pFoodInBody->i].x, poSnakePos[pFoodInBody->i].y, hIconBodyVertical);
				pFoodInBody = pFoodInBody->pNext;
			}

			//Draw the score
			UINT wShiWan = wCurrentScore / 1000;
			UINT wWan = wCurrentScore / 100 - wShiWan*10;
			UINT wQian = wCurrentScore / 10 - wShiWan*100 - wWan*10;
			UINT wBai = wCurrentScore - wShiWan*1000 - wWan*100 - wQian*10;
			switch (wBai)
			{
			case 0:
				DrawIconEx(hdc, 649, 59, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 1:
				DrawIconEx(hdc, 649, 59, hIconScore1, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 2:
				DrawIconEx(hdc, 649, 59, hIconScore2, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 3:
				DrawIconEx(hdc, 649, 59, hIconScore3, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 4:
				DrawIconEx(hdc, 649, 59, hIconScore4, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 5:
				DrawIconEx(hdc, 649, 59, hIconScore5, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 6:
				DrawIconEx(hdc, 649, 59, hIconScore6, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 7:
				DrawIconEx(hdc, 649, 59, hIconScore7, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 8:
				DrawIconEx(hdc, 649, 59, hIconScore8, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 9:
				DrawIconEx(hdc, 649, 59, hIconScore9, 46, 35, 0, NULL, DI_NORMAL);
				break;
			default:
				break;
			}
			switch (wQian)
			{
			case 0:
				DrawIconEx(hdc, 623, 59, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 1:
				DrawIconEx(hdc, 623, 59, hIconScore1, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 2:
				DrawIconEx(hdc, 623, 59, hIconScore2, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 3:
				DrawIconEx(hdc, 623, 59, hIconScore3, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 4:
				DrawIconEx(hdc, 623, 59, hIconScore4, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 5:
				DrawIconEx(hdc, 623, 59, hIconScore5, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 6:
				DrawIconEx(hdc, 623, 59, hIconScore6, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 7:
				DrawIconEx(hdc, 623, 59, hIconScore7, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 8:
				DrawIconEx(hdc, 623, 59, hIconScore8, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 9:
				DrawIconEx(hdc, 623, 59, hIconScore9, 46, 35, 0, NULL, DI_NORMAL);
				break;
			default:
				break;
			}
			switch (wWan)
			{
			case 0:
				DrawIconEx(hdc, 597, 59, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 1:
				DrawIconEx(hdc, 597, 59, hIconScore1, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 2:
				DrawIconEx(hdc, 597, 59, hIconScore2, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 3:
				DrawIconEx(hdc, 597, 59, hIconScore3, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 4:
				DrawIconEx(hdc, 597, 59, hIconScore4, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 5:
				DrawIconEx(hdc, 597, 59, hIconScore5, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 6:
				DrawIconEx(hdc, 597, 59, hIconScore6, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 7:
				DrawIconEx(hdc, 597, 59, hIconScore7, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 8:
				DrawIconEx(hdc, 597, 59, hIconScore8, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 9:
				DrawIconEx(hdc, 597, 59, hIconScore9, 46, 35, 0, NULL, DI_NORMAL);
				break;
			default:
				break;
			}
			switch (wShiWan)
			{
			case 0:
				DrawIconEx(hdc, 571, 59, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 1:
				DrawIconEx(hdc, 571, 59, hIconScore1, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 2:
				DrawIconEx(hdc, 571, 59, hIconScore2, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 3:
				DrawIconEx(hdc, 571, 59, hIconScore3, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 4:
				DrawIconEx(hdc, 571, 59, hIconScore4, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 5:
				DrawIconEx(hdc, 571, 59, hIconScore5, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 6:
				DrawIconEx(hdc, 571, 59, hIconScore6, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 7:
				DrawIconEx(hdc, 571, 59, hIconScore7, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 8:
				DrawIconEx(hdc, 571, 59, hIconScore8, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case 9:
				DrawIconEx(hdc, 571, 59, hIconScore9, 46, 35, 0, NULL, DI_NORMAL);
				break;
			default:
				break;
			}

			UINT wLevel;
			switch (wSpeed)
			{
			case 500:
			{
				wLevel = level1;
			}
			break;
			case 300:
			{
				wLevel = level2;
			}
			break;
			case 200:
			{
				wLevel = level3;
			}
			break;
			case 160:
			{
				wLevel = level4;
			}
			break;
			case 120:
			{
				wLevel = level5;
			}
			break;
			case 100:
			{
				wLevel = level6;
			}
			break;
			case 80:
			{
				wLevel = level7;
			}
			break;
			case 50:
			{
				wLevel = level8;
			}
			break;
			case 30:
			{
				wLevel = level9;
			}
			break;
			case 10:
			{
				wLevel = level10;
			}
			break;
			}

			//Draw the level
			switch (wLevel)
			{
			case level1:
				DrawIconEx(hdc, 690, 241, hIconScore1, 46, 35, 0, NULL, DI_NORMAL);
				DrawIconEx(hdc, 655, 241, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case level2:
				DrawIconEx(hdc, 690, 241, hIconScore2, 46, 35, 0, NULL, DI_NORMAL);
				DrawIconEx(hdc, 655, 241, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case level3:
				DrawIconEx(hdc, 690, 241, hIconScore3, 46, 35, 0, NULL, DI_NORMAL);
				DrawIconEx(hdc, 655, 241, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case level4:
				DrawIconEx(hdc, 690, 241, hIconScore4, 46, 35, 0, NULL, DI_NORMAL);
				DrawIconEx(hdc, 655, 241, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case level5:
				DrawIconEx(hdc, 690, 241, hIconScore5, 46, 35, 0, NULL, DI_NORMAL);
				DrawIconEx(hdc, 655, 241, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case level6:
				DrawIconEx(hdc, 690, 241, hIconScore6, 46, 35, 0, NULL, DI_NORMAL);
				DrawIconEx(hdc, 655, 241, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case level7:
				DrawIconEx(hdc, 690, 241, hIconScore7, 46, 35, 0, NULL, DI_NORMAL);
				DrawIconEx(hdc, 655, 241, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case level8:
				DrawIconEx(hdc, 690, 241, hIconScore8, 46, 35, 0, NULL, DI_NORMAL);
				DrawIconEx(hdc, 655, 241, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case level9:
				DrawIconEx(hdc, 690, 241, hIconScore9, 46, 35, 0, NULL, DI_NORMAL);
				DrawIconEx(hdc, 655, 241, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				break;
			case level10:
				DrawIconEx(hdc, 690, 241, hIconScore0, 46, 35, 0, NULL, DI_NORMAL);
				DrawIconEx(hdc, 655, 241, hIconScore1, 46, 35, 0, NULL, DI_NORMAL);
				break;
			}

			//Draw the gameover pic
			if (bGameOver == TRUE)
			{
				hdcmem = CreateCompatibleDC(hdc);
				SelectObject(hdcmem, hBmpGameOver);
				GetObject(hBmpGameOver, sizeof(bm), (LPSTR)&bm);
				BitBlt(hdc, GAMEOVERX, GAMEOVERY, bm.bmWidth, bm.bmHeight, hdcmem, 0, 0, SRCCOPY);
				DeleteDC(hdcmem);
			}

            EndPaint(hWnd, &ps);
        }
        break;
	case WM_CLOSE:
	    {
			KillTimer(hWnd, IDTIMER_SPEED);
			DestroyWindow(hWnd);
	    }
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
LRESULT CALLBACK Info(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK AddRecord(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	    {
			char szTemp[100] = "";
			GetWindowTextA(GetDlgItem(hDlg, IDC_NEWNAME), szTemp, sizeof(szTemp));
			if (strlen(szTemp) == 0)
				EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
	    }
		return TRUE;

	case WM_COMMAND:
	    {
			if (LOWORD(wParam) == IDOK)
			{
				GetWindowTextA(GetDlgItem(hDlg, IDC_NEWNAME), szTempName, sizeof(szTempName));
				EndDialog(hDlg, TRUE);
				return TRUE;
			}

			if (LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, FALSE);
				return TRUE;
			}

			if (LOWORD(wParam) == IDC_NEWNAME)
			{
				char szTemp[100] = "";
				GetWindowTextA(GetDlgItem(hDlg, IDC_NEWNAME), szTemp, sizeof(szTemp));
				if (strlen(szTemp) == 0)
					EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
				else
					EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
			}
	    }
		break;
	}
	return FALSE;
}

LRESULT CALLBACK Armory(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	    {
			char szTemp[100];
			//Get name of hero from ini;
			GetPrivateProfileStringA(szHeroName_T, szChampionName, "???", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_NAME1), szTemp);
			GetPrivateProfileStringA(szHeroName_T, szSecondName, "???", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_NAME2), szTemp);
			GetPrivateProfileStringA(szHeroName_T, szThirdName, "???", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_NAME3), szTemp);
			//Get score of hero from ini;
			GetPrivateProfileStringA(szList_HeroS, szChampionScore, "0", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_SCORE1), szTemp);
			GetPrivateProfileStringA(szList_HeroS, szSecondScore, "0", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_SCORE2), szTemp);
			GetPrivateProfileStringA(szList_HeroS, szThirdScore, "0", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_SCORE3), szTemp);
	    }
		return TRUE;

	case WM_COMMAND:
	    {
			if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, TRUE);
			return TRUE;
		}

			if (LOWORD(wParam) == IDC_RESET)
		{
			char szTemp[100];

			//write hero
			if (!WritePrivateProfileStringA(szHeroName_T, szChampionName, "???", szFileName))
				MessageBoxA(hDlg, "错误", "打不开配置文件", MB_OK);
			if (!WritePrivateProfileStringA(szHeroName_T, szSecondName, "???", szFileName))
				MessageBoxA(hDlg, "错误", "打不开配置文件", MB_OK);
			if (!WritePrivateProfileStringA(szHeroName_T, szThirdName, "???", szFileName))
				MessageBoxA(hDlg, "错误", "打不开配置文件", MB_OK);
			if (!WritePrivateProfileStringA(szList_HeroS, szChampionScore, "0", szFileName))
				MessageBoxA(hDlg, "错误", "打不开配置文件", MB_OK);
			if (!WritePrivateProfileStringA(szList_HeroS, szSecondScore, "0", szFileName))
				MessageBoxA(hDlg, "错误", "打不开配置文件", MB_OK);
			if (!WritePrivateProfileStringA(szList_HeroS, szThirdScore, "0", szFileName))
				MessageBoxA(hDlg, "错误", "打不开配置文件", MB_OK);

			//Get name of hero from ini;
			GetPrivateProfileStringA(szHeroName_T, szChampionName, "???", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_NAME1), szTemp);
			GetPrivateProfileStringA(szHeroName_T, szSecondName, "???", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_NAME2), szTemp);
			GetPrivateProfileStringA(szHeroName_T, szThirdName, "???", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_NAME3), szTemp);

			//Get score of hero from ini;
			GetPrivateProfileStringA(szList_HeroS, szChampionScore, "0", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_SCORE1), szTemp);
			GetPrivateProfileStringA(szList_HeroS, szSecondScore, "0", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_SCORE2), szTemp);
			GetPrivateProfileStringA(szList_HeroS, szThirdScore, "0", szTemp, sizeof(szTemp), szFileName);
			SetWindowTextA(GetDlgItem(hDlg, IDC_SCORE3), szTemp);

			EnableWindow(GetDlgItem(hDlg, IDC_RESET), FALSE);
		}
	    }
		break;
	}
	return FALSE;
}

LRESULT CALLBACK Difficulty(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	static UINT wLevel = 0;

	switch (message)
	{
	case WM_INITDIALOG:
	    {
			char szTemp[100];
			UINT wSpeed;
			//Get wSpeed from ini;
			GetPrivateProfileStringA(szSpeed_T, szCurrentSpeed, "0", szTemp, sizeof(szTemp), szFileName);
			wSpeed = atoi(szTemp);

			switch (wSpeed)
			{
			case 500:
			{
				wLevel = level1;
				CheckRadioButton(hDlg, IDC_LEVEL1, IDC_LEVEL10, IDC_LEVEL1);
			}
				break;
			case 300:
			{
				wLevel = level2;
				CheckRadioButton(hDlg, IDC_LEVEL1, IDC_LEVEL10, IDC_LEVEL2);
			}
				break;
			case 200:
			{
				wLevel = level3;
				CheckRadioButton(hDlg, IDC_LEVEL1, IDC_LEVEL10, IDC_LEVEL3);
			}
				break;
			case 160:
			{
				wLevel = level4;
				CheckRadioButton(hDlg, IDC_LEVEL1, IDC_LEVEL10, IDC_LEVEL4);
			}
				break;
			case 120:
			{
				wLevel = level5;
				CheckRadioButton(hDlg, IDC_LEVEL1, IDC_LEVEL10, IDC_LEVEL5);
			}
				break;
			case 100:
			{
				wLevel = level6;
				CheckRadioButton(hDlg, IDC_LEVEL1, IDC_LEVEL10, IDC_LEVEL6);
			}
				break;
			case 80:
			{
				wLevel = level7;
				CheckRadioButton(hDlg, IDC_LEVEL1, IDC_LEVEL10, IDC_LEVEL7);
			}
				break;
			case 50:
			{
				wLevel = level8;
				CheckRadioButton(hDlg, IDC_LEVEL1, IDC_LEVEL10, IDC_LEVEL8);
			}
				break;
			case 30:
			{
				wLevel = level9;
				CheckRadioButton(hDlg, IDC_LEVEL1, IDC_LEVEL10, IDC_LEVEL9);
			}
				break;
			case 10:
			{
				wLevel = level10;
				CheckRadioButton(hDlg, IDC_LEVEL1, IDC_LEVEL10, IDC_LEVEL10);
			}
				break;
			}
	    }
		return TRUE;
	case WM_COMMAND:
	    {
			int wmId = LOWORD(wParam);

			switch (wmId)
			{
			case IDOK:
			    {
					KillTimer(ghWnd, IDTIMER_SPEED);
					EndDialog(hDlg, TRUE);
					SendMessageA(ghWnd, WM_SENDLEVEL, (WPARAM)wLevel, 0);
			    }
				break;
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				break;
			case IDC_LEVEL1:
				wLevel = level1;
				break;
			case IDC_LEVEL2:
				wLevel = level2;
				break;
			case IDC_LEVEL3:
				wLevel = level3;
				break;
			case IDC_LEVEL4:
				wLevel = level4;
				break;
			case IDC_LEVEL5:
				wLevel = level5;
				break;
			case IDC_LEVEL6:
				wLevel = level6;
				break;
			case IDC_LEVEL7:
				wLevel = level7;
				break;
			case IDC_LEVEL8:
				wLevel = level8;
				break;
			case IDC_LEVEL9:
				wLevel = level9;
				break;
			case IDC_LEVEL10:
				wLevel = level10;
				break;
			}
	    }
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK Help(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
