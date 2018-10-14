/* Simple program to demonstrate a UDP-based server.
 * Loops forever.
 * Receives a word from the client. 
 * Sends back the length of that word to the client.
 * 
 * Compile using "cc -o wordlenserver wordlenserver.c"
 */
 
/* Include files */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
/* Manifest constants */
#define MAX_BUFFER_SIZE 1000
#define PORT 12345

/* Verbose debugging */
#define DEBUG
#define SEQINDEX 0
#define WORDSUM 1
#define MYHASH 0
/* Main program */
int main()
  {
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;
    int s;
    unsigned int len=sizeof(si_server);
    char messagein[MAX_BUFFER_SIZE];
    char messageout[MAX_BUFFER_SIZE];
    #if SEQINDEX 
    char definitionStrings[MAX_BUFFER_SIZE][MAX_BUFFER_SIZE];
    char definitionHex[MAX_BUFFER_SIZE][MAX_BUFFER_SIZE];
    int numOfDefinitions =0;
    #endif
    int readBytes;

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    {
    	printf("Could not setup a socket!\n");
    	return 1;
    }
    memset((char *) &si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(PORT);
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *) &si_server;
    client = (struct sockaddr *) &si_client;

    if (bind(s, server, sizeof(si_server))==-1)
    {
    	printf("Could not bind to port %d!\n", PORT);
    	return 1;
    }
    printf("wordlenserver now listening on port %d...\n", PORT);
	
    /* big loop, looking for incoming messages from clients */
    while(true)
    {
      	/* clear out message buffers to be safe */
      	bzero(messagein, MAX_BUFFER_SIZE);
      	bzero(messageout, MAX_BUFFER_SIZE);

      	/* see what comes in from a client, if anything */
      	if ((readBytes=recvfrom(s, messagein, MAX_BUFFER_SIZE, 0, client, &len)) < 0)
      	  {
      	    printf("Read error!\n");
      	    return -1;
      	  }
        printf("wordlenserver received word \"%s\" from client %s on port %d\n",
             messagein, inet_ntoa(si_client.sin_addr), ntohs(si_client.sin_port));
        //Implement hashing
        //Check if hex//
        //NOT hex//
        char * nullTerminator = strchr(messagein, '\0');
        *nullTerminator = ' ';
        char * currentWord = strtok(messagein, " ");
        while(currentWord != NULL){
          #ifdef DEBUG
            printf("%s\n", currentWord);
          #endif
          char punctuation[3] =" "; 
          char * hashTo = NULL;
          if(ispunct(currentWord[strlen(currentWord)-1]))
          {
            sprintf(punctuation, "%c ", currentWord[strlen(currentWord)-1]);
            currentWord[strlen(currentWord)-1] = '\0';
          }
          if(strncmp(currentWord, "0x", 2))
          {
            //Sequential Index
            #if SEQINDEX
            for(int t =0; t<numOfDefinitions; t++)
            {
              //Check if its in there
              if(!strcmp(definitionStrings[t], currentWord))
              {
                hashTo = definitionHex[t];
                break;
              }
            }
            if(hashTo == NULL) //Then hash the word
            {
              strcpy(definitionStrings[numOfDefinitions],currentWord);
              sprintf(definitionHex[numOfDefinitions], "0x%x", numOfDefinitions+1);
              hashTo = definitionHex[numOfDefinitions++];
            }
            sprintf(messageout +strlen(messageout), "%s%s", hashTo, punctuation);
             //Word Sum
            #elif WORDSUM
            //grab the word and create a hash 
            int hashNum =0;
            for(unsigned int t = 0; t<strlen(currentWord); t++)
            {
              hashNum += currentWord[t];
            }
            sprintf(messageout +strlen(messageout), "0x%x%s", hashNum, punctuation);
            //Your Hash
            #elif MYHASH
            #endif
          }
          else
          {
            //Sequential Index
            hashTo = "(Invalid)";
            #if SEQINDEX
            for(int t =0; t<numOfDefinitions; t++)
            {
              //Check if its in there
              if(!strcmp(definitionHex[t], currentWord))
              {
                hashTo = definitionStrings[t];
                break;
              }
            }
            //Word Sum
            #elif WORDSUM
            //Your Hash
            #elif MYHASH
            #endif
            sprintf(messageout +strlen(messageout), "%s%s", hashTo, punctuation);
          }
          currentWord = strtok(NULL, " ");
        }
        *strrchr(messageout, ' ') = '\0';
        #ifdef DEBUG
        	printf("Server sending message: %s\n", messageout);
        #endif

      	/* send the result message back to the client */
      	sendto(s, messageout, strlen(messageout), 0, client, len);		
    }

    close(s);
    return 0;
  }