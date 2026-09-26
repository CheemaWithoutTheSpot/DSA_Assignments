#include "Q2.h"

#define INTERACTIVE_MODE

#include <cstdio>

#ifdef INTERACTIVE_MODE
void printMenu()
{
    printf("\n----------------------------------------\n");
    printf(" 0  EXIT\n");
    printf(" 1  BOOK day id name service price\n");
    printf(" 2  CANCEL day slot\n");
    printf(" 3  PRINT_DAY day\n");
    printf(" 4  PRINT_WEEK\n");
    printf(" 5  MOVE fromDay fromSlot toDay\n");
    printf(" 6  BUILD_INDEX\n");
    printf(" 7  PRINT_INDEX\n");
    printf(" 8  SORT_INDEX\n");
    printf(" 9  DROP_INDEX\n");
    printf("10  FIND id\n");
    printf("11  SEED\n");
    printf("12  BOOK_BYVAL day id name service price\n");
    printf("13  DESTROY_WEEK\n");
    printf("14  UTILS\n");
    printf("----------------------------------------\n");
    printf("Enter command: ");
}
#endif

int main()
{
    Week w;
    initWeek(w);

    Appointment** index = nullptr;
    int indexSize = 0;

    printf("=== GLOW & GRACE SALON ===\n");
    printf("ROLL_N=%d P2=%d P3=%d\n", ROLL_N, P2, P3);

    int cmd;
    bool running = true;

    while (running && (
#ifdef INTERACTIVE_MODE
        printMenu(),
#endif
        cin >> cmd))
    {
        switch (cmd)
        {
        case 0:
        {
            running = false;
            break;
        }
        case 1: 
        {
            int day, id;
            char name[64], service[64];
            float price;
            cin >> day >> id;
            cin.width(64); cin >> name;
            cin.width(64); cin >> service;
            cin >> price;

            if (index != nullptr) destroyIndex(index, indexSize);

            if (bookAppointment(w, day, id, name, service, price))
            {
                printf("OK BOOKED d%d %04d\n", day, id);
            }
            break;
        }
        case 2: 
        {
            int day, slot;
            cin >> day >> slot;

            if (index != nullptr) destroyIndex(index, indexSize);

            if (cancelAppointment(w, day, slot))
            {
                printf("OK CANCELLED d%d s%d\n", day, slot);
            }
            break;
        }
        case 3: 
        {
            int day;
            cin >> day;

            if (w.days == nullptr)
            {
                printf("ERR WEEK_DESTROYED\n");
            }
            else if (day < 0 || day >= DAYS_IN_WEEK)
            {
                printf("ERR BAD_DAY\n");
            }
            else
            {
                printDay(*(w.days + day), day);
            }
            break;
        }
        case 4: 
        {
            printWeek(w);
            break;
        }
        case 5: 
        {
            int fromDay, fromSlot, toDay;
            cin >> fromDay >> fromSlot >> toDay;

            if (index != nullptr) destroyIndex(index, indexSize);

            if (moveAppointment(w, fromDay, fromSlot, toDay))
            {
                printf("OK MOVED d%d s%d -> d%d\n", fromDay, fromSlot, toDay);
            }
            break;
        }
        case 6: 
        {
            if (index != nullptr) destroyIndex(index, indexSize);
            index = buildIndex(w, indexSize);
            printf("OK INDEX_BUILT size=%d\n", indexSize);
            break;
        }
        case 7:
        {
            printIndex(index, indexSize);
            break;
        }
        case 8: 
        {
            sortIndexByPrice(index, indexSize);
            printf("OK INDEX_SORTED\n");
            break;
        }
        case 9:
        {
            destroyIndex(index, indexSize);
            printf("OK INDEX_DROPPED\n");
            break;
        }
        case 10: 
        {
            int id;
            cin >> id;
            int outDay, outSlot;
            Appointment* a = findAppointment(w, id, outDay, outSlot);

            if (a == nullptr)
            {
                printf("ERR NOT_FOUND\n");
            }
            else
            {
                printf("OK FOUND %04d day=%d slot=%d %-8s PKR %8.2f\n",
                    id, outDay, outSlot, a->service, a->price);
            }
            break;
        }
        case 11: 
        {
            if (index != nullptr) destroyIndex(index, indexSize);
            loadSeedWeek(w);
            printf("OK SEED_LOADED\n");
            break;
        }
        case 12: 
        {
            int day, id;
            char name[64], service[64];
            float price;
            cin >> day >> id;
            cin.width(64); cin >> name;
            cin.width(64); cin >> service;
            cin >> price;

            if (day < 0 || day >= DAYS_IN_WEEK)
            {
                printf("ERR BAD_DAY\n");
                break;
            }
            if (w.days == nullptr)
            {
                printf("ERR WEEK_DESTROYED\n");
                break;
            }

            DaySchedule dayCopy = *(w.days + day);
            printf("BYVAL pre count=%d capacity=%d\n", dayCopy.count, dayCopy.capacity);
            bool result = bookByValue(dayCopy, id, name, service, price);
            printf("BYVAL returned=%d\n", result ? 1 : 0);
            printf("BYVAL post count=%d capacity=%d\n", dayCopy.count, dayCopy.capacity);
            break;
        }
        case 13: 
        {
            if (index != nullptr) destroyIndex(index, indexSize);
            destroyWeek(w);
            printf("OK WEEK_DESTROYED\n");
            break;
        }
        case 14:
        {
            reportSizes();

            float list[10];
            fillPrices(list, list + 10, 10);

            printf("A2 LIST:");
            for (int i = 0; i < 10; i++) printf(" %.1f", list[i]);
            printf("\n");

            printf("A2 SUM=%.2f\n", sumRange(list, list + 10));

            float* mx = maxElementPtr(list, list + 10);
            printf("A2 MAX=%.1f OFF=%d\n", *mx, (int)(mx - list));

            printf("A2 ABOVE=%d\n", countAbove(list, list + 10, 1000.0f));

            reverseInPlace(list, list + 10);
            printf("A2 REV:");
            for (int i = 0; i < 10; i++) printf(" %.1f", list[i]);
            printf("\n");
            break;
        }
        default:
        {
            printf("ERR BAD_CMD\n");
            break;
        }
        }

#ifdef INTERACTIVE_MODE
        printf("\nPress Enter to continue...");
        cin.ignore(1000, '\n');
        cin.get();
        printf("\x1B[2J\x1B[H");
#endif
    }

    if (index != nullptr) destroyIndex(index, indexSize);
    destroyWeek(w);
    printf("BYE\n");

    return 0;
}