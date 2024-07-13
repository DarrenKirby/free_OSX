/***************************************************************************
 *   free.c - free for OS X                                                *
 *                                                                         *
 *   Copyright (C) 2015 - 2024 by Darren Kirby                             *
 *   bulliver@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <stdint.h>


#define APPNAME    "free"
#define APPVERSION "0.8"


struct Meminfo {
    int64_t mem_total;
    int64_t mem_used;
    int64_t mem_free;

    int64_t swap_total;
    int64_t swap_used;
    int64_t swap_free;
} m_info;


int get_total_mem(void) {
    size_t size;
    int64_t buf;
    size = sizeof(int64_t);

    if (sysctlbyname("hw.memsize", &buf, &size, NULL, 0) != 0) {
        fprintf(stderr, "Could not collect VM info, errno %d - %s",
                errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    m_info.mem_total = buf;
    return 0;
}


int get_used_mem(void) {
    FILE *fd;

    if ((fd = popen("vm_stat", "r")) == NULL) {
        fprintf(stderr, "popen failed, errno %d - %s",
                errno, strerror(errno));
    }

    char *line = NULL;
    size_t linecap = 0;

    /* first line is throwaway */
    getline(&line, &linecap, fd);
    int64_t value[3];

    for (int i = 0; i < 3; i++) {
        getline(&line, &linecap, fd);
        sscanf(line, "Pages %*s %lld.", &value[i]);
    }

    // result returned in pages; page size is 4096 bytes
    m_info.mem_used = (value[0] + value[1] + value[2]) * 4096;
    m_info.mem_free = (m_info.mem_total - m_info.mem_used);

    pclose(fd);
    return 0;
}


int get_swap(void) {
    struct xsw_usage vmusage;
    size_t size = sizeof(vmusage);

    if (sysctlbyname("vm.swapusage", &vmusage, &size, NULL, 0) != 0) {
        fprintf(stderr, "Could not collect VM info, errno %d - %s",
                errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    m_info.swap_total = vmusage.xsu_total;
    m_info.swap_used  = vmusage.xsu_used;
    m_info.swap_free  = vmusage.xsu_avail;

    return 0;
}


void fmt(char base, int64_t bytes, char* output, size_t outputSize) {
    if (base == 'g') {
        snprintf(output, outputSize, "%.02lf GB", (double)bytes / (1024 * 1024 * 1024));
    } else if (base == 'm') {
        snprintf(output, outputSize, "%lld MB", bytes / (1024 * 1024));
    } else if (base == 'k') {
        snprintf(output, outputSize, "%lld KB", bytes / 1024);
    } else {
        snprintf(output, outputSize, "%lld B", bytes);
    }
}


int get_free(char base) {
    char mem_total[30], mem_used[30], mem_free[30],
         swap_total[30], swap_used[30], swap_free[30];

    if (get_total_mem() != 0)
        printf("Could not get total memory\n");
    if (get_used_mem() != 0)
        printf("Could not obtain used memory\n");
    if (get_swap() != 0)
        printf("Could not obtain swap memory\n");

    fmt(base, m_info.mem_total, mem_total, sizeof(mem_total));
    fmt(base, m_info.mem_used, mem_used, sizeof(mem_used));
    fmt(base, m_info.mem_free, mem_free, sizeof(mem_free));
    fmt(base, m_info.swap_total, swap_total, sizeof(swap_total));
    fmt(base, m_info.swap_used, swap_used, sizeof(swap_used));
    fmt(base, m_info.swap_free, swap_free, sizeof(swap_free));

    printf("\t%11s\t%11s\t%11s\n", "Total", "Used", "Free");
    printf("Mem:\t%14s\t%14s\t%14s\n", mem_total, mem_used, mem_free);
    printf("Swap:\t%14s\t%14s\t%14s\n", swap_total, swap_used, swap_free);

    return 0;
}


int show_help(void) {
    printf("Usage: %s [OPTION]...\n\n\
Options:\n \
    -b\t\tdisplay memory usage in bytes\n \
    -k\t\tdisplay memory usage in kilobytes\n \
    -m\t\tdisplay memory usage in megabytes\n \
    -g\t\tdisplay memory usage in gigabytes\n \
    -s [delay]\tdisplay updated usage every [delay] seconds\n \
    -h\t\tdisplay this help\n \
    -V\tdisplay version information\n\n \
Report bugs to <bulliver@gmail.com>\n", APPNAME);
    return 0;
}


int show_version(void) {
    printf("OS X %s version %s\n", APPNAME, APPVERSION);
    return 0;
}


void signal_handler(int signal) {
    printf("Caught SIGINT (signal %d). Exiting...\n", signal);
    exit(0);
}


int main(int argc, char *argv[]) {
    int ch;
    int polling = 0;
    int poll_freq;
    char base;

    signal(SIGINT, signal_handler);

    while ((ch = getopt(argc, argv, "hVbgkms:")) != -1) {
        switch (ch) {
            case 'b':
                base = 'b';
                break;
            case 'k':
                base = 'k';
                break;
            case 'm':
                base = 'm';
                break;
            case 'g':
                base = 'g';
                break;
            case 's':
                polling = 1;
                poll_freq = atoi(optarg);
                break;
            case 'h':
                show_help();
                exit(EXIT_SUCCESS);
                break;
            case 'V':
                show_version();
                exit(EXIT_SUCCESS);
                break;
            case '?':
            default:
                show_help();
                exit(EXIT_FAILURE);
        }
    }

    if (polling == 1) {
        for (;;) {
            if (get_free(base) != 0)
                exit(EXIT_FAILURE);
            sleep(poll_freq);
            printf("\n");
        }
    } else {
        if (get_free(base) != 0)
            exit(EXIT_FAILURE);
        exit(EXIT_SUCCESS);
    }

    return 0;
}
