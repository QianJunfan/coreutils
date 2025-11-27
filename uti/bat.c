#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
static int get_mac_battery(void)
{
        FILE *fp;
        char line[80];
        char *ptr;
        int capacity = -1;

        fp = popen("pmset -g batt | grep -oE '[0-9]+%'", "r");
        if (!fp)
                return -1;

        if (fgets(line, sizeof(line), fp) != NULL) {
                ptr = line;
                while (*ptr) {
                        if (*ptr == '%') {
                                *ptr = '\0';
                                break;
                        }
                        ptr++;
                }
                capacity = atoi(line);
        }

        pclose(fp);

        return capacity;
}
#endif

#ifndef __APPLE__
#define BATTERY_PATH "/sys/class/power_supply/BAT0/capacity"

static int get_linux_battery(void)
{
        FILE *fp;
        int capacity = -1;
        char buffer[10];

        fp = fopen(BATTERY_PATH, "r");
        if (!fp)
                return -1;

        if (fgets(buffer, sizeof(buffer), fp) != NULL)
                capacity = atoi(buffer);

        fclose(fp);

        return capacity;
}
#endif

int main(void)
{
        int capacity = -1;

#ifdef __APPLE__
        capacity = get_mac_battery();
#else
        capacity = get_linux_battery();
#endif

        if (capacity >= 0)
                printf("%d\n", capacity);
        else
                fprintf(stderr, "Error: Could not get battery level.\n");

        return capacity >= 0 ? 0 : 1;
}
