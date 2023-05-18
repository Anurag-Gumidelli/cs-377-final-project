/*
  “Academic honesty” means demonstrating and upholding the highest
  integrity and honesty in all the academic work that you do as an
  individual (not in a group assignment). In short, it means doing
  your own work and not cheating, and not presenting the work of
  others as your own. This includes cheating, deception, fabrication,
  and plagiarism. At this point in your academic career you should
  know what is right and what is not. If you are unclear about what
  constitutes academic honesty you must talk to the professor of this
  course.
 */
#include "fs.h"
#include <iostream>
#include <sstream>

using namespace std;

myFileSystem::myFileSystem(char diskName[16]) {
  // open the file with the above name.
  // this file will act as the "disk" for your file system.
    disk.open(diskName);
}

int myFileSystem::create_file(char name[8], int size) {
  // create a file with this name and this size.
  // Step 1: Check to see if we have sufficient free space on disk by
  //   reading in the free block list. To do this:
  // Move the file pointer to the start of the disk file.
  // Read the first 128 bytes (the free/in-use block information)
  // Scan the list to make sure you have sufficient free blocks to
  //   allocate a new file of this size

  if (size >= 1 && size <=8) {

    int c_node = -1;

    char freeblockList[128];
    disk.seekg(0); // moves file pointer
    disk.read(freeblockList, 128);

    int num_free_blocks = 0;
    for (int i = 0; i < 128; i++) {
      if (!freeblockList[i]) { // if the i-th byte is 0, it indicates that the block is free
        if(++num_free_blocks == size){
          break;//found the needed number of freeblocks for this file
        }
      }
    }
    if(num_free_blocks < size){
      return -1;
    }


    // Step 2: we look for a free inode on disk
    // Read in an inode
    // Check the "used" field to see if it is free
    // If not, repeat the above two steps until you find a free inode
    // Set the "used" field to 1
    // Copy the filename to the "name" field
    // Copy the file size (in units of blocks) to the "size" field
    idxNode inode;

    disk.seekg(128, disk.beg);

    for (int i = 0; i < 16; i++) { // 16 = number of inodes

      disk.read((char *)&inode, sizeof(inode));
      if (inode.used == 0) {
        inode.used = 1;
        strcpy(inode.name, name);
        inode.size = size;
        c_node = i;
        break;
      }
      else if (i == 15){
        return -1;
      }
    }

    cout << "creating file :" << inode.name << " at node number " << c_node << endl;

    // Step 3: Allocate data blocks to the file
    // for(i=0;i<size;i++)
    // Scan the block list that you read in Step 1 for a free block
    // Once you find a free block, mark it as in-use (Set it to 1)
    // Set the blockPointer[i] field in the inode to this block number.
    // end for
    int allocatedBlocks = 0;

    for (int i = 0; i < 128; i++) {
      if (!freeblockList[i]) {
        freeblockList[i] = 1;
        inode.blockPointers[allocatedBlocks++] = i;
      }
      if (allocatedBlocks == size) { 
        break;
      }
    }

    // Step 4: Write out the inode and free block list to disk
    // Move the file pointer to the start of the disk file
    // Write out the 128 byte free block list to the disk file
    // Move the file pointer to the position on disk where this inode was stored
    // Write out the inode to the disk file
    disk.seekp(0, disk.beg); // moves file pointer
    disk.write(freeblockList, 128);
    int node_offset = (c_node * 48 ) + 128;
    disk.seekp(node_offset, disk.beg);
    disk.write((char *)&inode, 48);
    // Good luck!
    return 1;
  }
  return -1;
}  // End Create

