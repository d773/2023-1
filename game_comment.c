#include <conio.h> // 키보드 입력과 화면 출력을 다루기 위한 함수들
#include <locale.h> // 로케일 설정과 관련된 함수들
#include <stdbool.h> // ool 자료형과 관련된 매크로를 정의
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>


#define SCREEN_WIDTH 80 // 화면의 너비
#define SCREEN_HEIGHT 25 // 화면의 높이

char* g_title = "PACMAN";



// 정수 값 x를 범위 low와 high 사이로 감싸는 역할을 함. 만약 x가 low보다 작으면 high 값으로 설정하고, x가 high보다 크면 low 값으로 설정함
// 즉, x, l, h가 있을 때 x의 범위는 반 열린 구간인 [l, h)가 됨. l, l+1, ..., h-2, h-1을 포함
// 예를 들어, wrap(8, 0, 5)의 결과는 3이 됨. 8이 범위 0과 5 사이에 있으므로 3으로 감싸지게 되는 것
int wrap(int x, int low, int high) {
    int n = (x - low) % (high - low); // x를 low와 high 사이의 범위로 조정
    return (n >= 0) ? (n + low) : (n + high); // n이 0보다 크거나 같으면 (n + low)을, n이 0보다 작으면 (n + high) 값을 반환
}



typedef enum eColor {
    COLOR_BLACK,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_JADE,
    COLOR_RED,
    COLOR_PURPLE,
    COLOR_YELLOW,
    COLOR_WHITE,
    COLOR_GRAY,
    COLOR_LIGHT_BLUE,
    COLOR_LIGHT_GREEN,
    COLOR_LIGHT_JADE,
    COLOR_LIGHT_RED,
    COLOR_LIGHT_PURPLE,
    COLOR_LIGHT_YELLOW,
    COLOR_LIGHT_WHITE
} Color;

// 커서를 x, y 좌표로 이동
void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void setcursor(bool visible) {
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = visible;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

// 텍스트의 색상을 설정: fg는 전경색, bg는 배경색
void textcolor(Color fg, Color bg) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg | bg << 4);
}



// # 타일 유형, 문자 및 렌더링에 사용되는 색상을 포함하여 게임 보드의 레이아웃과 속성을 정의

// 게임 보드의 레퍼런스, 배열의 각 요소는 보드의 타일에 해당하며 유형에 따라 값이 할당됨.
// 0은 비어 있음, 1은 벽, 2는 포인트, 3은 문, 4는 아이템
// 너비가 28이고 높이가 24인 보드에는 28 * 24 = 672개의 타일이 포함됨
int g_kBoardData[] = {
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,\
   1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1,\
   1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1,\
   1,4,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,4,1,\
   1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,\
   1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1,\
   1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1,\
   1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1,\
   0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0,\
   0,0,0,0,0,1,2,1,1,0,1,1,1,3,3,1,1,1,0,1,1,2,1,0,0,0,0,0,\
   1,1,1,1,1,1,2,1,1,0,1,0,0,0,0,0,0,1,0,1,1,2,1,1,1,1,1,1,\
   0,0,0,0,0,0,2,0,0,0,1,0,0,0,0,0,0,1,0,0,0,2,0,0,0,0,0,0,\
   1,1,1,1,1,1,2,1,1,0,1,0,0,0,0,0,0,1,0,1,1,2,1,1,1,1,1,1,\
   0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0,\
   0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0,\
   1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1,\
   1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1,\
   1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1,\
   1,4,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,4,1,\
   1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1,\
   1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1,\
   1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1,\
   1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,\
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,\
};
// 실제 게임에 사용되는 보드 데이터
int g_boardData[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,\
    1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1,\
    1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1,\
    1,4,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,4,1,\
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,\
    1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1,\
    1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1,\
    1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1,\
    0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0,\
    0,0,0,0,0,1,2,1,1,0,1,1,1,3,3,1,1,1,0,1,1,2,1,0,0,0,0,0,\
    1,1,1,1,1,1,2,1,1,0,1,0,0,0,0,0,0,1,0,1,1,2,1,1,1,1,1,1,\
    0,0,0,0,0,0,2,0,0,0,1,0,0,0,0,0,0,1,0,0,0,2,0,0,0,0,0,0,\
    1,1,1,1,1,1,2,1,1,0,1,0,0,0,0,0,0,1,0,1,1,2,1,1,1,1,1,1,\
    0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0,\
    0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0,\
    1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1,\
    1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1,\
    1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1,\
    1,4,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,4,1,\
    1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1,\
    1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1,\
    1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1,\
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,\
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,\
};
int g_kBoardWidth = 28; // 보드의 너비
int g_kBoardHeight = 24; // 보드의 높이
// 각 타일 유형을 표시하는 데 사용되는 문자를 나타내는 와이드 문자열,
wchar_t* g_tileCharData = L" #.-*"; // ' ': 비어 있음, '#': 벽, '.': 포인트, '-': 문, '*': 아이템
Color g_tileFgColorDate[] = { COLOR_BLACK, COLOR_BLUE, COLOR_YELLOW, COLOR_GRAY, COLOR_LIGHT_RED }; // 각 타일의 전경색
Color g_tileBgColorDate[] = { COLOR_BLACK, COLOR_BLUE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK }; // 각 타일의 배경색



// # 게임 보드를 관리 및 표시하고 시각적 효과를 제공

// 게임 보드를 초기화함. 상수 g_kBoardData의 초기 데이터를 실제 게임판 g_boardData로 복사
void reset_board() {
    int index = 0;
    for (int y = 0; y < g_kBoardHeight; ++y) {
        for (int x = 0; x < g_kBoardWidth; ++x) {
            int kCurIndexData = g_kBoardData[index];
            g_boardData[index++] = kCurIndexData;
        }
    }
}

