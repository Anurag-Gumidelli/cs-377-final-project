
#include "fs.h"
#include <bits/stdc++.h>

#define PORT 48664

using namespace std;



int main(int argc, char* argv[]) {

  if (argc == 1) {
    fprintf(stderr, "usage: %s <diskFileName> \n", argv[0]);
    exit(0);
  }

  myFileSystem *f = new myFileSystem((char*)argv[1]);// initialize file

  int rc;
  vector<pthread_t> connections;

  // -------SERVER NETWORK SETUP CODE STARTS---------

  // Start a server socket to listen to incoming connections and listen to them
  int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);


	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 48664
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET; // Specifies this is for ipv4 connections
	address.sin_addr.s_addr = INADDR_ANY; // Accepts connections from any src ip 
	address.sin_port = htons(PORT); // creates platform specific PORT number addressing scheme

	// Forcefully attaching socket to the port 48664
	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
 // -------SERVER NETWORK SETUP CODE ENDS---------

  cout << "Server is up and running for disk image " << (char*)argv[1] << " on server port " << PORT << ".\n";

  while(true){ // keep listening to incoming connections and spin a new thread after each thread is created and give them a file object
    if ((new_socket
      = accept(server_fd, (struct sockaddr*)&address,
          (socklen_t*)&addrlen))
      < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    cout << "User " << new_socket << " has connected to the file system!\n";

    pthread_t new_conn;

    argPointers *worker_args = (argPointers *)malloc(sizeof(argPointers));

    (*worker_args).socket_fd = (void *)(long long int)new_socket;
    (*worker_args).file_pointer = (void *)f;

    rc = pthread_create(&new_conn, NULL, worker, (void *)worker_args);
    // create new worker thread and launch it with the appropriate arguments (worker_args) FileSystem object and Socket_fd
    assert(rc == 0);

    connections.push_back(new_conn);
  }

  f->close_disk();
  return 0;
}



/**
 * @brief Handle new TCP connections and connect them with the file
 * This worker thread 
 * @param socket_fd
 * @return void* 
 */
void* worker(void *args){

  int cmd_cnt, socket, ret;

  argPointers * thread_args = (argPointers *)args;
  socket = (long long int) thread_args->socket_fd;
  myFileSystem *f = (myFileSystem *)(thread_args->file_pointer);

  char buffer[1024]; // read incoming packets and strtok
  for (int i = 0; i < 1024; i++) {
    buffer[i] = '\0';
  }
  char buff[1024];
  // fill dummy buffer with 1's to write default
  for (int i = 0; i < 1024; i++) {
    buff[i] = '1';
  }
  char read_buff[1024]; // read buffer
  for (int i = 0; i < 2048; i++) {
    read_buff[i] = '\0';
  }

  char send_buff[2048]; // send buffer back to the client
  for (int i = 0; i < 2048; i++) {
    send_buff[i] = '\0';
  }

  char *cmd[3];
  char *input;
  bool quit = false;

  

  while (true) {
    // split line into parts
    read(socket, buffer, 1024);
    
    cmd_cnt = 0;
    input = strtok(buffer, " ");

    while (input != NULL && cmd_cnt < 10) {
      cmd[cmd_cnt] = input;
      cmd_cnt++;
      input = strtok(NULL, " ");
    }

    // process each part from cmd


    switch (*cmd[0]) {
      // Use thread safe access to memory, memory access level set to Serializable
      // Will only observe completed operations

      case 'C': { // CREATE
        
        pthread_mutex_lock(&(f->mem_lock));
        ret = f->create_file((char*)cmd[1], atoi(cmd[2]));
        pthread_mutex_unlock(&(f->mem_lock));

        if(ret == 1){
          sprintf(send_buff, "Successfully created file %s.\n", cmd[1]);
        }else{
          sprintf(send_buff, "Failed file creation.\n");
        }
        break;
      }
      case 'D': { // DELETE

        pthread_mutex_lock(&(f->mem_lock));
        ret = f->delete_file((char*)cmd[1]);
        pthread_mutex_unlock(&(f->mem_lock));

        if(ret == 1){
          sprintf(send_buff, "Successfully deleted file %s.\n", cmd[1]);
        }else{
          sprintf(send_buff, "Failed file deletion.\n");
        }
        break;
      }
      case 'L': { // LIST FILES

        pthread_mutex_lock(&(f->mem_lock));
        ret = f->ls(send_buff);
        pthread_mutex_unlock(&(f->mem_lock));

        if(ret == 0){
          sprintf(send_buff, "%s",read_buff);
        }else{
          sprintf(send_buff, "List files operation failed.\n");
        }
        break;
      }
      case 'R': { // READ FILE BLOCK

        pthread_mutex_lock(&(f->mem_lock));
        ret = f->read((char*)cmd[1], atoi(cmd[2]), read_buff);
        pthread_mutex_unlock(&(f->mem_lock));

        if(ret == 1){
          cout << cmd[1];
          sprintf(send_buff, "Successfully read file %s.\noutput:\n%s", cmd[1], read_buff);
        }else{
          sprintf(send_buff, "Failed reading file.\n");
        }
        break;
      }
      case 'W': { // WRITE FILE BLOCK

        pthread_mutex_lock(&(f->mem_lock));
        ret = f->write((char*)cmd[1], atoi(cmd[2]), buff);
        pthread_mutex_unlock(&(f->mem_lock));

        if(ret == 1){
          sprintf(send_buff, "Write successful for file %s.\n", cmd[1]);
        }else{
          sprintf(send_buff, "Failed writing to file.\n");
        }
        break;
      }
      case 'Q':{ // CLOSE THIS TCP connection 
        quit = true;
        sprintf(send_buff, "Bye!\n");
        break;
      } 
      default: { // Unknown operation suggest Quit option
        printf("No OP!\n");
        sprintf(send_buff, "To quit select: Q\n\n");
        break;
      }
    }  // end switch

    send(socket, send_buff, strlen(send_buff), 0); // send the message to the telnet client wiht success, failure and/or read blocks!
    printf("Message sent to client %d\n", socket);

    for (int i = 0; i < 1024; i++) {
      buffer[i] = '\0';
    } // clear buffer
    for (int i = 0; i < 2048; i++) {
      send_buff[i] = '\0';
    } // clear send_buff
    for (int i = 0; i < 1024; i++) {
      read_buff[i] = '\0';
    } // clear send_buff

    if(quit){
      break;
    }
  }

  close(socket); // close the socket stated here and join the main thread
  
  return NULL;
}
