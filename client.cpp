#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>


#define SERVER_PORT 6666
#define BUFFER_SIZE 1024
#define CLIENT_PORT 9988
#define CLIENT_DATA_PORT 8888
#define MAX_CONNECTION 10

void* thread_function(void* msg);


int main(int argc, char** argv){
  int client_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(client_sockfd < 0){
    printf("client socket create error!\n");
    return 1;
  }
  struct sockaddr_in client_address;
  memset(&client_address, sizeof(client_address),0);
  client_address.sin_family = AF_INET;
  client_address.sin_port = htons(CLIENT_PORT);
  client_address.sin_addr.s_addr = INADDR_ANY;
  if(bind(client_sockfd, (struct sockaddr*)&client_address, sizeof(client_address))){
    printf("bind client socket error!\n");
    return 1;
  }
  
  
  // data socket thread
  int res;
  pthread_t d_thread;
  void* thread_result;
  res = pthread_create(&d_thread, NULL, thread_function, (void*)NULL);
  if(res != 0){
    printf("data thread create error!\n");
  }else{
    printf("create data thread!\n");
  }




  struct sockaddr_in server_address;
  server_address.sin_len = sizeof(struct sockaddr_in);
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(SERVER_PORT);
  server_address.sin_addr.s_addr = inet_addr("10.108.16.239");
  memset(&(server_address.sin_zero), 8, 0);

  if(client_sockfd < 0){
    printf("create socket error!\n");
    return 1;
  }

  if(connect(client_sockfd, (struct sockaddr*)&server_address, sizeof(struct sockaddr_in)) < 0){
    printf("connect error: %d, %s\n", errno, strerror(errno));
    return 1;
  }

  char recv_msg[BUFFER_SIZE];
  char reply_msg[BUFFER_SIZE];
  while(1){
    memset(recv_msg, BUFFER_SIZE, 0);
    memset(reply_msg, BUFFER_SIZE, 0);

    //    long byte_num = recv(client_sockfd, recv_msg, BUFFER_SIZE, 0);
    //    recv_msg[byte_num] = 0;
    //    printf("server said: %s\n", recv_msg);

    printf("relpy:");
    scanf("%s", reply_msg);
    int send_res = send(client_sockfd, reply_msg, 1024, 0);
    if(send_res < 0){
      printf("send message error!\n");
      if(connect(client_sockfd, (struct sockaddr*)&server_address, sizeof(struct sockaddr_in)) < 0){
	printf("reconnect error: %d, %s\n", errno, strerror(errno));
	return 1;
      }else{
	printf("reconnect success, please type the command\n");
      }
    }
  }
}



void* thread_function(void* msg){
  int data_sockfd, server_sockfd;
  struct sockaddr_in data_address, server_address;

  data_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(data_sockfd < 0){
    printf("create socket error!\n");
    return NULL;
  }
  data_address.sin_family = AF_INET;
  data_address.sin_addr.s_addr = INADDR_ANY;
  data_address.sin_port = htons(CLIENT_DATA_PORT);

  int data_len = sizeof(data_address);
  if(bind(data_sockfd, (struct sockaddr*)&data_address, data_len) < 0){
    printf("bind socket error!\n");
    return NULL;
  }
  

  listen(data_sockfd, MAX_CONNECTION);

  int server_len = sizeof(server_address);
  server_sockfd = accept(data_sockfd, (struct sockaddr*)&server_address,(socklen_t *)&server_len); 
  if(server_sockfd < 0){
    printf("accept data error!\n");
    close(server_sockfd);
    pthread_exit((void*)"thread exit");
  }

  while(1){
    char msg[BUFFER_SIZE];
    memset(msg, BUFFER_SIZE, 0);
    if(recv(server_sockfd, msg, BUFFER_SIZE, 0) <= 0){
      printf("server connection error!\n");
      break;
    }
    printf("data from server: %s\n", msg);
  }
  close(server_sockfd);
  pthread_exit((void*)"thread exit");

}