// 게임 보드를 화면에 렌더링함. gotoxy() 함수를 사용하여 화면의 왼쪽 상단 모서리에 커서를 놓은 다음 g_boardData 배열의 각 요소를 반복, wprintf() 함수를 사용하여 해당 타일 문자를 출력, 타일의 전경색과 배경색은 각각 g_tileFgColorDate 및 g_tileBgColorDate 배열에 의해 결정
void draw_board() {
    int index = 0;
    gotoxy(0, 0);
    textcolor(COLOR_BLACK, COLOR_BLACK);
    for (int y = 0; y < g_kBoardHeight; ++y) {
        for (int x = 0; x < g_kBoardWidth; ++x) {
            int kCurIndexData = g_boardData[index++];
            textcolor(g_tileFgColorDate[kCurIndexData], g_tileBgColorDate[kCurIndexData]);
            wprintf(L"%lc", g_tileCharData[kCurIndexData]);
        }
        textcolor(COLOR_BLACK, COLOR_BLACK);
        wprintf(L"\n");
    }
}

// 지정된 색상으로 전체 게임 보드의 벽을 깜박임. 타일 ​​값이 1(벽)이면 해당 타일의 전경색과 배경색을 color 매개변수로 설정 후 wprintf() 함수를 사용하여 해당 타일 문자를 출력
void flash_board(Color color) {
    int index = 0;
    gotoxy(0, 0);
    textcolor(color, color);
    for (int y = 0; y < g_kBoardHeight; ++y) {
        for (int x = 0; x < g_kBoardWidth; ++x) {
            int kCurIndexData = g_boardData[index++];
            if (kCurIndexData == 1) {
                gotoxy(x, y);
                wprintf(L"%lc", g_tileCharData[kCurIndexData]);
            }
        }
    }
    textcolor(COLOR_BLACK, COLOR_BLACK);
}

// 게임 보드에 깜박이는 효과를 수행함. 지정된 반복 횟수(iter) 동안 다른 색상(흰색과 파란색)으로 flash_board()를 반복적으로 호출합니다. 'Sleep()' 함수를 사용하여 각 반복 사이에 지연(timePerIter)을 도입합니다.
void flash(int iter, int timePerIter) {
    for (int i = 0; i < iter; ++i) {
        flash_board(COLOR_WHITE);
        Sleep(timePerIter / 2);
        flash_board(COLOR_BLUE);
        Sleep(timePerIter / 2);
    }
}

// 게임 보드에 남은 점수를 계산하여 반환함. 타일 ​​값이 2(포인트) 또는 4(아이템)이면 count 변수를 증가시킵
int getRemainingPoint() {
    int index = 0;
    int count = 0;
    for (int y = 0; y < g_kBoardHeight; ++y) {
        for (int x = 0; x < g_kBoardWidth; ++x) {
            int kCurIndexData = g_boardData[index++];
            if (kCurIndexData == 2 || kCurIndexData == 4) {
                count++;
            }
        }
    }
    return count;
}



// # 오브젝트 공통 속성

typedef struct _Position {
    int x;
    int y;
} Position;

// Position 구조체에 대한 메모리를 동적으로 할당함. x, y값으로 초기화하여 새로 생성된 Position에 대한 포인터를 반환
Position* newPos(int x, int y) {
    Position* newPos = (Position*)malloc(sizeof(Position));
    newPos->x = x;
    newPos->y = y;
    return newPos;
}

// 이동할 수 있는 가능한 방향
typedef enum eDirection {
    DIR_NONE = -1,
    DIR_UP,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_DOWN
} Direction;

// Direction 값을 받아 반대 방향을 반환함. 'oppositeDirOf'라는 배열을 사용하여 각 가능한 방향의 반대 방향을 저장.
Direction getOppositeDirectionOf(Direction dir) {
    Direction oppositeDirOf[5] = { DIR_NONE, DIR_DOWN, DIR_RIGHT, DIR_LEFT, DIR_UP };
    return oppositeDirOf[dir + 1];
}

// 주어진 방향과 관련된 x 좌표의 변화를 반환함. kDx에 각 방향에 대한 x좌표의 변화량을 저장
int getDx(Direction dir) {
    int kDx[5] = { 0, 0, -1, +1, 0 };
    return kDx[dir + 1];
}

// 주어진 방향과 관련된 y 좌표의 변화를 반환함. kDy에 각 방향에 대한 y좌표의 변화량을 저장
int getDy(Direction dir) {
    int kDy[5] = { 0, -1, 0, 0, +1 };
    return kDy[dir + 1];
}



// # 플레이어의 속성과 동작을 정의
typedef struct _Player {
    clock_t posTimeStep; // 위치 업데이트를 위한 시간 간격
    clock_t frameTimeStep; // 프레임 업데이트를 위한 시간 간격
    int posDeltaTime; // 위치 업데이트 사이의 시간변화량
    int renderDeltaTime; // 프레임 업데이트 사이의 시간변화량
    Position pos; // 현재 위치
    Direction dir; // 향하고 있는 현재 방향
    Direction nextDir; // 이동하려는 다음 방향
    Color color;
    int curFrame; // 현재 프레임 인덱스
    int lifeRemaining; // 남은 목숨 수
    int speedPerSecond; // 초당 단위의 이동 속력
    bool isEaten; // 먹혔는가?
} Player;

Position Player_kStartingPoint = { 13, 18 }; // 시작점
Direction Player_kInitialDirection = DIR_LEFT; // 향하는 초기 방향
int Player_kFrameCount = 6; // 프레임 수
wchar_t* Player_kFrameIdle = L"O";
// 각 방향에 대한 프레임
wchar_t* Player_kFrameData[4] = {
   L"VV|||V", // UP
   L"})>->)", // LEFT
   L"{(<-<(", // RIGHT
   L"^^|||^", // DOWN
};

// 속성을 초기 값으로 재설정함
void Player_reset(Player* this) {
    this->pos = Player_kStartingPoint;
    this->dir = Player_kInitialDirection;
    this->nextDir = DIR_NONE;
    this->color = COLOR_LIGHT_YELLOW;
    this->speedPerSecond = 5;
    this->posDeltaTime = 0;
    this->posTimeStep = CLOCKS_PER_SEC / this->speedPerSecond;
    this->curFrame = 0;
    this->renderDeltaTime = 0;
    this->frameTimeStep = this->posTimeStep / Player_kFrameCount;
    this->isEaten = false;
}

