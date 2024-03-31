#pragma once

// Inkludiere erforderliche Headerdateien
#include "gidxwin.h"
#include <thread>
#include <future>
#include <windows.h>

// Deklariere eine Handle-Variablendie f�r einen Thread verwendet wird
HANDLE hThread;

// Deklariere die Fensterprozedur
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

// Deklariere die Funktion, die als Einstiegspunkt f�r einen Thread verwendet wird
DWORD main(LPVOID pvoid);

// Deklariere den Namespace gdx und seine Funktionen
namespace gdx
{
    // Funktion zur Erstellung eines GIDX-Fensters
    int CreateGidx(int, int, int, int, int);

    // Funktion zum Herunterfahren des GIDX-Fensters
    void MainLoop(bool);

    // Funktion zum Freigeben von Ressourcen
    void Release(void);

    // Funktion zum Einf�rben des Hintergrunds
    void SetWindowBackgroundColor(int r, int g, int b, HWND hwnd);
}

