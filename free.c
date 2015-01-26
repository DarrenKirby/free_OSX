/***************************************************************************
 *   free.c - free for OS X                                                *
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

#include "free.h"


long int fmt(char base, long int n) {
    if (base == 'm')
        return n / 1024 / 1024;
    else if (base == 'k')
        return n / 1024;
    else
        return n;
}

int get_free(char base) {
    get_total_mem();
    get_used_mem();
    get_swap();

    printf("\t%10s\t%10s\t%10s\n", "Total", "Used", "Free");
    printf("Mem:\t%10ld\t%10ld\t%10ld\n", fmt(base, m_info.mem_total),fmt(base, m_info.mem_used),fmt(base, m_info.mem_free));
    printf("Swap:\t%10ld\t%10ld\t%10ld\n", fmt(base, m_info.swap_total),fmt(base, m_info.swap_used),fmt(base, m_info.swap_free));

    return 0;
}

int main(int argc, char *argv[]) {
    int ch;
    int polling = 0;
    int poll_freq;
    char base;

    while ((ch = getopt(argc, argv, "hVbkms:")) != -1) {
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
            case 's':
                polling = 1;
                poll_freq = atoi(optarg);
                break;
            case 'h':
                show_help();
                exit(0);
                break;
            case 'V':
                show_version();
                exit(0);
                break;
            case '?':
            default:
                show_help();
                exit(-1);
        }
    }

    if (polling == 1) {
        for (;;) {
            get_free(base);
            sleep(poll_freq);
            printf("\n");
        }
    } else {
        get_free(base);
        exit(EXIT_SUCCESS);
    }

    return 0;
}
