#include <windows.h>
#include <stdio.h>
#define INPUT_BUFFER_SIZE 1024
#ifdef VIM_CONSOLE // Using console vim
  #define VIM_EXE "vim "
#endif
#ifdef VIM_GUI // Using gui vim
  #define VIM_EXE "gvim "
#endif
INT main() {
  INT iBytesRead, iBytesWritten = 0;
  CHAR wsInputBuffer[INPUT_BUFFER_SIZE];
  HANDLE hTempFile;
  CHAR szTempDirectoryPath[MAX_PATH];
  CHAR szTempFileName[MAX_PATH];
  CHAR szFullCommand[MAX_PATH+sizeof(VIM_EXE)];
  STARTUPINFO si;
  SECURITY_ATTRIBUTES sa;
  PROCESS_INFORMATION pi;
  // Get %TEMP% env variable
  GetTempPath(MAX_PATH, (LPSTR)&szTempDirectoryPath);
  // Create unique temp bat file
  GetTempFileName(szTempDirectoryPath, "edx", 0, szTempFileName);
  // If piped input, then pipe it to the empty file
  if(GetFileType(GetStdHandle(STD_INPUT_HANDLE)) == FILE_TYPE_PIPE) {
    // Write temp file
    hTempFile = CreateFile((LPTSTR) szTempFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    // Read stdin
    do {
      ReadFile(GetStdHandle(STD_INPUT_HANDLE), wsInputBuffer, INPUT_BUFFER_SIZE, &iBytesRead, NULL);
      WriteFile(hTempFile, wsInputBuffer, iBytesRead, &iBytesWritten, NULL);
    } while(iBytesRead);
    CloseHandle(hTempFile);
  }
  else {
    printf("No input detected.\n");
    ExitProcess(1);
  }
#ifdef VIM_CONSOLE // Using console vim
  // Reset  & reopen stdout for vim terminal
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;
  freopen("CONOUT$", "w", stdout);
  SetStdHandle(STD_OUTPUT_HANDLE,
    CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING, 0, 0));
  ZeroMemory( &sa, sizeof(sa) );

  // Reset stdin to FILE_TYPE_CHAR for console vim
  freopen("CONIN$", "r", stdin);
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;
  SetStdHandle(STD_INPUT_HANDLE,
    CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING, 0, 0));
  ZeroMemory( &sa, sizeof(sa) );
#endif
  // Build command path
  lstrcpy(szFullCommand, VIM_EXE);
  lstrcat(szFullCommand, szTempFileName);
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  ZeroMemory( &sa, sizeof(sa) );
  if( !CreateProcess( NULL, szFullCommand, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi)) {
    printf( "Unable to create editor process: %d\n", GetLastError() );
    ExitProcess(1);
  }
  WaitForSingleObject( pi.hProcess, INFINITE );
  CloseHandle( pi.hProcess );
  CloseHandle( pi.hThread );
  // Print the file to stdout
  hTempFile = CreateFile((LPTSTR) szTempFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if(hTempFile == NULL) {
    printf("Could not read created temporary file\n");
  }
  do {
    ReadFile(hTempFile, wsInputBuffer, INPUT_BUFFER_SIZE, &iBytesRead, NULL);
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), wsInputBuffer, iBytesRead, &iBytesWritten, NULL);
  } while(iBytesRead);
  CloseHandle(hTempFile);
  ExitProcess(0);
}