// nextPos 위치로 이동할 수 있는지 확인함. g_boardData 배열에서 해당 인덱스의 값을 확인하여 값이 1(벽) 또는 3(문)이 아닌 경우 해당 위치는 이동 가능한 것으로 간주, nextPos 매개변수에 대해 동적으로 메모리를 할당하므로 결과를 반환하기 전에 해당 메모리를 해제
bool Player_isMovable(Position* nextPos) {
    bool isMovable = !((g_boardData[nextPos->y * 28 + nextPos->x] == 1) || (g_boardData[nextPos->y * 28 + nextPos->x] == 3));
    free(nextPos);
    return isMovable;
}

// 입력된 dir 방향을 기준으로 플레이어의 방향을 업데이트함.
void Player_look(Player* this, Direction dir) {
    // getDx(dir) 및 getDy(dir) 함수를 사용하여 현재 위치 좌표에 적절한 델타를 추가하여 다음 위치를 계산
    int kNextX = this->pos.x + getDx(dir);
    int kNextY = this->pos.y + getDy(dir);
    // 이동 가능한가?
    if (Player_isMovable(newPos(kNextX, kNextY))) {
        // 이동할 수 있으므로 현재 방향은 dir로 업데이트되고 다음 방향은 DIR_NONE으로 설정
        this->dir = dir;
        this->nextDir = DIR_NONE;
    } else {
        // 이동할 수 없으므로 다음(이동 가능한 위치일 때 설정할)방향을 dir로 업데이트
        this->nextDir = dir;
    }
}

// 게임 보드의 너비(g_kBoardWidth) 범위 내에서 좌표 x를 래핑
int Player_x_wrap(int x) {
    return wrap(x, 0, g_kBoardWidth);
}

// 게임 보드의 높이(g_kBoardHeight) 범위 내에서 좌표 y를 래핑
int Player_y_wrap(int y) {
    return wrap(y, 0, g_kBoardHeight);
}

// 플레이어의 프레임 수(Player_kFrameCount) 범위 내에서 주어진 프레임 인덱스 curFrame을 래핑
int Player_curFrame_wrap(int curFrame) {
    return wrap(curFrame, 0, Player_kFrameCount);
}



// # 유령의 속성과 동작을 정의

// 유령의 종류
typedef enum eGhostType {
    GHOST_BLINKY,
    GHOST_INKY,
    GHOST_PINKY,
    GHOST_CLYDE,
} GhostType;

// 유령의 행동과 프레임, 색상을 결정할 상태
typedef enum eGhostState {
    GHOST_STAYING,
    GHOST_SCATTERING,
    GHOST_CHASING,
    GHOST_FRIGHTENED,
    GHOST_EATEN
} GhostState;

// 유령의 상태별 지속 기간
enum eGhostDuration {
    STAYING_DURATION = 1 * CLOCKS_PER_SEC,
    SCATTERING_DURATION = 7 * CLOCKS_PER_SEC,
    CHASING_DURATION = 20 * CLOCKS_PER_SEC,
    FRIGHTENED_DURATION = 8 * CLOCKS_PER_SEC,
    EATEN_DURATION = 8 * CLOCKS_PER_SEC
};

typedef struct _Ghost {
    bool isState[5]; // 유령이 인덱스의 상태인가? 각 인덱스는 GhostState 열거형 값에 해당하며 true는 고스트가 해당 상태에 있음을 의미
    bool isInHome; // 유령이 현재 유령의 집 안에 있는가?
    int stateDuration[5]; // 각 유령 상태에 대한 남은 기간
    clock_t posTimeStep; // 위치 업데이트를 위한 시간 간격
    int posDeltaTime; // 위치 업데이트 사이의 시간변화량
    Position pos; // 현재 위치
    Position targetPos; // 움직이는 목표 위치
    Direction dir; // 현재 향하고 있는 방향
    Color curColor; // 현재 색상
    int curFrame; // 현재 프레임 인덱스
    int speedPerSecond; // 초당 이동속력
} Ghost;

wchar_t* Ghost_kFrameData = L"Mm%%"; // 'M': 일반, 'm': Frightened, '%': Eaten
Position Ghost_kInside = { 13, 11 }; // 집의 내부 영역 위치
Position Ghost_kOutside = { 13, 8 }; // 집 외부 영역의 시작 위치
Position Ghost_kStartingPoints[4] = { { 13, 8 }, {11 , 11}, {13 , 11}, {15 , 11} }; // 각 유령의 시작 위치
Direction Ghost_kInitialDirections[4] = { DIR_LEFT, DIR_UP, DIR_DOWN, DIR_UP }; // 각 유령의 초기 방향
int Ghost_kInitialStayingDurations[4] = { 0, 2, 8, 16 }; // 각 유령의 초단위의 초기 대기 시간
Color Ghost_kColors[4] = { COLOR_LIGHT_RED, COLOR_LIGHT_BLUE, COLOR_LIGHT_PURPLE, COLOR_YELLOW }; // 각 유령의 색상
Color Ghost_kVulnerableColor = COLOR_BLUE; // "공포-Frightened" 상태에 있을 때 유령을 나타내는 데 사용되는 색상
Color Ghost_kEatenColor = COLOR_WHITE; // 유령이 "먹힐 때"를 나타내는 데 사용되는 색상

// 유령의 상태를 초기화함
void Ghost_reset(Ghost* this, GhostType type) {
    this->pos = Ghost_kStartingPoints[type];
    this->targetPos = type == GHOST_BLINKY ? Ghost_kOutside : Ghost_kInside;
    this->dir = Ghost_kInitialDirections[type];
    this->curColor = Ghost_kColors[type];
    this->curFrame = 0;
    for (GhostState i = 0; i < 5;++i) {
        this->isState[i] = false;
        this->stateDuration[i] = 0;
    }
    this->isState[GHOST_STAYING] = true;
    this->stateDuration[GHOST_STAYING] = Ghost_kInitialStayingDurations[type] * STAYING_DURATION;
    this->isInHome = true;
    this->speedPerSecond = 4;
    this->posDeltaTime = 0;
    this->posTimeStep = CLOCKS_PER_SEC / this->speedPerSecond;
}

