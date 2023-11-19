#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <iomanip>
#include <locale>

void startup(LPSTR command)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessA (NULL, command, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}


struct Comma final : std::numpunct<char>
{
    char do_decimal_point() const override { return ','; }
};
int main() {
    const std::string lab = "Lab4.exe";

    const int PROCESSES = 4;
    int processes[PROCESSES] = { 1, 2, 4, 8 };

    const int SIZE = 7, TIMES = 3;
    int sizes[SIZE] = { 10, 100, 10000,20000, 30000, 40000, 50000};

    for (int p = 0; p < PROCESSES; p++) {

        std::ofstream file{ "logs.txt" };
        file.close();

        for (int i = 0; i < SIZE; i++) {
            std::string command = lab + " " + std::to_string(sizes[i]);

            if (processes[p] > 1) {
                command = "mpiexec -n " + std::to_string(processes[p]) + " " + 
                    command + " 1";
            }
            

            for (int j = 0; j < TIMES; j++) startup((LPSTR)(command.c_str()));
        }

        std::ifstream input{ "logs.txt" };
        std::ofstream out{ std::to_string(processes[p]) + "-result.txt" }, 
            out_short{ std::to_string(processes[p]) + "-result-short.txt" };

        out_short << std::fixed << std::setprecision(3);
        out_short.imbue(std::locale(std::locale::classic(), new Comma()));

        for (int i = 0; i < SIZE; i++) {
            double time = 0, current;
            out << "Size: " << sizes[i] << " -> ";
            for (int j = 0; j < TIMES; j++) {
                input >> current;
                time += current;
                out << current << " ";
            }
            time /= TIMES;

            out << " => " << time << "\n";
            out_short << time << "\n";
        }

        input.close();
        out.close();
        out_short.close();
    }
    
    return 0;
}
