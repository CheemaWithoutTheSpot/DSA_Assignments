#include"Q1.h"
using namespace std;
#define idd //comment out for removing enter button and menu;

#ifdef idd
void printMenu()
{
    printf("\n----------------------------------------\n");
    printf(" 0  EXIT\n");
    printf(" 1  ADD id sign\n");
    printf(" 2  LOG id sensor value status\n");
    printf(" 3  REMOVE id\n");
    printf(" 4  FIND id\n");
    printf(" 5  PRINT_A\n");
    printf(" 6  GROW\n");
    printf(" 7  ADD_BYVAL id sign\n");
    printf(" 8  CLONE id\n");
    printf(" 9  PRINT_CLONE\n");
    printf("10  ALIAS id\n");
    printf("11  SEED_A\n");
    printf("12  SEED_B\n");
    printf("13  PRINT_B\n");
    printf("14  MERGE_B\n");
    printf("15  MERGE_SELF\n");
    printf("16  DESTROY\n");
    printf("17  SIZES\n");
    printf("----------------------------------------\n");
    printf("Enter command: ");
}
#endif


int main()
{
    Fleet A, B;
    Probe* clone = nullptr;

    initFleet(A, P1);
    initFleet(B, 2);

    printf("=== MISSION CONTROL ===\n");
    printf("ROLL_N=%d P1=%d P2=%d P3=%d\n", ROLL_N, P1, P2, P3);

    int cmd;
    bool running = true;

    while (running)
    {
        printMenu();
        std::cin >> cmd;

        switch (cmd)
        {
        case 0:
        {
            running = false;
            break;
        }
        case 1:
        {
            int id;
            char sign[64];
            cin >> id;
            cin.width(64);
            cin >> sign;

            if (addProbe(A, id, sign))
            {
                printf("OK PROBE_ADDED %04d\n", id);
            }
            break;
        }
        case 2: 
        {
            int id;
            char sensor[64];
            float value;
            char status;
            cin >> id;
            cin.width(64);
            cin >> sensor;
            cin >> value >> status;

            Probe* p = findProbe(A, id);
            if (p == nullptr)
            {
                printf("ERR NOT_FOUND\n");
            }
            else if (addReading(p, sensor, value, status))
            {
                printf("OK LOG_ADDED %04d %s\n", id, sensor);
            }
            break;
        }
        case 3: 
        {
            int id;
            cin >> id;
            if (removeProbe(A, id))
            {
                printf("OK PROBE_REMOVED %04d\n", id);
            }
            break;
        }
        case 4:
        {
            int id;
            cin >> id;
            Probe* p = findProbe(A, id);
            if (p == nullptr)
            {
                printf("ERR NOT_FOUND\n");
            }
            else
            {
                printf("OK FOUND %04d\n", id);
                printProbe(p);
            }
            break;
        }
        case 5: 
        {
            printf("A:\n");
            printFleet(A);
            break;
        }
        case 6:
        {
            if (growFleet(A))
            {
                printf("OK FLEET_GROWN capacity=%d\n", A.capacity);
            }
            break;
        }
        case 7:
        {
            int id;
            char sign[64];
            cin >> id;
            cin.width(64);
            cin >> sign;

            printf("BYVAL pre count=%d capacity=%d\n", A.count, A.capacity);
            bool result = addProbeByValue(A, id, sign);
            printf("BYVAL returned=%d\n", result ? 1 : 0);
            printf("BYVAL post count=%d capacity=%d\n", A.count, A.capacity);
            break;
        }
        case 8:
        {
            int id;
            cin >> id;
            Probe* p = findProbe(A, id);
            if (p == nullptr)
            {
                printf("ERR NOT_FOUND\n");
            }
            else
            {
                destroyProbe(clone);
                deepCopyProbe(p, clone);
                printf("OK CLONED %04d\n", id);
            }
            break;
        }
        case 9: 
        {
            if (clone == nullptr)
            {
                printf("ERR NO_CLONE\n");
            }
            else
            {
                printf("CLONE:\n");
                printProbe(clone);
            }
            break;
        }
        case 10: 
        {
            int id;
            cin >> id;
            Probe* src = findProbe(A, id);
            if (src == nullptr)
            {
                printf("ERR NOT_FOUND\n");
                break;
            }

            Probe copy;
            copy.probeId = 0;
            copy.callSign = nullptr;
            copy.readings = nullptr;
            copy.readingCount = 0;
            copy.readingCapacity = 0;

            aliasCopyProbe(src, &copy);

            int signSame = (copy.callSign == src->callSign) ? 1 : 0;
            int logsSame = (copy.readings == src->readings) ? 1 : 0;
            printf("ALIAS sign=%d logs=%d\n", signSame, logsSame);

            if (copy.callSign != nullptr && myStrLen(copy.callSign) > 0)
            {
                *(copy.callSign) = 'Z';
            }

            char st;
            if (copy.readings != nullptr && copy.readingCount > 0)
            {
                (copy.readings)->status = 'C';
                st = (src->readings)->status; 
            }
            else
            {
                st = '-';
            }

            printf("ALIAS after sign=%s log0=%c\n", src->callSign, st);
            break;
        }
        case 11:
        {
            loadFleetA(A);
            printf("OK SEED_A count=%d\n", A.count);
            break;
        }
        case 12: 
        {
            loadFleetB(B);
            printf("OK SEED_B count=%d\n", B.count);
            break;
        }
        case 13: 
        {
            printf("B:\n");
            printFleet(B);
            break;
        }
        case 14:
        {
            int before = A.count;
            if (mergeFleets(A, B))
            {
                printf("OK MERGED added=%d count=%d capacity=%d\n",
                    A.count - before, A.count, A.capacity);
            }
            else
            {
                printf("ERR MERGE_FAILED\n");
            }
            break;
        }
        case 15:
        {
            mergeFleets(A, A);
            break;
        }
        case 16:
        {
            destroyFleet(A);
            printf("OK FLEET_DESTROYED\n");
            break;
        }
        case 17: 
        {
            reportSizes();
            break;
        }
        default:
        {
            printf("ERR BAD_CMD\n");
            break;
        }
        }

#ifdef idd
        printf("\nPress Enter to continue...");
        cin.ignore(1000, '\n');
        cin.get();
        printf("\x1B[2J\x1B[H");  
#endif
    }

    if (clone != nullptr)
    {
        destroyProbe(clone);
    }
    destroyFleet(A);
    destroyFleet(B);
    printf("BYE\n");

    return 0;
}