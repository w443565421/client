#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <irrlicht.h>

#define SERVER_PORT 6666
#define BUFFER_SIZE 1024
#define BUFFER_SIZE_FILE 1024 * 1024
#define CLIENT_PORT 9988
#define CLIENT_DATA_PORT 8888
#define MAX_CONNECTION 10

using namespace irr;
using namespace gui;

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")
#endif

class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	MyEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

void* thread_function(void* msg);
int lock_set(int fd,int type);
int main(int argc, char** argv){

  video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
  MyEventReceiver receiver;
  IrrlichtDevice *device = createDevice(driverType, core::dimension2d<u32>(640, 480), 16, false, false, false, &receiver);
  if (device == 0)
    return 1;
  video::IVideoDriver* driver = device->getVideoDriver();
//scene::ISceneManager* smgr = device->getSceneManager();
//device->getCursorControl()->setVisible(false);


  int client_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(client_sockfd < 0){
    printf("client socket create error!\n");
    return 1;
  }
  struct sockaddr_in client_address;
//memset(&client_address, sizeof(client_address),0);
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
  res = pthread_create(&d_thread, NULL, thread_function, /*(void*)*/NULL);
  if(res != 0){
    printf("data thread create error!\n");
  }else{
    printf("create data thread!\n");
  }




  struct sockaddr_in server_address;
//server_address.sin_len = sizeof(struct sockaddr_in);
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(SERVER_PORT);
  server_address.sin_addr.s_addr = inet_addr("10.108.16.239");
//memset(&(server_address.sin_zero), 8, 0);

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
  int IsBegin = 0;
  while(1){
    if(IsBegin == 0){
      scanf("%s", reply_msg);
      int send_res = send(client_sockfd, reply_msg, 1024, 0);
      if(!strcmp(reply_msg,"begin")){
        IsBegin = 1;
      }
    }
    else{
//    printf("waiting device\n");
      bool KeyIsDownLast[KEY_KEY_CODES_COUNT] = {true};
      video::ITexture * iTexture = driver->getTexture("sc.png");
      while(device->run()){
//  memset(recv_msg, BUFFER_SIZE, 0);
//  memset(reply_msg, BUFFER_SIZE, 0);

    //    long byte_num = recv(client_sockfd, recv_msg, BUFFER_SIZE, 0);
    //    recv_msg[byte_num] = 0;
    //    printf("server said: %s\n", recv_msg);

//  printf("relpy:");
	/**
	int fd;  
	fd=open("sc.jpg",O_RDWR|O_CREAT,0666);  
	if(fd<0){  
	  printf("Open file error\n");  
	}  
	lock_set(fd,F_RDLCK);  
	*/
	//        lock_set(fd,F_UNLCK);
	//	close(fd);
//        driver->draw2DImage(iTexture, core::position2d<s32>(0, 0));
        bool MyKeyPressedChanged = false;
        for(int i = 0;i < KEY_KEY_CODES_COUNT;i ++){
          MyKeyPressedChanged = MyKeyPressedChanged || (KeyIsDownLast[i] != receiver.KeyIsDown[i]);
          KeyIsDownLast[i] = receiver.KeyIsDown[i];
        }
        if(MyKeyPressedChanged){
          printf("keyPressed\n");
          int send_res = send(client_sockfd, (char *)&receiver.KeyIsDown, sizeof(receiver.KeyIsDown), 0);
//    int send_res = send(client_sockfd, reply_msg, 1024, 0);
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
  int index = 0;
  while(1){
    char buffer[BUFFER_SIZE_FILE];
//  memset(msg, BUFFER_SIZE, 0);
    char *  file_name = "sc.png";
//  bzero(file_name, BUFFER_SIZE);
//  sprintf(file_name, "sc%d.jpg", index++);


    // 从服务器接收数据到buffer中 
    // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止 
    bzero(buffer, BUFFER_SIZE_FILE); 
    int length = 0; 
    length = recv(server_sockfd, buffer, BUFFER_SIZE_FILE, 0);
    printf("length is %d\n", length);
    if(length > 0){
      /**
      int fd;  
      fd=open("sc.jpg",O_RDWR|O_CREAT,0666);  
      if(fd<0){  
        printf("Open file error\n");  
	break;
      } 
      lock_set(fd,F_WRLCK);  
      if(write(fd, buffer, length) < 0){
	printf("File: %s write failed", file_name);                                                                                                                                                                                             }
      lock_set(fd,F_UNLCK);
      close(fd);
      **/

      FILE* fp = fopen(file_name, "w");
      if(NULL == fp) {
	printf("File:\t%s Can Not Open To Write\n", file_name);
	break;
      }
      if(fwrite(buffer, sizeof(char), length, fp) < length){
	printf("File: %s write failed", file_name);
      }
      fclose(fp);

//    video::ITexture * iTexture = driver->getTexture(file_name);
      
    }else{
      printf("recv file data failed\n");
    }
    /**
    while((length = recv(data_sockfd, buffer, BUFFER_SIZE, 0)) > 0) { 
      if(strcmp(buffer, "over") == 0){
	break;
      }else if(fwrite(buffer, sizeof(char), length, fp) < length) { 
	printf("File:\t%s Write Failed\n", file_name); 
	break; 
      } 
      bzero(buffer, BUFFER_SIZE); 
    } 
    **/
    // 接收成功后，关闭文件，关闭socket 
    printf("Receive File: %s  Successful!\n", file_name); 
  }
    /**
    if(recv(server_sockfd, msg, BUFFER_SIZE, 0) <= 0){
      printf("server connection error!\n");
      break;
    }
    printf("data from server: %s\n", msg);
  }
    **/
  close(server_sockfd);
  pthread_exit((void*)"thread exit");

}






int lock_set(int fd,int type){
  struct flock lock;

  lock.l_type=type;
  lock.l_start=0;
  lock.l_whence=SEEK_SET;
  lock.l_len = 0;
  lock.l_pid=-1;

  fcntl(fd,F_GETLK,&lock);
  if(lock.l_type!=F_UNLCK){
    if(lock.l_type == F_RDLCK)
      printf("Read lock already set by %d!\n",lock.l_pid);
    else if(lock.l_type == F_WRLCK)
      printf("Write lock already set by %d!\n",lock.l_pid);
  }
  lock.l_type = type;
  //此处的F_SETLKW为F_SETLK的阻塞版本，当无法获取锁时进入睡眠等待状态                                                                                                                                                                      
  if(fcntl(fd,F_SETLKW,&lock)<0){
    printf("Lock failed:type=%d!\n",lock.l_type);
    exit(1);
  }

  switch(lock.l_type){
  case F_RDLCK:
    printf("read lock set by %d\n",getpid());
    break;
  case F_WRLCK:
    printf("write lock set by %d\n",getpid());
    break;
  case F_UNLCK:
    printf("UN lock set by %d\n",getpid());
    break;
  default:
    break;
  }
}

