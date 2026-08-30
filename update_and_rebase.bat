@echo off
setlocal

set GIT_CMD=git
%GIT_CMD% --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    set GIT_CMD="C:\Program Files\Git\cmd\git.exe"
    %GIT_CMD% --version >nul 2>&1
    if %ERRORLEVEL% neq 0 (
        echo Git is not found in PATH or C:\Program Files\Git\cmd\.
        pause
        exit /b 1
    )
)

echo Fetching latest updates from upstream (release-3_44)...
%GIT_CMD% fetch upstream release-3_44
if %ERRORLEVEL% neq 0 (
    echo Failed to fetch upstream.
    pause
    exit /b %ERRORLEVEL%
)

echo Rebasing current branch onto upstream release-3_44...
%GIT_CMD% rebase FETCH_HEAD
if %ERRORLEVEL% neq 0 (
    echo Rebase encountered conflicts or an error.
    echo Please resolve any conflicts manually.
    echo After resolving conflicts, run: %GIT_CMD% rebase --continue
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Repository updated and commits rebased successfully!
pause
