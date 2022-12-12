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

        if (argc > 1) {
                bool help = false;
                for (int i = 1; i < argc; i++) {
                        if (!strcmp("-d", argv[i]) || !strcmp("--dry-run", argv[i])) dryrun = true;
                        if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i])) help = true;
                }
                if (!dryrun || help) {
                        cout << "agdsxw_t Launcher v1.0" << endl << endl;
                        cout << "Usage:" << endl;
                        cout << "\t-d / --dry-run:\tGenerate verification code without writing it to disk and launching the main executable." << endl;
                        cout << "\t-h / --help:\tShow usage." << endl << endl;
                        return 0;
                }
        }

        ofstream fout;
        time_t t; time(&t);
        struct tm *timeObj = localtime(&t);

        int multiplier = timeObj->tm_mday > 9 ? 100 : 1000;
        int c = pow((timeObj->tm_year + 1900) % timeObj->tm_mday, 2) + (timeObj->tm_mon + 1) + timeObj->tm_mday;
        char UnEncryCode[32];
        sprintf(UnEncryCode, "UnEncryCode=%d%d%d", multiplier, c, timeObj->tm_mday * multiplier);

        cout << "Verification code is " << UnEncryCode << endl;

        if (!dryrun) {
                fout.open("agdsxw_t\\UnEncryCode.txt", ios::out | ios::trunc);
                if (!fout.is_open()) {
                        cout << "Failed to open agdsxw_t\\UnEncryCode.txt" << endl << endl;
                        return 1;
                }
                fout << UnEncryCode;
                fout.flush(); fout.close();
                cout << "Launching agdsxw_t\\agdsxw_t.exe" << endl;
                if (system("start agdsxw_t\\agdsxw_t.exe")) {
                        cout << "An error occurred while launching the exectuable." << endl << endl;
                        return 1;
                }
        }

        cout << endl;

        return 0;
}