// 유령이 주어진 위치로 이동할 수 있는지 확인함. 유령의 상태(예를 들어, "먹힌" 상태가 아니거나 지속 시간이 4초 초과로 남아있는 "먹힌" 상태가 아니면 벽을 통과할 수 없음. , 다음 위치의 보드 데이터, 유령이 집에 있는지 여부를 확인하는 것입니다. 고스트가 다음 위치로 이동할 수 있는지 여부를 나타내는 부울 값을 반환합니다. 또한 nextPos 매개변수에 할당된 메모리를 해제합니다.
bool Ghost_isMovable(Ghost* this, Position* nextPos) {
    bool isMovable = !(
        ((!this->isState[GHOST_EATEN] // "먹힌" 상태가 아니거나,
            || this->isState[GHOST_EATEN] && this->stateDuration[GHOST_EATEN] > 4000) // 지속 시간이 4초 초과로 남아있는 "먹힌" 상태가 아닌데
            && (g_boardData[nextPos->y * 28 + nextPos->x] == 1)) // 벽이라면 통과할 수 없음.
        || // 또는
        ((!(this->isState[GHOST_STAYING] && !this->isInHome) //  (유령이 "대기" 상태이고 집에 없는 것이) 아니거나,
            || !(!this->isState[GHOST_EATEN] && !this->isInHome) // (유령이 "먹힌" 상태가 아니고 집에 없는 것이) 아니거나,
            || this->isInHome) // 집에 있는 것인데
            && g_boardData[nextPos->y * 28 + nextPos->x] == 3) // 문이라면 통과할 수 없음.
        );
    free(nextPos);
    return isMovable;
}

// 유령의 방향을 업데이트함
void Ghost_look(Ghost* this, Direction dir) {
    this->dir = dir;
}

void Ghost_update(Ghost* this, GhostType type, Player* player, int deltaTime) {
    int kPx = this->pos.x;
    int kPy = this->pos.y;
    Direction kDir = this->dir;
    Direction kBackwardsDir = getOppositeDirectionOf(kDir);

    // 각 활성 상태('isState'에 저장됨)의 'stateDuration'은 'deltaTime' 값만큼 줄임. 이 값은 마지막 업데이트 이후 경과된 시간을 나타냄.
    for (GhostState index = 0; index < 5; ++index) {
        if (this->isState[index]) {
            this->stateDuration[index] -= deltaTime;
        }
    }

    // 유령이 "먹힌" 상태
    if (this->isState[GHOST_EATEN]) {
        int kTx = this->targetPos.x;
        int kTy = this->targetPos.y;
        // 유령이 목표 위치에 도달했는지(kPx == kTx && kPy == kTy), 먹힌 상태의 남은 지속 시간이 만료되었다면
        if (kPx == kTx && kPy == kTy || this->stateDuration[GHOST_EATEN] <= 0) {
            // 유령을 재설정
            this->pos = Ghost_kStartingPoints[type == GHOST_BLINKY ? GHOST_PINKY : type];
            this->isState[GHOST_EATEN] = false;
            this->stateDuration[GHOST_EATEN] = 0;
            this->isState[GHOST_STAYING] = true;
            this->stateDuration[GHOST_STAYING] = STAYING_DURATION * 8;
            this->isInHome = true;
            this->targetPos = Ghost_kInside;
            this->curColor = Ghost_kColors[type];
            this->curFrame = 0;
            this->posDeltaTime /= 4;
            this->speedPerSecond = 4;
            this->posTimeStep = CLOCKS_PER_SEC / this->speedPerSecond;

            int* tileAtGhostPos = &g_boardData[kPy * 28 + kPx];
            gotoxy(kPx, kPy);
            textcolor(g_tileFgColorDate[*tileAtGhostPos], g_tileBgColorDate[*tileAtGhostPos]);
            wprintf(L"%lc", g_tileCharData[*tileAtGhostPos]);
            return;
        }
    }

    // 유령이 "두려움" 상태
    if (this->isState[GHOST_FRIGHTENED]) {
        // 두려움 상태의 남은 지속 시간이 만료되었다면
        if (this->stateDuration[GHOST_FRIGHTENED] <= 0) {
            // "흩어짐" 상태로 전환
            this->isState[GHOST_FRIGHTENED] = false;
            this->stateDuration[GHOST_FRIGHTENED] = 0;
            this->isState[GHOST_SCATTERING] = true;
            this->stateDuration[GHOST_SCATTERING] = SCATTERING_DURATION;
            this->curColor = Ghost_kColors[type];
            this->curFrame = 0;
            this->posDeltaTime /= 2;
            this->speedPerSecond = 4;
            this->posTimeStep = CLOCKS_PER_SEC / this->speedPerSecond;
            return;
        }
    }

    // 유령이 "대기" 상태
    if (this->isState[GHOST_STAYING]) {
        // 아직 대기 중이라면
        if (this->stateDuration[GHOST_STAYING] > 0) {
            //현재 위치를 기준으로 다음 위치를 결정
            int kNx = kPx + getDx(kDir);
            int kNy = kPy + getDy(kDir);
            if (!Ghost_isMovable(this, newPos(kNx, kNy))) {
                Ghost_look(this, kBackwardsDir);
            }
            return;
        }
        // 대기 중이 아닌데 집 안이라면
        if (this->isInHome) {
            // y축이 아직 목표 위치에 도달하지 않은 경우 다음 위치를 결정하고 이동 가능한지 확인
            if (kPy != this->targetPos.y) {
                int kNx = kPx + getDx(kDir);
                int kNy = kPy + getDy(kDir);
                if (!Ghost_isMovable(this, newPos(kNx, kNy))) {
                    Ghost_look(this, kBackwardsDir);
                }
                return;
            }
            // y축이 목표 위치와 같다면 x축의 방향을 목표 위치 쪽으로 설정
            if (kPx != this->targetPos.x) {
                Ghost_look(this, this->targetPos.x - kPx > 0 ? DIR_RIGHT : DIR_LEFT);
                return;
            }
            // 목표 위치에 도달하였다면 다음 목표 위치를 집 외부 시작점으로 설정 후 위로 이동
            this->isInHome = false;
            this->targetPos = Ghost_kOutside;
            Ghost_look(this, DIR_UP);
            return;
        }
        // 아직 외부 시작점에 도달하지 않았다면 변화 없음(위로 이동)
        if (kPy != this->targetPos.y) {
            return;
        }
        // 외부 시작점에 도달하였다면 "흩어짐" 상태로 전환
        this->isState[GHOST_STAYING] = false;
        this->stateDuration[GHOST_STAYING] = 0;
        this->isState[GHOST_SCATTERING] = true;
        this->stateDuration[GHOST_SCATTERING] = SCATTERING_DURATION;
        Ghost_look(this, rand() & 1 ? DIR_LEFT : DIR_RIGHT);
        return;
    }

    // 유령이 "두려움" 상태가 아니면서 "흩어짐" 상태라면
    if (!this->isState[GHOST_FRIGHTENED] && this->isState[GHOST_SCATTERING]) {
        // 지속 시간이 만료되었다면
        if (this->stateDuration[GHOST_SCATTERING] <= 0) {
            // "추적" 상태로 전환
            this->isState[GHOST_SCATTERING] = false;
            this->stateDuration[GHOST_SCATTERING] = 0;
            this->isState[GHOST_CHASING] = true;
            this->stateDuration[GHOST_CHASING] = CHASING_DURATION;
            return;
        }

        // 지속 시간이 만료되지 않았면 현재 위치에서 이동 가능한 방향을 탐색
        Direction movableDirs[3];
        int movableDirCount = 0;

        for (Direction dir = 0; dir < 4; ++dir) {
            if (dir == kBackwardsDir) {
                continue;
            }
            int kNx = kPx + getDx(dir);
            int kNy = kPy + getDy(dir);
            if (Ghost_isMovable(this, newPos(kNx, kNy))) {
                movableDirs[movableDirCount++] = dir;
            }
        }
        // 이동 가능한 방향들 중에서 랜덤으로 현재 방향을 설정
        int kRandDirIndex = movableDirCount > 1 ? rand() % movableDirCount : 0;
        Ghost_look(this, movableDirs[kRandDirIndex]);
        return;
    }

    // 유령이 "두려움" 상태가 아니면서 "추적" 상태라면
    if (!this->isState[GHOST_FRIGHTENED] && this->isState[GHOST_CHASING]) {
        // 지속 시간이 만료되었다면
        if (this->stateDuration[GHOST_CHASING] <= 0) {
            // "흩어짐" 상태로 전환
            this->isState[GHOST_CHASING] = false;
            this->stateDuration[GHOST_CHASING] = 0;
            this->isState[GHOST_SCATTERING] = true;
            this->stateDuration[GHOST_SCATTERING] = SCATTERING_DURATION;
            return;
        }
        // 지속 시간이 만료되지 않았다면 목표 위치를 플레이어 위치로 설정
        this->targetPos = player->pos;
    }

    // 현재 타겟 위치를 설정
    int kTx = this->targetPos.x;
    int kTy = this->targetPos.y;

    int bestDir = DIR_NONE;
    int distBestDir = this->isState[GHOST_FRIGHTENED] ? -1 : 9876543; // 최선 방향의 거리는 "두려움" 상태일 때 플레이어로 부터 최대 거리, "추적" 상태일 때 최단 거리

    for (Direction dir = 0; dir < 4; ++dir) {
        // 탐색 방향이 뒷 방향이면 다음 탐색 방향으로 탐색
        if (dir == kBackwardsDir) {
            continue;
        }

        // 탐색 방향의 위치 변화를 저장
        int kNx = kPx + getDx(dir);
        int kNy = kPy + getDy(dir);

        // 탐색 방향으로 이동 불가하다면 다음 탐색 방향으로 탐색
        if (!Ghost_isMovable(this, newPos(kNx, kNy))) {
            continue;
        }

        // 탐색 방향의 예상 위치와 플레이어와 위치 사이의 거리를 계산
        int kDist = (kTx - kNx) * (kTx - kNx) + (kTy - kNy) * (kTy - kNy); // L2 Norm
        // int kDist = abs(kNx - kTx) + abs(kNy - kTy); // Manhattan dist L1 Norm

        if (this->isState[GHOST_FRIGHTENED]) {
            // "두려움" 상태일 때 최대 거리를 선택
            if (kDist > distBestDir || bestDir == DIR_NONE) {
                bestDir = dir;
                distBestDir = kDist;
            }
        } else {
            // "추적" 상태일 때 최단 거리를 선택
            if (kDist < distBestDir || bestDir == DIR_NONE) {
                bestDir = dir;
                distBestDir = kDist;
            }
        }
    }

    // 선택된 최선 방향으로 현재 방향을 설정
    Ghost_look(this, bestDir);
}

