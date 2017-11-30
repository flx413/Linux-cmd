#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <utmp.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

bool opt_H = false;
bool opt_q = false;

static int mywho();

int main( int argc, char *argv[] ) {
    int c;
    int ok;

    // 暂时没做-b选项
    while( ( c = getopt( argc, argv, "Hqb" ) ) != -1 ) {
        switch( c ) {
            case 'H':
                opt_H = true;
                break;

            case 'q':
                opt_q = true;
                break;

            default:
                exit( -1 );
        }
    }

    // 命令行检错
    if( argc != optind ) {
        printf( "fault command!\n" );
    }

    ok = mywho();
    if( !ok ) {
        return 0;
    }
    else {
        exit( -1 );
    }

    return 0;
}

static int mywho() {
    struct utmp *um;
    char timebuf[24];
    int users = 0;

    if( opt_q ) {
        while( ( um = getutent() ) ) {
            if( um->ut_type != USER_PROCESS ) {
                continue;
            }
            printf( "%-2s", um->ut_user );
            users += 1;
        }
        printf( "\n# users=%d\n", users );
        endutent();
        return 0;
    }

    if( opt_H ) {
        printf( "%-12s%-12s%-20.20s %s\n", "NAME", "LINE", "TIME", "COMMENT" );
    }

    while( ( um = getutent() ) ) {
        if( um->ut_type != USER_PROCESS ) {
            continue;
        }
        time_t tm;
        tm = ( time_t )( um->ut_tv.tv_sec );
        strftime( timebuf, 24, "%F %R", localtime( &tm ) );
        printf( "%-12s%-12s%-20.20s (%s)\n", um->ut_user, um->ut_line, timebuf, um->ut_host );
    }
    endutent();

    return 0;
}
