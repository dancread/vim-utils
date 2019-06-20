#include <windows.h>
#include <string.h>
#include <shlwapi.h>
#include <stdio.h>

/*
 * Consider
 * https://support.microsoft.com/en-us/help/830473/command-prompt-cmd.-exe-command-line-string-limitation
 */
/*
 * use remote gvim server to open lots of files
 */
#define VIM_EXE "gvim.exe"
#define VIM_OPTS "--servername vimopen "
void SearchRecursively(LPCTSTR lpFolder, LPCTSTR lpFilePattern, void (*FindFileCallBack)(char *));
void SearchCallBack(char *szFoundFile);
INT main(int argc, char **argv) {
    CHAR szCommand[MAX_PATH];
    CHAR szInputBuffer[MAX_PATH];
    CHAR szCWD[MAX_PATH];
    GetCurrentDirectory(MAX_PATH,szCWD);
    // If piped input, then open those files
    if(GetFileType(GetStdHandle(STD_INPUT_HANDLE)) == FILE_TYPE_PIPE){
      ShellExecute(NULL, "open",VIM_EXE, VIM_OPTS, NULL, SW_SHOWDEFAULT);
      // Read stdin
      while (fgets(szInputBuffer, sizeof(szInputBuffer),stdin)) {
        // Strip newline fro fgets return
        szInputBuffer[strcspn(szInputBuffer, "\r\n")] = 0;
        PathQuoteSpaces(szInputBuffer);
        lstrcpyn(szCommand, VIM_OPTS, MAX_PATH);
        lstrcat(szCommand, "--remote ");
        lstrcat(szCommand, szInputBuffer);
        ShellExecute(NULL, "open",VIM_EXE, szCommand, NULL, SW_SHOWDEFAULT);
      }
      ExitProcess(0);
    }
    // Else use the argument
    else{
      ShellExecute(NULL, "open",VIM_EXE, VIM_OPTS, NULL, SW_SHOWDEFAULT);
      SearchRecursively(szCWD,argv[1],SearchCallBack);
      ExitProcess(0);
    }
    ExitProcess(1);
}
void SearchRecursively(LPCTSTR lpFolder, LPCTSTR lpFilePattern, void (*FindFileCallBack)(char *))
{
    TCHAR szFullPattern[MAX_PATH];
    WIN32_FIND_DATA FindFileData;
    HANDLE hFindFile;
    // first we are going to process any subdirectories
    PathCombine(szFullPattern, lpFolder, "*");
    hFindFile = FindFirstFile(szFullPattern, &FindFileData);
    if(hFindFile != INVALID_HANDLE_VALUE)
    {
        do
        {
          if(strcmp(FindFileData.cFileName,".") && strcmp(FindFileData.cFileName,"..")){
            if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                // found a subdirectory; recurse into it
                PathCombine(szFullPattern, lpFolder, FindFileData.cFileName);
                SearchRecursively(szFullPattern, lpFilePattern,FindFileCallBack);
            }
          }
        } while(FindNextFile(hFindFile, &FindFileData));
        FindClose(hFindFile);
    }

    // Now we are going to look for the matching files
    PathCombine(szFullPattern, lpFolder, lpFilePattern);
    hFindFile = FindFirstFile(szFullPattern, &FindFileData);
    if(hFindFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
              // found a file; do somethinG with it
              PathCombine(szFullPattern, lpFolder, FindFileData.cFileName);
              PathQuoteSpaces(szFullPattern);
              SearchCallBack(szFullPattern);
            }
        } while(FindNextFile(hFindFile, &FindFileData));
        FindClose(hFindFile);
    }
}
void SearchCallBack(char *szFoundFile){
  CHAR szCommand[MAX_PATH];
  printf("Opening: %s\n",szFoundFile);
  lstrcpyn(szCommand, VIM_OPTS, MAX_PATH);
  lstrcat(szCommand, "--remote ");
  lstrcat(szCommand, szFoundFile);
  ShellExecute(NULL, "open",VIM_EXE, szCommand, NULL, SW_SHOWDEFAULT);
}
