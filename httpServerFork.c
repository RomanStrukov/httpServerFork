#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

struct {
	char *ext;
	char *conttype;
} extensions[] = {
	{".txt", "text/html"},
	{".htm", "text/html"},
	{".html", "text/html"},
	{".jpg", "text/jpeg"},
	{".jpeg", "text/jpg"},
	{".png", "image/png"},
	{".ico", "image/ico"},
	{".css", "text/css"},
	{".js", "text/javascript"},
	{".php", "text/php"},
	{".xml", "text/xml"},
	{".pdf", "application/pdf"},
	{0, 0}	
};

void headers(int client, int size, int httpcode, char* content_type) {
	char buf[1024];
	char strsize[20];
	sprintf(strsize, "%d", size);
	if (httpcode == 200) {
		strcpy(buf, "HTTP/1.0 200 OK\r\n");
	}
	else if (httpcode == 404) {
		strcpy(buf, "HTTP/1.0 404 Not Found\r\n");
	}
	else {
		strcpy(buf, "HTTP/1.0 500 Internal Server Error\r\n");
	}
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "Connection: keep-alive\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "Content-length: ");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, strsize);
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "simple-server");
	send(client, buf, strlen(buf), 0);
	if(content_type != null)
	{
	  sprintf(buf, "Content-Type:%s\r\n", content_type);
	  send(client, buf, strlen(buf), 0);
	  strcpy(buf, "\r\n");
	  send(client, buf, strlen(buf), 0);
	}	
}


void parseFileName(char *line, char **filepath, size_t *len) {
	char *start = NULL;
	while ((*line) != '/') line++;
	start = line + 1;
	while ((*line) != ' ') line++;
	(*len) = line - start;
	*filepath = (char*)malloc(*len + 1);
	*filepath = strncpy(*filepath, start, *len);
	(*filepath)[*len] = '\0';
	printf("%s \n", *filepath);
}

char* getExtension(char* fileName)
{
  return strrchr(fileName,'.');
}

int main() {
	int my_socket = 0;
	int res = 0;
	int cd = 0;
	int filesize = 0;
	int numberOfChildren = 0;
	pid_t chpid;
	pid_t *chPids = NULL;
	const int backlog = 10;
	struct sockaddr_in saddr;
	struct sockaddr_in caddr;
	char *line = NULL;
	size_t len = 0;
	char *filepath = NULL;
	size_t filepath_len = 0;
	int empty_str_count = 0;
	socklen_t size_saddr;
	socklen_t size_caddr;
	FILE *fd;
	FILE *file;

	my_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (my_socket == -1) {
		printf("listener create error \n");
	}
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8080);
	saddr.sin_addr.s_addr = INADDR_ANY;
	res = bind(my_socket, (struct sockaddr *)&saddr, sizeof(saddr));
	if (res == -1) {
		printf("bind error \n");
	}
	res = listen(my_socket, backlog);
	if (res == -1) {
		printf("listen error \n");
	}
	chpid = fork();
	if(chpid < 0)
	{
		printf("fork error");
		continue;
	}
	if(chpid == 0)
	{
		while (1) 
		{		
			cd = accept(my_socket, (struct sockaddr *)&caddr, &size_caddr);
			if (cd == -1) 
			{
				printf("accept error \n");
				continue;
			}		
				printf("client is in %d descriptor. Client's address is %d \n", cd, caddr.sin_addr.s_addr);
				fd = fdopen(cd, "r");
				if (fd == NULL) 
				{
					printf("error open client descriptor as file \n");
				}
			while ((res = getline(&line, &len, fd)) != -1) 
			{
				if (strstr(line, "GET")) 
				{
					parseFileName(line, &filepath, &filepath_len);
				}
				if (strcmp(line, "\r\n") == 0) 
				{
					empty_str_count++;
				}
				else 
				{
					empty_str_count = 0;
				}
				if (empty_str_count == 1) 
				{
					break;
				}
				printf("%s", line);
			}
			printf("open %s \n", filepath);
			file = fopen(filepath, "r");
			if (file == NULL) 
			{
				printf("404 File Not Found \n");
				headers(cd, 0, 404);
			}
			else 
			{
				char *fileExt = getExtension(filepath);
				char *content_type = 0;
				int i = 0;
				while (extensions[i].ext != 0) 
				{
					if (strcmp(extensions[i].ext, fileExt) == 0) 
					{
						int n = strlen(extensions[i].conttype);
						content_type = (char*) malloc(n * sizeof(char));
						strncpy(content_type, extensions[i].conttype, n);
						break;
					}
					i++;
				}
				if (content_type != 0) 
				{
					fseek(file, 0L, SEEK_END);
					filesize = ftell(file);
					fseek(file, 0L, SEEK_SET);
					headers(cd, filesize, 200, content_type);
					while (getline(&line, &len, file) != -1) 
					{
						res = send(cd, line, len, 0);
						if (res == -1) 
						{
							printf("send error \n");
						}
					}
				}
			}		
						
		}
	}
	else 
	{
		//waitin' till all of the child procs finish
	   do {
			stillWaiting = 0;
			for (int i = 0; i < numberOfChildren; ++i) 
			{
			   if (childPids[i] > 0) 
			   {
				  if (waitpid(childPids[i], NULL, WNOHANG) != 0)
					{				 
					 childPids[i] = 0;
					}
				  else 
				    {				 
					   stillWaiting = 1;
				    }
			   }		  
			   //sleep(0);
			}
		} while (stillWaiting);
		free(chPids);
	}
	close(cd);
	return 0;
}