int myFileSystem::delete_file(char name[8]) {
  // Delete the file with this name
  // Step 1: Locate the inode for this file
  // Move the file pointer to the 1st inode (129th byte)
  // Read in an inode
  // If the inode is free, repeat above step.
  // If the inode is in use, check if the "name" field in the
  //   inode matches the file we want to delete. If not, read the next
  //   inode and repeat
  int file_num = -1;
  idxNode inode;

  disk.seekg(128, disk.beg); // move file pointer

  for(int i = 0; i < 16; i++){
    disk.read((char *)&inode, sizeof(inode));  // read an inode
    if (inode.used == 1 && !strcmp(inode.name,name)) {
      file_num = i;
      break;
    }
    else if(i == 15){
      return -1;
    }
  }

  // Step 2: free blocks of the file being deleted
  // Read in the 128 byte free block list (move file pointer to start
  //   of the disk and read in 128 bytes)
  // Free each block listed in the blockPointer fields as follows:
  // for(i=0;i< inode.size; i++)
  // freeblockList[ inode.blockPointer[i] ] = 0;
  disk.seekg(0, disk.beg);
  char freeblockList[128];
  disk.read(freeblockList, 128);

  for (int i = 0; i < inode.size; i++) {
    freeblockList[inode.blockPointers[i]] = 0;
  }

  // Step 3: mark inode as free
  // Set the "used" field to 0.
  inode.used = 0;

  // Step 4: Write out the inode and free block list to disk
  // Move the file pointer to the start of the file
  // Write out the 128 byte free block list
  // Move the file pointer to the position on disk where this inode was stored
  // Write out the inode
  disk.seekp(0, disk.beg); // moves file pointer
  disk.write(freeblockList, 128);
  disk.seekp((128 + (48 * file_num)), disk.beg);
  disk.write((char *)&inode, 48);

  // return 1;
  return 1;
}  // End Delete

int myFileSystem::ls(char buf[1024]) {
  // List names of all files on disk
  // Step 1: read in each inode and print
  // Move file pointer to the position of the 1st inode (129th byte)
  // for(i=0;i<16;i++)
  // Read in an inode
  // If the inode is in-use
  // print the "name" and "size" fields from the inode
  // end for
  idxNode inode;
  stringstream s;

  disk.seekg(128, ios::beg); // move file pointer
  for (int i = 0; i < 16; i++) {
    disk.read((char *)&inode, sizeof(inode));  // read an inode
    if (inode.used == 1) {
      s << "name = " << inode.name << " and size = " << inode.size << ".\n";
    }
  }

  sprintf( buf, "%s", s.str());

  return 0;
}  // End ls

int myFileSystem::read(char name[8], int blockNum, char buf[1024]) {
  // read this block from this file
  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in an inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat

  idxNode inode;
  
  disk.seekg(128, ios::beg);
  for (int i = 0; i < 16; i++) {
    disk.read((char *)&inode, sizeof(inode));  // read an inode
    if (inode.used == 1 && !strcmp(inode.name, name)) {
      break;
    }
    if(i == 15){
      return -1;
    }
  }

  // Step 2: Read in the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // That is, addr = inode.blockPointer[blockNum]
  // Move the file pointer to the block location (i.e., to byte #
  //   addr*1024 in the file)
  // Read in the block => Read in 1024 bytes from this location
  //   into the buffer "buf"
  if (blockNum < inode.size) {
    int block_addr = inode.blockPointers[blockNum];
    disk.seekg((block_addr * 1024), disk.beg);
    disk.read(buf, 1024);
    return 1;
  }
  else {
    return -1;
  }

  // return -1;
}  // End read

int myFileSystem::write(char name[8], int blockNum, char buf[1024]) {
  // write this block to this file
  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in a inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat
  idxNode inode;
  disk.seekg(128, ios::beg);

  for (int i = 0; i < 16; i++) {
    disk.read((char *)&inode, sizeof(inode));  // read an inode
    if (inode.used == 1 && !strcmp(inode.name, name)) {
      break;
    }
    if(i == 15){
      return -1;
    }
  }

  // Step 2: Write to the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // That is, addr = inode.blockPointer[blockNum]
  // Move the file pointer to the block location (i.e., byte # addr*1024)

  // Write the block! => Write 1024 bytes from the buffer "buff" to
  //   this location
  if (blockNum < inode.size) {
    int addr = inode.blockPointers[blockNum];
    disk.seekp((addr * 1024), disk.beg);
    disk.write(buf, 1024);
    return 1;
  }
  else {
    return -1;
  }
}  // end write

int myFileSystem::close_disk() {
  // close the disk! 
  // if (disk.close()) { return 1; }
  if (!disk.is_open()) { return 0; }
  disk.close();
  if (!disk.is_open()) { return 1; }
  return 0;
}
