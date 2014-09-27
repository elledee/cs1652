/**********************************************************************************
 *                                                                                *
 *                     Project 1: Web Client and Server                           *
 *                     Elizabeth Davis - ead59@pitt.ed                            *
 *                   Joesph Podnar - sasukechaos@gmail.c                          *
 *                              http_client.cc                                    *
 *                                                                                *
 **********************************************************************************/
 
#include <ctype.h>
#include <iostream>
#include "minet_socket.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h> 

#define BUFSIZE 1024

using namespace std;
int main(int argc, char * argv[]) 
{

  bool ok = false;
	char buff[BUFSIZE];
	char* server_name = NULL;
  char* server_path = NULL;
  char* req = NULL;
	fd_set fl_dscrptr;
  int server_port = -1;
	int res;
	int ret = 0;
	int s_fd; 
	string r = "";
	string h = "";
	string s;
	string::size_type position;
	struct hostent *hp;
	struct sockaddr_in *s_ad;

  /*parse args */
  if (argc != 5) 
	{
	    fprintf(stderr, "usage: http_client k|u server port path\n");
		  exit(-1);
  }

  server_name = argv[2];
  server_port = atoi(argv[3]);
  server_path = argv[4];

  req = (char *)malloc(strlen("GET  HTTP/1.0\r\n\r\n") 
			 + strlen(server_path) + 1);  

  /* initialize */
  if (toupper(*(argv[1])) == 'K') 
	{ 
	    minet_init(MINET_KERNEL);
  } 
	else if (toupper(*(argv[1])) == 'U') 
	{ 
	    //minet_init(MINET_USER);
		  fprintf(stderr, "Currently cannot run using the user stack\n" 
		          "Please enter k as the first argument to run using the kernel stack\n");
		  free(req);
	    exit(-1);
  } 
	else 
	{
	    fprintf(stderr, "First argument must be k or u\n");
	  	free(req);
	    exit(-1);
  }

  /* make socket */
	s_fd = minet_socket(SOCK_STREAM);
	if (s_fd != -1) 
	{
	   	/* get host IP address  */
      /* Hint: use gethostbyname() */
	    hp = gethostbyname(server_name);
	    if (hp != 0) 
	    {
          /* set address */
	        s_ad.sin_family = AF_INET;
	        memcpy(&s_ad.sin_addr.s_addr, hp->h_addr, hp->h_length);
	
	        s_ad.sin_port = htons(server_port);
	
          /* connect to the server socket */
	        if (minet_connect(s_fd,&s_ad) >= 0) 
	        {
			        /* send request message */
			        sprintf(req, "GET %s HTTP/1.0\r\n\r\n", server_path);
	            if (minet_write(s_fd, req, strlen(req)) >= 0) 
	            { 
                  /* wait till socket can be read. */
                  /* Hint: use select(), and ignore timeout for now. */
	
	                FD_ZERO(&fl_dscrptr); 
	                FD_SET(s_fd, &fl_dscrptr); 
	
	                if (minet_select(s_fd+1, &fl_dscrptr, NULL, NULL, NULL) >= 0) 
	                { 
                      /* first read loop -- read headers */
	                    res = minet_read(s_fd, buff, BUFSIZE-1);
	
						          while (res > 0) 
						          {
						              buff[res] = '\0';
						              r += string(buff);
						              position = r.find("\r\n\r\n",0);
			
						              if (position != string::npos) 
						              { 
						                  h = r.substr(0, position);
						                  r = r.substr(position+4);
						                  break;
						              }
			
						              res = minet_read(s_fd, buff, BUFSIZE-1);
						          }

						          /* examine return code */
						          s = h.substr(h.find(" ") + 1);
						          s = s.substr(0, s.find(" "));
		

						          //Skip "HTTP/1.0"
						          //remove the '\0'

						          // Normal reply has return code 200
						          /* print first part of response: header, error code, etc. */
						          if(s == "200") 
						          {
						              ok = true;
						              cout << r; 
						          }
						          else 
						          {
						              ok = false;
						              cerr << h + "\r\n\r\n" + r; 
						          }

						          /* second read loop -
						             - print out the rest of the response: real web content */
						          while ((res = minet_read(s_fd, buff, BUFSIZE-1)) > 0) 
						          {
						              buff[res] = '\0';
						              if(ok) 
						              {
							                printf("%s", buff);
						              } 
						              else 
						              {
							                fprintf(stderr, buff);
						              }
	                    }
						
						          /*close socket and deinitialize */
						          minet_close(s_fd);
						          minet_deinit();
	                }
					        else
					        {
					            fprintf(stderr, "Error selecting socket.\n");
		                  free(req);
                      minet_close(s_fd); // Close socket before exiting
                      minet_deinit();
						          ret = -1;
                  }
				      }
				      else
				      {
				          fprintf(stderr, "Error sending request to server.\n");
		              ret = -1;
	            }
		    	}
			    else
			    {
			        fprintf(stderr, "Error connecting to socket server.\n");
		          ret = -1;
			    }
		  }
		  else
		  { 
		      fprintf(stderr, "Error getting host IP address.\n");
		      ret = -1;
		  }
	}
	else
	{
	    fprintf(stderr, "Error creating the socket.\n");
		  ret = -1;
	}	
	
	free(req);
	
  if (ok) 
	{
	    ret = 0;
  } 
	else 
	{
	    ret = -1;
  }
	
	return ret;
}
