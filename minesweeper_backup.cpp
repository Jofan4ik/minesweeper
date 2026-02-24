#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <mmsystem.h>
#include <shellapi.h>
#include <cstring>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shell32.lib")

const int MAX_GRID_SIZE = 32;

int gridSize = 9;
int cellSize = 40;
int mineCount = 10;

bool mines[MAX_GRID_SIZE][MAX_GRID_SIZE];
bool special99[MAX_GRID_SIZE][MAX_GRID_SIZE];
bool open[MAX_GRID_SIZE][MAX_GRID_SIZE];
bool flag[MAX_GRID_SIZE][MAX_GRID_SIZE];
bool gameOver = false;
bool gameWon = false;
bool firstCellClicked = false;
bool autoMode = false;
int flagCount = 0;
char cheatCode[10] = {0};

int countMines(int y, int x) {
    int c = 0;
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            int ny = y + dy, nx = x + dx;
            if (ny >= 0 && ny < gridSize && nx >= 0 && nx < gridSize)
                if (mines[ny][nx]) c++;
        }
    return c;
}

bool hasAllMinesAround(int y, int x) {
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue;
            int ny = y + dy, nx = x + dx;
            if (ny >= 0 && ny < gridSize && nx >= 0 && nx < gridSize) {
                if (!mines[ny][nx]) return false;
            }
        }
    }
    return true;
}

void revealAllMines() {
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            if (mines[y][x]) {
                open[y][x] = true;
            }
        }
    }
}

void autoSetFlags() {
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            if (!open[y][x] || mines[y][x] || special99[y][x]) continue;
            
            int mineCount = countMines(y, x);
            if (mineCount == 0) continue;
            
            int closedCount = 0;
            int flaggedCount = 0;
            
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dy == 0 && dx == 0) continue;
                    int ny = y + dy, nx = x + dx;
                    if (ny >= 0 && ny < gridSize && nx >= 0 && nx < gridSize) {
                        if (!open[ny][nx]) {
                            closedCount++;
                            if (flag[ny][nx]) flaggedCount++;
                        }
                    }
                }
            }
            
            if (closedCount > 0 && (closedCount - flaggedCount) == (mineCount - flaggedCount)) {
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dy == 0 && dx == 0) continue;
                        int ny = y + dy, nx = x + dx;
                        if (ny >= 0 && ny < gridSize && nx >= 0 && nx < gridSize) {
                            if (!open[ny][nx] && !flag[ny][nx] && flagCount < mineCount) {
                                flag[ny][nx] = true;
                                flagCount++;
                            }
                        }
                    }
                }
            }
        }
    }
}

void initMines(int excludeY, int excludeX) {
    srand((unsigned)time(0));
    int placed = 0;
    while (placed < mineCount) {
        int x = rand() % gridSize;
        int y = rand() % gridSize;
        if (!mines[y][x] && !(y == excludeY && x == excludeX) && !special99[y][x]) {
            mines[y][x] = true;
            placed++;
            
            if (rand() % 100 < 5) {
                int attempts = 0;
                while (attempts < 5) {
                    int dy = (rand() % 3) - 1;
                    int dx = (rand() % 3) - 1;
                    if (dy == 0 && dx == 0) { attempts++; continue; }
                    
                    int ny = y + dy;
                    int nx = x + dx;
                    if (ny >= 0 && ny < gridSize && nx >= 0 && nx < gridSize && 
                        !special99[ny][nx] && !mines[ny][nx]) {
                        special99[ny][nx] = true;
                        break;
                    }
                    attempts++;
                }
            }
        }
    }
}

int countFlags() {
    int count = 0;
    for (int y = 0; y < gridSize; y++)
        for (int x = 0; x < gridSize; x++)
            if (flag[y][x]) count++;
    return count;
}

bool checkWin() {
    for (int y = 0; y < gridSize; y++)
        for (int x = 0; x < gridSize; x++)
            if (!mines[y][x] && !open[y][x])
                return false;
    return true;
}

void openCell(int y, int x) {
    if (y < 0 || y >= gridSize || x < 0 || x >= gridSize) return;
    if (open[y][x] || flag[y][x]) return;

    open[y][x] = true;
    if (countMines(y, x) == 0) {
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++)
                openCell(y + dy, x + dx);
    }
}

void spawn99Mines(int cy, int cx) {
    int minesPlaced = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue;
            
            int ny = cy + dy;
            int nx = cx + dx;
            if (ny >= 0 && ny < gridSize && nx >= 0 && nx < gridSize) {
                if (!mines[ny][nx] && !special99[ny][nx]) {
                    mines[ny][nx] = true;
                    minesPlaced++;
                }
            }
        }
    }
}