// 플레이어가 파워 아이템을 먹었을 때
void Player_eatPowerItem(Ghost ghosts[4]) {
    for (int i = 0; i < 4; i++) {
        // 유령이 이미 "먹힌" 상태하면 다음 유령으로 진행
        if (ghosts[i].isState[GHOST_EATEN]) {
            continue;
        }
        // 유령의 상태를 "두려움" 상태로 전환
        ghosts[i].isState[GHOST_FRIGHTENED] = true;
        ghosts[i].stateDuration[GHOST_FRIGHTENED] = FRIGHTENED_DURATION;
        ghosts[i].curColor = Ghost_kVulnerableColor;
        ghosts[i].curFrame = 1;
        ghosts[i].speedPerSecond = 2;
        ghosts[i].posDeltaTime *= 2;
        ghosts[i].posTimeStep = CLOCKS_PER_SEC / ghosts[i].speedPerSecond;
    }
}

// 플레이어가 유령을 먹었을 때
void Player_eatGhost(Ghost* ghost, GhostType type) {
    for (int i = 0; i < 4; ++i) {
        // 유령의 모든 상태와 지속 시간을 재설정
        ghost->isState[i] = false;
        ghost->stateDuration[i] = 0;
    }
    // 유령을 먹힌 "상태"로 전환
    ghost->isState[GHOST_EATEN] = true;
    ghost->stateDuration[GHOST_EATEN] = EATEN_DURATION;
    ghost->curColor = Ghost_kEatenColor;
    ghost->curFrame = 2;
    ghost->speedPerSecond = 10;
    ghost->posDeltaTime /= 4;
    ghost->posTimeStep = CLOCKS_PER_SEC / ghost->speedPerSecond;
    ghost->targetPos = Ghost_kOutside; // 유령의 목표 위치를 집 외부시작점으로 설정
}

