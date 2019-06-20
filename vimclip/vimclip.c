#include <windows.h>
#include <stdio.h>
#define INPUT_BUFFER_SIZE 1024
#define VIM_CONSOLE
#ifdef VIM_CONSOLE // Using console vim
    #define VIM_EXE "vim "
#endif
#ifdef VIM_GUI // Using gui vim
    #define VIM_EXE "gvim "
#endif
INT main() {
    HANDLE hClipBoardData;
    INT iBytesRead, iBytesWritten = 0;
    CHAR wsInputBuffer[INPUT_BUFFER_SIZE];
    CHAR * lpszText;
    HANDLE hTempFile;
    CHAR szTempDirectoryPath[MAX_PATH];
    CHAR szTempFileName[MAX_PATH];
    CHAR szFullCommand[MAX_PATH+sizeof(VIM_EXE)];
    // Get %TEMP% env variable
    GetTempPath(MAX_PATH, &szTempDirectoryPath);
    // Create unique temp bat file
    GetTempFileName(szTempDirectoryPath, "edx", 0, szTempFileName);
    // Write temp file
    hTempFile = CreateFile((LPTSTR) szTempFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    // Start vim with that unique file
#ifdef VIM_CONSOLE // Using console vim
    // Reset stdin to FILE_TYPE_CHAR for console vim
    freopen("CONIN$", "r", stdin);
    // Reset stdout for pdcurses
    freopen("CONOUT$", "w", stdout);
#endif
    // Attempt to open the system clipboard
    if (! OpenClipboard(NULL)){
      printf("Can not access clipboard.\n");
      ExitProcess(1);
    }
    // Get clipboard data
    hClipBoardData = GetClipboardData(CF_TEXT);
    if (hClipBoardData == NULL){
      printf("Clipboard data is null.\n");
      ExitProcess(1);
    }
    // Lock the handle to get the actual text pointer
    lpszText = GlobalLock(hClipBoardData);
    if (lpszText == NULL){
      printf("Clipboard text is empty.\n");
      ExitProcess(1);
    }
    // Write clipboard data to the file
    WriteFile(hTempFile, lpszText, lstrlen(lpszText), &iBytesWritten, NULL);
    CloseHandle(hTempFile);
    // Release the lock
    GlobalUnlock( hClipBoardData );
    // Release the clipboard
    CloseClipboard();
    // Build command path and execute vim
    lstrcpy(szFullCommand, VIM_EXE);
    lstrcat(szFullCommand, szTempFileName);
    system(szFullCommand);
    // Read the file back
    hTempFile = CreateFile((LPTSTR) szTempFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile(hTempFile, wsInputBuffer, INPUT_BUFFER_SIZE, &iBytesRead, NULL);
    CloseHandle(hTempFile);
    wsInputBuffer[iBytesRead] = '\0';
    // Attempt to open the system clipboard
    if (!OpenClipboard(NULL)){
      printf("Can not access clipboard.\n");
      ExitProcess(1);
    }
    EmptyClipboard();
    // Set clipboard data
    hClipBoardData = SetClipboardData(CF_TEXT, wsInputBuffer);
    if (hClipBoardData == NULL){
      printf("Error in setting clipboard data.\n");
      printf("%s\n", wsInputBuffer);
      ExitProcess(1);
    }
    CloseClipboard();
    ExitProcess(0);
}
