#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <time.h>

using namespace std;

int main(int argc, char **argv) {

        bool dryrun = false;
        bool setTime = false;

        if (argc > 1) {
                bool help = false;
                for (int i = 1; i < argc; i++) {
                        if (!strcmp("-d", argv[i]) || !strcmp("--dry-run", argv[i])) dryrun = true;
                        if (!strcmp("-t", argv[i]) || !strcmp("--time", argv[i]))    setTime = true;
                        if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i]))    help = true;
                }
                if (!(dryrun || setTime) || help) {
                        cout << "agdsxw_t Launcher v1.2" << endl << endl;
                        cout << "Usage:" << endl;
                        cout << "\t-d / --dry-run:\tGenerate verification code without writing it to disk and launching the main executable." << endl;
                        cout << "\t-t / --time: \tSpecify a date interactively to generate a code. (Implies --dry-run)" << endl;
                        cout << "\t\t\tNote that the year 1999 is blacklisted for some reason." << endl;
                        cout << "\t-h / --help:\tShow usage." << endl << endl;
                        return 0;
                }
        }

        ofstream fout;
        time_t t; time(&t);
        struct tm *timeObj = localtime(&t);

        if (setTime) {
                dryrun = true;

                int day, mon, year;
                cout << "Enter a date (d/m/y): " << flush;
                if (scanf("%d/%d/%d", &day, &mon, &year) < 3) {
                        cout << "Missing parameters." << endl << endl;
                        return 1;
                }
                if (day < 1 || day > 31 || mon < 1 || mon > 12 || year < 1900 || year > 9999) {
                        cout << "Invalid date." << endl << endl;
                        return 1;
                }
                mon -= 1; year -= 1900;
                timeObj->tm_mday = day; timeObj->tm_mon = mon; timeObj->tm_year = year;
        }

        int multiplier = timeObj->tm_mday > 9 ? 100 : 1000;
        int remainder = (timeObj->tm_year + 1900) % timeObj->tm_mday;
        char UnEncryCode[32];
        if (remainder) {
                int c = pow(remainder, 2) + (timeObj->tm_mon + 1) + timeObj->tm_mday;
                sprintf(UnEncryCode, "UnEncryCode=%d%d%d", multiplier, c, timeObj->tm_mday * multiplier);
        } else {
                // In ActionScript 1, Math.pow(0, n) returns NaN for whatever the fuck reason it may be.
                sprintf(UnEncryCode, "UnEncryCode=%dNaN%d", multiplier, timeObj->tm_mday * multiplier);
        }

        char *justCode = UnEncryCode + 12;  // Jump over "UnEncryCode="
        cout << "Verification code is " << justCode << endl;

        if (!dryrun) {
                fout.open("agdsxw_t\\UnEncryCode.txt", ios::out | ios::trunc);
                if (!fout.is_open()) {
                        cout << "Failed to open agdsxw_t\\UnEncryCode.txt" << endl << endl;
                        return 1;
                }
                fout << UnEncryCode << flush;
                fout.close();
                cout << "Launching agdsxw_t\\agdsxw_t.exe" << endl;
                if (system("start agdsxw_t\\agdsxw_t.exe")) {
                        cout << "An error occurred while launching the executable." << endl << endl;
                        return 1;
                }
        }

        cout << endl;

        return 0;
}