void init(int difficulty) {
    if (difficulty == 10) {
        gridSize = 9;
        cellSize = 40;
        mineCount = 10;
    } else if (difficulty == 40) {
        gridSize = 16;
        cellSize = 25;
        mineCount = 40;
    } else {
        gridSize = 32;
        cellSize = 25;
        mineCount = 160;
    }

    gameOver = false;
    gameWon = false;
    firstCellClicked = false;
    flagCount = 0;

    for (int y = 0; y < gridSize; y++)
        for (int x = 0; x < gridSize; x++)
            mines[y][x] = open[y][x] = flag[y][x] = special99[y][x] = false;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_ERASEBKGND:
        return TRUE;

    case WM_KEYDOWN: {
        char key = (char)wParam;
        if (key >= 'a' && key <= 'z') key = key - 32;
        
        if (key >= 'A' && key <= 'Z') {
            memmove(cheatCode, cheatCode + 1, 8);
            cheatCode[8] = key;
            cheatCode[9] = 0;
            
            if (strstr(cheatCode, "CHEAT") != NULL) {
                revealAllMines();
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        
        if (wParam == 'A' || wParam == 'a') {
            autoMode = !autoMode;
            if (autoMode && !gameOver) {
                autoSetFlags();
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        break;
    }

    case WM_LBUTTONDOWN: {
        if (gameOver) break;

        int x = LOWORD(lParam) / cellSize;
        int y = HIWORD(lParam) / cellSize;

        if (x < 0 || y < 0 || x >= gridSize || y >= gridSize) break;
        if (flag[y][x]) break;

        if (!firstCellClicked) {
            firstCellClicked = true;
            initMines(y, x);
        }

        if (mines[y][x]) {
            PlaySound(TEXT("boom.wav"), NULL, SND_FILENAME | SND_ASYNC);
            gameOver = true;

            int result = MessageBox(hwnd, "You lose 💥\n\nPlay again?", 
                "Minesweeper", MB_YESNO | MB_ICONQUESTION);

            if (result == IDYES)
                init(mineCount);
            else
                PostMessage(hwnd, WM_CLOSE, 0, 0);
        } else if (special99[y][x]) {
            spawn99Mines(y, x);
            open[y][x] = true;
            PlaySound(TEXT("boom.wav"), NULL, SND_FILENAME | SND_ASYNC);
            gameOver = true;

            int result = MessageBox(hwnd, "You unlocked 99 MINES! 💣\n\nPlay again?", 
                "Minesweeper", MB_YESNO | MB_ICONQUESTION);

            if (result == IDYES)
                init(mineCount);
            else
                PostMessage(hwnd, WM_CLOSE, 0, 0);
        } else {
            PlaySound(TEXT("open.wav"), NULL, SND_FILENAME | SND_ASYNC);
            openCell(y, x);

            if (checkWin()) {
                gameOver = true;
                gameWon = true;
                
                ShellExecute(NULL, "open", "victory.png", NULL, NULL, SW_SHOW);
                
                int result = MessageBox(hwnd, "You Won! 🎉\n\nPlay again?", 
                    "Minesweeper", MB_YESNO | MB_ICONINFORMATION);

                if (result == IDYES)
                    init(mineCount);
                else
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
        }

        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_RBUTTONDOWN: {
        if (gameOver) break;

        int x = LOWORD(lParam) / cellSize;
        int y = HIWORD(lParam) / cellSize;

        if (x < 0 || y < 0 || x >= gridSize || y >= gridSize) break;

        if (!open[y][x]) {
            if (flag[y][x]) {
                flag[y][x] = false;
                flagCount--;
            } else {
                if (countFlags() < mineCount) {
                    flag[y][x] = true;
                    flagCount++;
                }
            }
            waveOutSetVolume(NULL, 0x19991999);
            PlaySound(TEXT("flag.wav"), NULL, SND_FILENAME | SND_ASYNC);
            waveOutSetVolume(NULL, 0xFFFFFFFF);
        }

        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        // Двойная буферизация
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        // Красивый фон
        HBRUSH bgBrush = CreateSolidBrush(RGB(50, 50, 60));
        FillRect(memDC, &clientRect, bgBrush);
        DeleteObject(bgBrush);

        HBRUSH openBrush = CreateSolidBrush(RGB(220, 220, 230));
        HBRUSH closedBrush = CreateSolidBrush(RGB(120, 130, 150));
        HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(40, 40, 50));
        HPEN lightPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 210));
        
        // Создаём шрифты один раз
        HFONT font1 = CreateFont(cellSize - 10, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Arial");
        HFONT font2 = CreateFont(cellSize - 12, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Arial");

        for (int y = 0; y < gridSize; y++) {
            for (int x = 0; x < gridSize; x++) {
                RECT r = { x * cellSize, y * cellSize,
                           (x + 1) * cellSize, (y + 1) * cellSize };

                SelectObject(memDC, open[y][x] ? openBrush : closedBrush);
                SelectObject(memDC, borderPen);
                Rectangle(memDC, r.left, r.top, r.right, r.bottom);

                if (!open[y][x]) {
                    SelectObject(memDC, lightPen);
                    MoveToEx(memDC, r.left, r.top, NULL);
                    LineTo(memDC, r.right - 2, r.top);
                    MoveToEx(memDC, r.left, r.top, NULL);
                    LineTo(memDC, r.left, r.bottom - 2);
                }

                if (open[y][x]) {
                    if (special99[y][x]) {
    HFONT oldFont = (HFONT)SelectObject(memDC, font1);
    SetTextColor(memDC, RGB(255, 0, 0));
    DrawText(memDC, "???", 3, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SetTextColor(memDC, RGB(0, 0, 0));
    SelectObject(memDC, oldFont);
} else if (mines[y][x]) {
                        HFONT oldFont = (HFONT)SelectObject(memDC, font2);
                        SetTextColor(memDC, RGB(200, 50, 50));
                        DrawText(memDC, "X", 1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                        SetTextColor(memDC, RGB(0, 0, 0));
                        SelectObject(memDC, oldFont);
                    } else {
                        int m = countMines(y, x);
                        if (m > 0) {
                            char c = '0' + m;
                            HFONT oldFont = (HFONT)SelectObject(memDC, font1);
                            
                            if (m == 1) SetTextColor(memDC, RGB(20, 100, 200));
                            else if (m == 2) SetTextColor(memDC, RGB(20, 150, 30));
                            else if (m == 3) SetTextColor(memDC, RGB(200, 30, 30));
                            else if (m == 4) SetTextColor(memDC, RGB(30, 40, 150));
                            else if (m == 5) SetTextColor(memDC, RGB(150, 30, 30));
                            else if (m == 6) SetTextColor(memDC, RGB(30, 130, 130));
                            else if (m >= 7) SetTextColor(memDC, RGB(50, 50, 50));
                            
                            DrawText(memDC, &c, 1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                            SetTextColor(memDC, RGB(0, 0, 0));
                            SelectObject(memDC, oldFont);
                        }
                    }
                } else if (flag[y][x]) {
                    int cx = r.left + cellSize / 2;
                    int cy = r.top + cellSize / 2;
                    
                    HPEN redPen = CreatePen(PS_SOLID, 3, RGB(255, 80, 80));
                    HPEN oldPen = (HPEN)SelectObject(memDC, redPen);
                    
                    MoveToEx(memDC, cx, r.top + cellSize / 6, NULL);
                    LineTo(memDC, cx, r.bottom - cellSize / 6);
                    
                    HBRUSH redBrush = CreateSolidBrush(RGB(255, 100, 100));
                    RECT fr = { cx + 2, r.top + cellSize / 6,
                                cx + cellSize / 2 - 2, r.top + cellSize / 3 };
                    FillRect(memDC, &fr, redBrush);
                    
                    SelectObject(memDC, oldPen);
                    DeleteObject(redPen);
                    DeleteObject(redBrush);
                }
            }
        }

        DeleteObject(font1);
        DeleteObject(font2);
        DeleteObject(openBrush);
        DeleteObject(closedBrush);
        DeleteObject(borderPen);
        DeleteObject(lightPen);

        // Копируем буфер на экран
        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
        
        // Очищаем
        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int selectDifficulty() {
    int result = MessageBox(NULL,
        "Easy (9x9, 10 mines)?",
        "Difficulty", MB_YESNO | MB_ICONQUESTION);
    
    if (result == IDYES) return 10;
    
    result = MessageBox(NULL,
        "Hard (16x16, 40 mines)?",
        "Difficulty", MB_YESNO | MB_ICONQUESTION);
    
    if (result == IDYES) return 40;
    
    result = MessageBox(NULL,
        "Impossible (32x32, 160 mines)?",
        "Difficulty", MB_YESNO | MB_ICONQUESTION);
    
    if (result == IDYES) return 1000;
    
    return 10;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    init(selectDifficulty());

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "Minesweeper";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "Minesweeper", "Minesweeper",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        200, 200,
        gridSize * cellSize + 16,
        gridSize * cellSize + 39,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
