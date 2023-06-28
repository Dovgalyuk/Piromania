#ifndef MENU_H
#define MENU_H

#include <windows.h>

// нарисовать главное меню
void DrawMainMenu();
// инициализировать переменные
void CreateMainMenu();
// освободить память
void ReleaseMainMenu();
// обработка сообщений
bool MainMenuMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);


#endif