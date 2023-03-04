#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <time.h>
#include <windows.h>

using namespace std;

#define NewLineExit(n) {cout << endl; return n;}

#define ERR_OK 0
#define ERR_START_EXE 1
#define ERR_WRITE_FILE 2
#define ERR_SET_TIME_MISSING_PARAMS 3
#define ERR_SET_TIME_INVALID_DATE 4

char *generateCode(struct tm *, bool);
bool writeToDisk(char *);
PROCESS_INFORMATION *startExecutable();

int main(int argc, char **argv) {
	
        bool dryrun = false, setTime = false, noNaNFix = false, noDaemon = false;

        if (argc > 1) {
                bool help = false;
                for (int i = 1; i < argc; i++) {
                        if (!strcmp("-d", argv[i]) || !strcmp("--dry-run", argv[i]))    dryrun = true;
                        if (!strcmp("-t", argv[i]) || !strcmp("--time", argv[i]))       setTime = true;
                        if (!strcmp("-f", argv[i]) || !strcmp("--no-nan-fix", argv[i])) noNaNFix = true;
                        if (!strcmp("-m", argv[i]) || !strcmp("--no-daemon", argv[i]))  noDaemon = true;
                        if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i]))       help = true;
                }
                if (!(dryrun || setTime || noNaNFix || noDaemon) || help) {
                        cout << endl << "agdsxw_t Launcher v2.0" << endl << endl;
                        cout << "Usage:" << endl;
                        cout << "\t-d / --dry-run:\t\tOnly print verification code to console. (Implies --no-daemon)" << endl;
                        cout << "\t-t / --time:\t\tSpecify a date interactively to generate a code. (Implies --dry-run)" << endl;
                        cout << "\t\t\t\tNote that the year 1999 is blacklisted for some reason." << endl;
                        cout << "\t-f / --no-nan-fix:\tDisable fix for the issue where 'c' equals NaN on the first day of the month." << endl;
                        cout << "\t-m / --no-daemon:\tDisable daemon for refreshing code on date change." << endl;
                        cout << "\t-h / --help:\t\tShow usage." << endl;
                        NewLineExit(ERR_OK);
                }
        }

        // Get current time
        time_t t; time(&t);
        struct tm *timeObj = localtime(&t);

        // Interactive time setting
        if (setTime) {
                dryrun = true;

                int day, mon, year;
                cout << "Enter a date (d/m/y): " << flush;
                if (scanf("%d/%d/%d", &day, &mon, &year) < 3) {
                        cout << "Missing parameters." << endl;
                        NewLineExit(ERR_SET_TIME_MISSING_PARAMS);
                }
                if (day < 1 || day > 31 || mon < 1 || mon > 12 || year < 1900 || year > 9999) {
                        cout << "Invalid date." << endl;
                        NewLineExit(ERR_SET_TIME_INVALID_DATE);
                }
                mon -= 1; year -= 1900;
                timeObj->tm_mday = day; timeObj->tm_mon = mon; timeObj->tm_year = year;
        }

        // Generate code (and write to disk)
        char *UnEncryCode = generateCode(timeObj, noNaNFix);
        if (dryrun)
                NewLineExit(ERR_OK);
        if (!writeToDisk(UnEncryCode))
                NewLineExit(ERR_WRITE_FILE);

        // Start main executable
        PROCESS_INFORMATION *pi = startExecutable();
        if (!pi)
                NewLineExit(ERR_START_EXE);

        // Look for date change, exit with the main executable
        if (!noDaemon) {
                DWORD exitCode;
                int prevDay = timeObj->tm_mday, prevMon = timeObj->tm_mon, prevYear = timeObj->tm_year;
                do {
                        Sleep(500);
                        time(&t); timeObj = localtime(&t);  // Update time
                        if (timeObj->tm_mday != prevDay || timeObj->tm_mon != prevMon || timeObj->tm_year != prevYear) {
                                prevDay = timeObj->tm_mday, prevMon = timeObj->tm_mon, prevYear = timeObj->tm_year;
                                writeToDisk(generateCode(timeObj, noNaNFix));
                        }
                        GetExitCodeProcess(pi->hProcess, &exitCode);

                } while (exitCode == STILL_ACTIVE);
        }

        // Close handle and exit
        CloseHandle(pi->hProcess);
        CloseHandle(pi->hThread);
        NewLineExit(ERR_OK);
}

char *generateCode(struct tm *timeObj, bool noNaNFix) {
        int multiplier = timeObj->tm_mday > 9 ? 100 : 1000;
        int remainder = (timeObj->tm_year + 1900) % timeObj->tm_mday;
        static char UnEncryCode[32];
        if (remainder || noNaNFix) {
                int c = pow(remainder, 2) + (timeObj->tm_mon + 1) + timeObj->tm_mday;
                sprintf(UnEncryCode, "UnEncryCode=%d%d%d", multiplier, c, timeObj->tm_mday * multiplier);
        } else {
                // In ActionScript 1, Math.pow(0, n) returns NaN for whatever the fuck reason it may be.
                sprintf(UnEncryCode, "UnEncryCode=%dNaN%d", multiplier, timeObj->tm_mday * multiplier);
        }

        cout << "Verification code is " << UnEncryCode + 12 << endl;
        return UnEncryCode;
}

bool writeToDisk(char *UnEncryCode) {
        ofstream fout;
        fout.open("agdsxw_t\\UnEncryCode.txt", ios::out | ios::trunc);
        if (!fout.is_open()) {
                cout << "Failed to open agdsxw_t\\UnEncryCode.txt" << endl;
                return false;
        }
        fout << UnEncryCode << flush;
        fout.close();
        return true;
}

PROCESS_INFORMATION *startExecutable() {
        char cmdLine[] = "agdsxw_t\\agdsxw_t.exe";
        static PROCESS_INFORMATION pi = {0};
        STARTUPINFO si = {0};
        si.cb = sizeof(si);
        cout << "Launching agdsxw_t\\agdsxw_t.exe" << endl;
        if (CreateProcess(NULL, cmdLine, NULL, NULL, false, 0, NULL, NULL, &si, &pi)) {
                cout << "Executable launched successfully." << endl;
                return &pi;
        } else {
                cout << "Failed to launch executable." << endl;
                return nullptr;
        }
}
