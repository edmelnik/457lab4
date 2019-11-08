#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock.h>
#include <conio.h>
#include <direct.h>
#include "ntwk.h"

char text[80];
char ip_addr[40];
int port_numb = 2222;


void Recv(void);
void Xmit(void);



struct ntwk_stuff {
	int command;						// 0=open, 1=write, 2=close
	char filelocation[100];				//destination file location 
	char file[80];						//user specified file 
	unsigned char buffer[10 * 1024];	//buffer for the data
	int length;							//length of the file 
};

struct ntwk_stuff packet,packet2;





int main(int argc, char *argv[])
{
   /* same code for both transmit and receive */
   printf("Enter: T for transmit or R for receive\n");
   gets(text);

   if (text[0] == 'R' || text[0] == 'r')
       Recv();
   else
       Xmit();

   return(0);
}



void Xmit(void)
{
      int rc;
	  //int ret;
	 
	  char directory[100];
	 
	  							//user file input
	  FILE * filep;

      printf("Enter the destination IP address: ");
      gets(ip_addr);

     // rc = SendInit(ip_addr, port_numb);
     /* if (rc < 0)
      {
         printf("Network Error %d\n",rc);
         exit(1);
      }*/

	  printf("Copy a file over the network connection\r\n");
	  printf("Enter the exact file name\r\n");
	  gets(packet.file);

	  filep = fopen(packet.file, "rb");						//open file
	  if (filep == (FILE *)NULL)						//make sure the file can be opened
	  {													//print error if it cannot open
		  printf("The file [%s] could not be opened\nfrom directory: %s\n",
			  packet.file, getcwd(directory,sizeof(directory)));
		  exit(1);
	  }
	  packet.command = 0;
	  rc = NtwkSend(sizeof(packet), (unsigned char*)&packet);
	  if (rc < 0)
		  break;
	  rc = NtwkRecv(sizeof(packet), (unsigned char*)&packet2);
	  if (rc < 0)
		  break;


	  while (1)
	  {
		  packet.length = fread(packet.buffer, sizeof(packet.buffer), sizeof(char), filep);		//read the file
		  if (packet.length <= 0)
			  break;

		  packet.command = 1;
		  rc = NtwkSend(sizeof(packet), (unsigned char*)&packet);
		  if (rc < 0)
			  break;
		  rc = NtwkRecv(sizeof(packet), (unsigned char*)&packet2);
		  if (rc < 0)
			  break;
		  // here is where you send the data and wait for an acknowledgement
	  }
	  packet.command = 2;
	  rc = NtwkSend(sizeof(packet), (unsigned char*)&packet);
	  if (rc < 0)
		  break;
	  rc = NtwkRecv(sizeof(packet), (unsigned char*)&packet2);
	  if (rc < 0)
		  break;

	  fclose(filep);												//close the file 




}










void Recv(void)
{
	int rc;

	printf("Initializing incoming network: (port %d)\n", port_numb);
	rc = RecvInit(port_numb);
	if (rc < 0)
	{
		printf("Network Error %d\n", rc);
		exit(1);
	}

	/* wait for incoming connection */
	rc = NtwkWait();
	if (rc < 0)
	{
		printf("Network Error %d\n", rc);
		exit(1);
	}

	while (1)
	{
		/* waits for incoming network message */
		rc = NtwkRecv(sizeof(text), text);
		if (rc < 0)
			break;

		printf("I got the text message:\n%s\n", text);

		printf("Enter your reply message:\n");
		gets(text);
		if (text[0] == '\0')
			break;

		rc = NtwkSend(sizeof(packet), (unsigned char *)&packet);
		if (rc < 0)
			break;
	}

	NtwkExit();
}