// 게임의 유령과 플레이어 상태를 초기화
void resetGameState(Ghost ghosts[4], Player* player) {
    Player_reset(player);
    for (GhostType i = 0; i < 4; ++i) {
        Ghost_reset(&ghosts[i], i);
    }
    draw_board();
}

// 유령이 플레이어를 먹었을 때
void Ghost_eatPacman(Ghost ghosts[4], Player* player) {
    // 게임 상태를 재설정
    player->isEaten = true;
    resetGameState(ghosts, player);
}




enum eKey {
    KEY_UP = 0x48,
    KEY_LEFT = 0x4B,
    KEY_RIGHT = 0x4D,
    KEY_DOWN = 0x50,
    KEY_ESC = 0x1B,
    KEY_ENTER = 0x0D
};

int getInputKey() {
    if (kbhit()) {
        int input = getch();
        bool isFunctionKey = input == 0x00;
        bool isExtendedKey = input == 0xE0;
        if (isFunctionKey || isExtendedKey) {
            return getch();
        } else {
            return input;
        }
    }
    return 0;
}



/*
enum eCommand {
    CMD_SET_DIR_UP,
    CMD_SET_DIR_LEFT,
    CMD_SET_DIR_RIGHT,
    CMD_SET_DIR_DOWN,
    CMD_QUIT_GAME
};

int g_command[] = { DIR_UP, DIR_LEFT, DIR_RIGHT, DIR_DOWN, CMD_QUIT_GAME };
int g_gameKey[128];

void GameKey_Initialize() {
    g_gameKey['w'] = CMD_SET_DIR_UP;
    g_gameKey[KEY_UP] = CMD_SET_DIR_UP;
    g_gameKey['a'] = CMD_SET_DIR_LEFT;
    g_gameKey[KEY_LEFT] = CMD_SET_DIR_LEFT;
    g_gameKey['d'] = CMD_SET_DIR_RIGHT;
    g_gameKey[KEY_RIGHT] = CMD_SET_DIR_RIGHT;
    g_gameKey['s'] = CMD_SET_DIR_DOWN;
    g_gameKey[KEY_DOWN] = CMD_SET_DIR_DOWN;
    g_gameKey['q'] = CMD_QUIT_GAME;
    g_gameKey[KEY_ESC] = CMD_QUIT_GAME;
}
*/



void clsFront() {
    char cmd[100];
    system("cls"); // NOLINT
    sprintf(cmd, "mode con: cols=%d lines=%d", SCREEN_WIDTH, SCREEN_HEIGHT);
    system(cmd); // NOLINT
    sprintf(cmd, "title %s", g_title);
    system(cmd); // NOLINT
    _wsetlocale(LC_ALL, L"");
    setcursor(false);
}

void clsEnd() {
    system("cls"); // NOLINT
    setcursor(true);
}

