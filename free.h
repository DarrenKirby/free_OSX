/***************************************************************************
 *   free.h - includes and functions common to all files                   *
 *                                                                         *
 *   Copyright (C) 2015 by Darren Kirby                                    *
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


#ifndef _FREE_H_
#define _FREE_H_

/* Version information */
#define APPNAME    "free"
#define APPVERSION "0.1"

/* Common includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <errno.h>


struct Meminfo {
    /* core */
    long int mem_total;
    long int mem_used;
    long int mem_free;
    /* swap */
    long int swap_total;
    long int swap_used;
    long int swap_free;
} m_info;

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

int get_total_mem(void) {
    size_t size;
    long int buf;
    size = sizeof(long int);

    if (sysctlbyname("hw.memsize", &buf, &size, NULL, 0) != 0) {
        fprintf(stderr, "Could not collect VM info, errno %d - %s",
                errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    m_info.mem_total = buf;
    return 0;
}

int get_used_mem(void) {
    long int wired, active, inactive;
    FILE *fd;

    if ((fd = popen("vm_stat", "r")) == NULL) {
        fprintf(stderr, "popen failed, errno %d - %s",
                errno, strerror(errno));
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    /* first line is throwaway */
    linelen = getline(&line, &linecap, fd);
    long int value[3];

    for (int i = 0; i < 3; i++) {
        linelen = getline(&line, &linecap, fd);
        sscanf(line, "Pages %*s %ld.", &value[i]);
    }

    m_info.mem_used = (value[0] + value[1] + value[2]) * 4096;
    m_info.mem_free = (m_info.mem_total - m_info.mem_used);

    pclose(fd);
    return 0;
}

int show_help(void) {
    printf("Usage: %s [OPTION]...\n\n\
Options:\n \
    -b\t\tdisplay memory usage in bytes\n \
    -k\t\tdisplay memory usage in kilobytes\n \
    -m\t\tdisplay memory usage in megabytes\n \
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

#endif // #ifndef _FREE_H_
