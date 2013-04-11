/*
 * =====================================================================================
 *
 *       Filename:  hw1.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年03月17日 22時32分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  grapefruit623 (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>

#define	BUFFSIZE 4096			/*  */
struct {
    char *ext;
    char *filetype;
} fileExtensions [] = {
    {"gif", "image/gif" },
    {"jpg", "image/jpeg"},
    {"jpeg","image/jpeg"},
    {"png", "image/png" },
    {"ico", "image/x-icon" },
    {"zip", "image/zip" },
    {"gz",  "image/gz"  },
    {"tar", "image/tar" },
    {"htm", "text/html" },
    {"html","text/html" },
    {"css","text/css" },
    {"js","application/x-javascript"},
    {"bmp","application/x-bmp"},
    {"exe","text/plain" },
    {"7z","text/plain" },
    {"txt","text/plain" },
    {"iso","application/x-iso9660-image" },
    {0,0} };


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  judgeFileType
 *  Description:  to check the file extensions, jpg, gif...etc
 * =====================================================================================
 */
		char *
judgeFileType ( char *argu )
{
		int i = 0;
		while ( 0 != fileExtensions[i].ext ) {
				
				if ( 0 == strcmp(argu, fileExtensions[i].ext ) ) { /* to match to file type */
//						printf ( "%s\n", fileExtensions[i].filetype  );
						return fileExtensions[i].filetype; 
				}
				i++;
		}
		return ;
}		/* -----  end of function judgeFileType  ----- */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Reply
 *  Description:  checking does the requested file exist, and write to a buffer to reply
 *  				to socket
 * =====================================================================================
 */
		void
Reply ( int acceptId , char *file, char *ip)
{
		int fd, log, ret;
		char buffer[BUFFSIZE+1];
		char logFile[BUFFSIZE];
		char timeBuf[BUFFSIZE];
		char *fileType;
		time_t timep;
		struct tm *p;

		if ( 0 > (fd = open(file, O_RDONLY)) ) {
			fprintf(stderr, "open file io fail");
		}
		else {
//				printf ( "\nfile: %s\n", file );
				strtok(file, ".");
				fileType = strtok(NULL, ".");
				fileType = judgeFileType( fileType ); /* get type of file */

				sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", fileType );
				write( acceptId, buffer, strlen(buffer)  ); /* to reply the requested header */
				
				while ( 0 < (ret = read(fd, buffer, BUFFSIZE)) ) /* to reply file */
						write(acceptId, buffer, ret);

				time( &timep );
				p = gmtime(&timep);             /* current time */
				sprintf(logFile, "log/%d-%d-%d.txt",1900+p->tm_year, 1+p->tm_mon, p->tm_mday); /* time format */
				if (  log = open( logFile, O_CREAT | O_APPEND | O_WRONLY )  ) { /* to record the request to log */
						write( log, ip, strlen(ip));
						write( log, "\n", strlen("\n") );
						sprintf( timeBuf, "%s", ctime(&timep) );
						write( log, timeBuf, strlen(timeBuf) );
						write( log, buffer, strlen(buffer));
				}
		}
		
		return ;
}		/* -----  end of function Reply  ----- */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ParseRequest
 *  Description:  judging to return index.html or other file 
 * =====================================================================================
 */
		void
ParseRequest ( int acceptId, char *ip )
{
		char buffer[BUFFSIZE+1];
		char index[BUFFSIZE];
		char *fileName;
		int ret;
		int i = 0, j = 0;
		ret = recv(acceptId, buffer, BUFFSIZE, 0);

		if (  0 == strncmp("GET", buffer, 3 ) ) {
				for ( i = 4; i < ret; i++ )
						if ( ' ' == buffer[i] )
								buffer[i] = '\0';
				if ( '/' == buffer[4] ) { 
						if ( '\0' == buffer[5] ) {      /* GET / */
								strcpy(index, "index.html");
								Reply(acceptId, index, ip);
						}
						else {                          /* another file */
								strtok(buffer, "/");
								fileName = strtok(NULL, "/");
		//						printf ( "\nfileName: %s\n", fileName );
								Reply(acceptId, fileName, ip);
						}
				}
		}
		else {
				if ( 0 == strncmp("POST", buffer, 4) ) { /* simple post request */
						i = strlen(buffer);
						
						while ( '=' != buffer[i] ) { /* to get the input name */
								i--;
						}
						for ( i = i+1 ; i < strlen(buffer) ;i++ ) { /* to concat chars to a string */
								index[j++] =  buffer[i];
						}
						index[j] = '\0';
						sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", "text/html" );
						write( acceptId, buffer, strlen(buffer)  ); /* to reply the requested header */
						sprintf(buffer, "<html> <body>  hello! <h2> %s </h2>  </body> </html>", index);
						write( acceptId, buffer, strlen(buffer));
				}
		}
		return ;
}		/* -----  end of function ParseRequest  ----- */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sigFork
 *  Description:  to avoid zombile 
 * =====================================================================================
 */
		void
sigFork ( int sig )
{
		pid_t pid;
		int stat;
		pid = waitpid(-1, &stat, 0);
//		printf("pid_t: %d , stat: %d\n", pid, stat);
		return ;
}		/* -----  end of function sigFork  ----- */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
		int
main ( int argc, char *argv[] )
{
		struct sockaddr_in myAddr;
		struct sockaddr_in clientInfo;
		socklen_t len = sizeof(clientInfo);
		int socketId = socket(AF_INET, SOCK_STREAM, 0);
		int bindId;
		int port = 8051;
		int acceptId;
		int forkId;
		char endBuf[] = "\n</body>\n </html>";
		char ip[64];


		printf ( "The server is running\n" );

		signal(SIGCHLD, sigFork);               /* to avoid zombile */

		if ( !socketId ) {
				fprintf(stderr, "socket failed");
		}
		
		bzero(&myAddr, sizeof(myAddr));
		myAddr.sin_family = AF_INET;
		myAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* any ip for the host */
		myAddr.sin_port = htons(port);

		bindId = bind( socketId, (struct sockaddr *)&myAddr, sizeof(myAddr));
		
		if ( 0 > bindId  ) {
				fprintf(stderr, "bind failed\n");
				return 0;
		}

		if ( 0 > listen( socketId, 10) ) {
				fprintf(stderr, "listening failed\n");
				return 0;
		}
		while ( 1 ) {
				acceptId = accept(socketId, (struct sockaddr *)&clientInfo, &len);
				strcpy(ip, inet_ntoa(clientInfo.sin_addr )); /* to get ipv4 */
				forkId = fork();
				if ( 0 > forkId ) {
						fprintf(stderr, "fork failed\n");
				}
				else {
						
						if ( 0 == forkId ) {    /* son */
								if ( -1 == acceptId ) {
										fprintf(stderr, "accept fail: %s\n", strerror(errno));
								}
								else {
//										fprintf(stdout, "\naccept success: %d\n", acceptId);	

										ParseRequest(acceptId, ip);
										close(socketId);
										close(acceptId);
										exit(1);
								}
						}
						else {                  /* father */
//								fprintf(stdout, "I am your father\n");
						}
				}

				close(acceptId);
		}
		close(socketId);	
		close(acceptId);
		return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
