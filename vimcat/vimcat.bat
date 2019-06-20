@ECHO OFF
REM
REM A Windows port of vimcat
REM
SET tmp_dir=%TEMP%\vimcat_temp
SET tmp_file=%tmp_dir%\vimcat
IF -%1-==-- echo No input files provided & exit /b
IF EXIST %tmp_dir% GOTO SKIP_DIR_CREATE
    MKDIR %tmp_dir%
:SKIP_DIR_CREATE
IF EXIST %tmp_file% (
    COPY /b /y NUL %tmp_file% > NUL
)

vim -X -R -i NONE -c "visual | call AnsiHighlight('%tmp_file%') | q" -- %1 > NUL 2> NUL & TYPE %tmp_file%
:END