int main(void) {
    // Initialize Game
    char kPressedCmpData[] = { 'w', 'a', 'd', 's', KEY_UP, KEY_LEFT, KEY_RIGHT, KEY_DOWN, 'q', KEY_ESC }; // 눌린 키 비교 데이터
    bool isPressed[10] = { false, }; // 눌린 키를 저장하는 배열
    int highScore = 0;

    // Initialize Player
    Player player = { 0, };
    Player_reset(&player);

    // Initialize Ghost
    Ghost ghosts[4] = { 0, };
    for (GhostType i = 0; i < 4;++i) {
        Ghost_reset(&ghosts[i], i);
    }

    // Menu Scene: 게임 시작 또는 종료를 선택할 수 있는 메뉴 화면. 메뉴 옵션이 표시되고 플레이어의 입력을 처리하여 옵션을 선택.
    clsFront();
    int selection = -1;

    do {
        system("cls");
        int curMenuPointer = 0;
        int input = 0;
        selection = -1;

        gotoxy(0, 0);
        textcolor(COLOR_LIGHT_WHITE, COLOR_BLACK);
        wprintf(L"> Start\n");
        wprintf(L"  Quit\n");

        while (selection == -1) {
            int kPrevPointer = curMenuPointer;

            input = getInputKey();

            switch (input) {
            case KEY_UP:
                curMenuPointer = wrap(curMenuPointer - 1, 0, 2);
                break;
            case KEY_DOWN:
                curMenuPointer = wrap(curMenuPointer + 1, 0, 2);
                break;
            case KEY_ESC:
                selection = 1;
                break;
            case KEY_ENTER:
                selection = curMenuPointer;
                break;
            default:
                break;
            }

            if (curMenuPointer != kPrevPointer) {
                gotoxy(0, kPrevPointer);
                wprintf(L" ");
                gotoxy(0, curMenuPointer);
                wprintf(L">");
            }
        }
        gotoxy(0, selection);
        wprintf(L" \n");

        if (selection == 1) {
            break;
        }



        // Game Scene: 게임 시작을 선택하면 게임 화면에 진입.
        // 게임의 변수를 초기화하고 게임판, 최고 점수, 현재 점수, 플레이어의 남은 목숨을 표시함.
        bool isPlaying = true;
        bool started = false;
        int remainingPoint = getRemainingPoint();
        int curScore = 0;
        Player_reset(&player);
        player.lifeRemaining = 3;
        for (GhostType i = 0; i < 4;++i) {
            Ghost_reset(&ghosts[i], i);
        }

        draw_board();

        // Draw Panel
        gotoxy(31, 0);
        textcolor(COLOR_RED, COLOR_BLACK);
        wprintf(L"HI-SCORE");
        gotoxy(31, 1);
        textcolor(COLOR_LIGHT_WHITE, COLOR_BLACK);
        wprintf(L"%8d", highScore);
        gotoxy(31, 2);
        wprintf(L"%8d", curScore);

        gotoxy(31, 4);
        textcolor(COLOR_LIGHT_YELLOW, COLOR_BLACK);
        wprintf(L"Life");
        textcolor(COLOR_LIGHT_WHITE, COLOR_BLACK);
        wprintf(L"x%d", player.lifeRemaining);

        gotoxy(31, 6);
        wprintf(L"  W : UP");
        gotoxy(31, 7);
        wprintf(L"  A : LEFT");
        gotoxy(31, 8);
        wprintf(L"  S : RIGHT");
        gotoxy(31, 9);
        wprintf(L"  D : DOWN");
        gotoxy(31, 10);
        wprintf(L"ESC : Quit");

        // 게임 루프에서 일관된 프레임 속도(kTargetFPS)를 유지하기 위한 고정 업데이트 메커니즘을 포함함. 고정된 시간 단계(kFixedTimeStep)를 기반으로 게임 로직과 엔티티를 업데이트하여 일관된 동작을 보장함.
        clock_t kTargetFPS = 60;
        clock_t kTargetFrameTime = CLOCKS_PER_SEC / kTargetFPS; // 프레임 사이 간격

        // 고정 업데이트 간격: 초당 50번 업데이트.유니티에서 물리 연산 처리시 주로 사용하는 FixedUpdate와 같은 원리.
        clock_t kFixedTimeStep = 0.02 * CLOCKS_PER_SEC; // 1 / targetFPS

        // FixedUpdate반복 횟수를 결정할 게임 루프 1회 반복 중 시간 변화량을 저장. 유니티에서의 Time.deltaTime과 같은 원리
        int fixedTimeLag = 0;
        clock_t previousTime = clock(); // 매번 갱신되는 가장 최근에 마지막으로 업데이트한 시각을 저장, 이하 '이전최근측정시간'

        while (isPlaying) {
            // # 일반적인 업데이트 부분. 유니티의 Update에 해당.
            // Process Input
            input = getInputKey();

            for (int key = 0; key < 10; ++key) {
                isPressed[key] = input == kPressedCmpData[key];
            }

            // Handle Input Key
            if (isPressed[8] || isPressed[9]) {
                isPlaying = false;
                continue;
            }

            // Handle Input Player Movement
            for (Direction dir = 0; dir < 4; ++dir) {
                if (!(isPressed[dir] || isPressed[dir + 4])) {
                    continue;
                }
                Player_look(&player, dir);
                break;
            }

            // Update Game
            int kScore = curScore; // 게임 업데이트 직전의 점수 저장
            int kLifeRemaining = player.lifeRemaining; // 게임 업데이트 직전의 목숨 수 저장

            if (remainingPoint <= 0) { // 모든 포인트를 먹었다면 게임 승리
                flash(3, 500);
                gotoxy(10, 16);
                textcolor(COLOR_LIGHT_WHITE, COLOR_YELLOW);
                wprintf(L" Clear! ");
                Sleep(3000);
                previousTime = clock();
                isPlaying = false;
            }

            // Update Player
            if (player.nextDir != DIR_NONE) { // 플레이어가 다음 방향으로 전환할 수 있는지 확인
                Player_look(&player, player.nextDir);
            }

            // Fixed Update
            // # 고정 시간 업데이트 부분. 유니티의 Fixed Update에 해당.
            clock_t currentTime = clock(); // 현재 시간 측정
            clock_t kElapsedTime = currentTime - previousTime; // '현재시간'과 '이전최근측정시간'의 차로 '지연된 시간'을 측정
            fixedTimeLag += kElapsedTime; // 지연된 시간을 고정 업데이트 시간 변화량에 추가

            player.posDeltaTime += fixedTimeLag; // 시간 변화량을 플레이어의 위치 업데이트 시간 변화량에 추가
            player.renderDeltaTime += fixedTimeLag; // 시간 변화량을 플레이어의 렌더링 업데이트 시간 변화량에 추가

            for (GhostType i = 0; i < 4; ++i) {
                ghosts[i].posDeltaTime += fixedTimeLag; // 시간 변화량을 유령의 위치 업데이트 시간 변화량에 추가
                Ghost_update(&ghosts[i], i, &player, fixedTimeLag);
            }

            // 시간 변화량이 고정 업데이트 간격(kFixedTimeStep) 보다 크다면 ex(0.07 >= 0.02)
            while (fixedTimeLag >= kFixedTimeStep) {
                // Player
                // 플레이어의 위치 업데이트 시간 변화량이 위치 업데이트 간격보다 크다면
                if (player.posDeltaTime >= player.posTimeStep) {
                    int kPrevX = player.pos.x;
                    int kPrevY = player.pos.y;
                    int kNextX = Player_x_wrap(kPrevX + getDx(player.dir));
                    int kNextY = Player_y_wrap(kPrevY + getDy(player.dir));
                    int* tileAtPlayerPos = &g_boardData[kPrevY * 28 + kPrevX];

                    // Add Score
                    if (*tileAtPlayerPos == 2) {
                        remainingPoint -= 1;
                        curScore += 10;
                        *tileAtPlayerPos = 0;
                    }

                    if (*tileAtPlayerPos == 4) {
                        remainingPoint -= 1;
                        curScore += 50;
                        *tileAtPlayerPos = 0;
                        Player_eatPowerItem(ghosts);
                    }

                    // Movement
                    // 플레이어가 이동 가능하다면 이동시킴
                    if (Player_isMovable(newPos(kNextX, kNextY))) {
                        player.pos.x = kNextX;
                        player.pos.y = kNextY;
                        gotoxy(kPrevX, kPrevY);
                        textcolor(g_tileFgColorDate[*tileAtPlayerPos], g_tileBgColorDate[*tileAtPlayerPos]);
                        wprintf(L"%lc", g_tileCharData[*tileAtPlayerPos]);
                    }

                    // 유령과의 접촉 여부를 확인
                    for (GhostType i = 0; i < 4; ++i) {
                        if (!ghosts[i].isState[GHOST_EATEN]) {
                            // 접촉했는데
                            if (player.pos.x == ghosts[i].pos.x && player.pos.y == ghosts[i].pos.y) {
                                // 유령이 "두려움" 상태라면
                                if (ghosts[i].isState[GHOST_FRIGHTENED]) {
                                    // 플레이어가 유령을 먹음
                                    curScore += 200;
                                    flash(1, 500);
                                    Player_eatGhost(&ghosts[i], i);
                                    currentTime = clock();
                                } else {
                                    // 아니라면 플레이어가 유령에게 먹힘
                                    if (!player.isEaten) {
                                        player.lifeRemaining--;
                                        flash(3, 500);
                                        // 플레이어의 목숨이 1개 이상이면
                                        if (player.lifeRemaining > 0) {
                                            // 유령에게 먹힘
                                            Ghost_eatPacman(ghosts, &player);
                                            currentTime = clock();
                                        } else {
                                            // 아니라면 게임에서 패배
                                            gotoxy(6, 16);
                                            textcolor(COLOR_LIGHT_WHITE, COLOR_PURPLE);
                                            wprintf(L"lose the game...");
                                            Sleep(3000);
                                            currentTime = clock();
                                            isPlaying = false;
                                        }
                                    }
                                }
                            }
                        }

                    }
                    // 플레이어 업데이트 간격 만큼 변화량 감소
                    player.posDeltaTime -= player.posTimeStep;
                }

                // Ghost
                for (GhostType i = 0; i < 4; ++i) {
                    Ghost* ghost = &ghosts[i];
                    // 유령의 위치 업데이트 시간 변화량이 위치 업데이트 간격보다 크다면
                    if (ghost->posDeltaTime >= ghost->posTimeStep) {
                        int kPrevX = ghost->pos.x;
                        int kPrevY = ghost->pos.y;
                        int kNextX = kPrevX + getDx(ghost->dir);
                        int kNextY = kPrevY + getDy(ghost->dir);
                        int* tileAtGhostPos = &g_boardData[kPrevY * 28 + kPrevX];
                        // 유령이 이동 가능하다면 이동시킴
                        if (Ghost_isMovable(ghost, newPos(kNextX, kNextY))) {
                            ghost->pos.x = kNextX;
                            ghost->pos.y = kNextY;
                            gotoxy(kPrevX, kPrevY);
                            textcolor(g_tileFgColorDate[*tileAtGhostPos], g_tileBgColorDate[*tileAtGhostPos]);
                            wprintf(L"%lc", g_tileCharData[*tileAtGhostPos]);
                        }
                        // 유령이 "먹힌" 상태가 아닌데
                        if (!ghost->isState[GHOST_EATEN]) {
                            // 플레이어와 접촉했고
                            if (ghost->pos.x == player.pos.x && ghost->pos.y == player.pos.y) {
                                // 유령이 "두려움" 상태라면
                                if (ghost->isState[GHOST_FRIGHTENED]) {
                                    // 유령이 플레이어에게 먹힘
                                    curScore += 200;
                                    flash(1, 500);
                                    Player_eatGhost(ghost, i);
                                    currentTime = clock();
                                } else {
                                    // 아니라면 유령이 플레이어를 먹음
                                    if (!player.isEaten) {
                                        player.lifeRemaining--;
                                        flash(3, 500);
                                        // 플레이어의 목숨이 1개 이상이면
                                        if (player.lifeRemaining > 0) {
                                            // 유령이 먹음
                                            Ghost_eatPacman(ghosts, &player);
                                            currentTime = clock();
                                        } else {
                                            // 아니라면 게임에서 패배
                                            gotoxy(5, 16);
                                            textcolor(COLOR_LIGHT_WHITE, COLOR_PURPLE);
                                            wprintf(L" lose the game... ");
                                            Sleep(3000);
                                            currentTime = clock();
                                            isPlaying = false;
                                        }
                                    }
                                }
                            }
                        }
                        // 유령 업데이트 간격 만큼 변화량 감소
                        ghost->posDeltaTime -= ghost->posTimeStep;
                    }
                }

                // 고정 업데이트 간격 만큼 변화량 감소
                fixedTimeLag -= kFixedTimeStep;
            }

            // Draw Player Frame
            // 플레이어의 렌더링 업데이트 시간 변화량이 렌더링 업데이트 간격보다 크다면
            while (player.renderDeltaTime >= player.frameTimeStep) {
                player.curFrame = Player_curFrame_wrap(player.curFrame + 1); // 플레이어의 프레임을 변경
                player.renderDeltaTime -= player.frameTimeStep; // 렌더링 업데이트 간격 만큼 변화량 감소
            }
            // 플레이어를 렌더링
            gotoxy(player.pos.x, player.pos.y);
            textcolor(player.color, COLOR_BLACK);
            wprintf(L"%lc", Player_kFrameData[player.dir][player.curFrame]);

            // Draw Ghost
            // 각 유령을 렌더링
            for (GhostType i = 0; i < 4; ++i) {
                Ghost* ghost = &ghosts[i];
                gotoxy(ghost->pos.x, ghost->pos.y);
                textcolor(ghost->curColor, COLOR_BLACK);
                wprintf(L"%lc", Ghost_kFrameData[ghost->curFrame]);
            }

            // Draw Score
            // 업데이트 전 점수와 현재 점수가 다르다면
            if (curScore != kScore) {
                // 점수 패널 갱신
                gotoxy(31, 2);
                textcolor(COLOR_LIGHT_WHITE, COLOR_BLACK);
                wprintf(L"%8d", curScore);
            }

            // Draw lifeRemaining
            // 업데이트 전 목숨수와 현재 목숨수가 다르다면
            if (player.lifeRemaining != kLifeRemaining) {
                // 목숨 패널 갱신
                gotoxy(31, 4);
                textcolor(COLOR_LIGHT_YELLOW, COLOR_BLACK);
                wprintf(L"Life");
                textcolor(COLOR_LIGHT_WHITE, COLOR_BLACK);
                wprintf(L"x%d", player.lifeRemaining);
            }

            // Delay: 지연된 시간과 목표 프레임 긴격의 차이로 다음 프레임까지 남은 시간을 계산
            clock_t kRemainingTime = kTargetFrameTime - kElapsedTime;
            // 다음 프레임까지 시간이 남았다면
            if (kRemainingTime > 0) {
                // 다음 프레임까지 대기
                Sleep(kRemainingTime);
            }
            // 이전최근측정시간을 갱신
            previousTime = currentTime;
        }

        // 게임화면이 종료되었다면 최대 점수를 갱신, 게임 보드를 초기화함
        highScore = curScore > highScore ? curScore : highScore;
        reset_board();
    } while (selection != 1);

    // 게임이 졸료되었다면 화면 되돌리기
    clsEnd();

    return 0;
}
