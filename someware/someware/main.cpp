#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <thread>
#include <mutex>

using namespace std;

struct WindowInfo {
    HWND hwnd;
    int dx, dy, width, height;
};

std::vector<WindowInfo> windows;
std::vector<std::thread> threads;
std::mutex windowMutex;
float movespeed;

void MoveWindowThread(WindowInfo w, float speed, int screenWidth, int screenHeight) {
    while (true) {
        RECT rect;
        GetWindowRect(w.hwnd, &rect);

        MoveWindow(w.hwnd, rect.left + w.dx * speed, rect.top + w.dy * speed, w.width, w.height, TRUE);

        if (rect.left + w.width + w.dx * speed >= screenWidth || rect.left + w.dx * speed <= 0) w.dx *= -1;
        if (rect.top + w.height + w.dy * speed >= screenHeight || rect.top + w.dy * speed <= 0) w.dy *= -1;

        Sleep(1);
    }
}
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (IsWindowVisible(hwnd) && !IsIconic(hwnd)) {
        if (hwnd == GetShellWindow() || hwnd == GetDesktopWindow()) {
            return TRUE;
        }
        RECT rect;
        GetWindowRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        if (width > 0 && height > 0) {
            std::lock_guard<std::mutex> lock(windowMutex);
            bool exists = false;
            for (const auto& w : windows) {
                if (w.hwnd == hwnd) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                WindowInfo info;
                info.hwnd = hwnd;
                info.dx = (rand() % 2 == 0) ? 3 : -3;
                info.dy = (rand() % 2 == 0) ? 3 : -3;
                info.width = width;
                info.height = height;
                windows.push_back(info);

                threads.emplace_back(MoveWindowThread, info, movespeed, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
            }
        }
    }
    return TRUE;
}
void MonitorNewWindows() {
    while (true) {
        EnumWindows(EnumWindowsProc, 0);
        Sleep(1000);
    }
}

int main(int argc, char* argv[]) {
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    
    if (argc == 2) movespeed = stof(argv[1]);
    else movespeed = 1;
    srand(static_cast<unsigned int>(time(nullptr)));



    EnumWindows(EnumWindowsProc, 0);

    std::thread monitorThread(MonitorNewWindows);

    for (auto& t : threads) {
        t.join();
    }
    monitorThread.join();

    return 0;
}
