#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <stdbool.h>

#define BUFFERSIZE 1024
#define COPYMORE 0644

int copyF2F( char*, char* );
bool isdir( char* );
char* strrev( char* );

int main( int argc, char *argv[] ) {
    bool opt_r = false;
    bool opt_l = false;
    bool opt_s = false;

    char *src = NULL;
    char *dest = NULL;

    char c;

    while( ( c = getopt( argc, argv, "rRls" ) ) != -1 ) {
        switch( c ) {
            case 'R':
            case 'r':
                opt_r = true;
                break;

            case 'l':
                opt_l = true;
                break;

            case 's':
                opt_s = true;
                break;
        }
    }

    if( optind >= argc - 1 ) {
        printf( "缺少操作符\n" );
        exit( -1 );
    }

    src = argv[optind];
    dest = argv[optind + 1];

    if( opt_l ) {
        if( isdir( src ) ) {
            printf( "目录不能创建硬链接\n" );
            exit( -1 );
        }
        if( ( link( src, dest ) ) == 0 ) {
            return 0;
        }
        else {
            printf( "创建硬链接失败\n" );
            exit( -1 );
        }
    }

    if( opt_s ) {
        if( isdir( src ) ) {
            printf( "目录不能创建符号链接\n" );
            exit( -1 );
        }
        if( ( symlink( src, dest ) ) == 0 ) {
            return 0;
        }
        else {
            printf( "创建符号链接失败\n" );
            exit( -1 );
        }
    }

    if( !isdir( src ) ) {
        if( ( copyF2F( src, dest ) ) == 0 ) {
            return 0;
        }
        else {
            printf( "复制文件失败\n" );
            exit( -1 );
        }
    }
    else if( isdir( src ) ) {
        if( !isdir( dest ) ) {
            printf( "不能将一个目录复制到一个文件\n" );
            exit( -1 );
        }
        else if( isdir( dest ) && opt_r ) {
            if( copyD2D( src, dest ) != 0 ) {
                printf( "目录拷贝失败\n" );
                exit( -1 );
            }
            else {
                return 0;
            }
        }
        else {
            printf( "拷贝目录需要用-r选项\n" );
            exit( -1 );
        }
    }
    else {
        printf( "该操作不合法\n" );
        exit( -1 );
    }

    return 0;
}

int copyD2D( char *src_dir, char *dest_dir ) {
    DIR *dp = NULL;
    struct dirent *dirp;
    char tempDest[256];
    char tempSrc[256];
    strcpy( tempDest, dest_dir );
    strcpy( tempSrc, src_dir );

    if( ( dp = opendir( src_dir ) ) == NULL ) {
        return 1;
    }
    else {
        while( ( dirp = readdir( dp ) ) ) {
            struct stat file_stat;
            if( !isdir( dirp->d_name ) ) {
                strcat( tempDest, dirp->d_name );
                strcat( tempSrc, dirp->d_name );

                copyF2F( tempSrc, tempDest );

                strcpy( tempDest, dest_dir );
                strcpy( tempSrc, src_dir );
            }
        }

        closedir( dp );
        return 0;
    }
}

bool isdir( char *filename ) {
    struct stat fileInfo;

    if( stat( filename, &fileInfo ) >= 0 ) {
        if( S_ISDIR( fileInfo.st_mode ) ) {
            return true;
        }
        else {
            return false;
        }
    }
}

int copyF2F( char *src_file, char *dest_file ) {
    int in_fd, out_fd, n_chars;
    char buf[BUFFERSIZE];

    if( isdir( dest_file ) ) {
        char c;
        char temp[10] = { '\0' };
        char *r_temp;
        int n = strlen( src_file );
        int m = 0;

        while( ( c = src_file[n - 1] ) != '/' ) {
            temp[m] = c;
            m++;
            n--;
        }
        r_temp = strrev( temp );
        strcat( dest_file, r_temp );
    }

    if( ( in_fd = open( src_file, O_RDONLY ) ) == -1 ) {
        printf( "%s文件读取失败！", src_file );
        return 1;
    }

    if( ( out_fd = open( dest_file, O_WRONLY | O_CREAT, COPYMORE ) ) == -1 ) {
        return 1;
    }

    while( ( n_chars = read( in_fd, buf, BUFFERSIZE ) ) > 0 ) {
        if( write( out_fd, buf, n_chars ) != n_chars ) {
            printf( "%s文件写失败！", dest_file );
            return 1;
        }


        if ( n_chars == -1 ) {
            printf( "%s文件读取失败！", src_file );
            return 1;
        }
    }

    if( close( in_fd ) == -1 || close( out_fd ) == -1 ) {
        printf( "文件关闭失败！" );
        return 1;
    }

    return 0;
}

char* strrev( char *str ) {
    int i = strlen( str ) - 1;
    int j = 0;

    char ch;
    while( i > j ) {
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
        i--;
        j++;
    }

    return str;
}
