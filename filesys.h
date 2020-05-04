#ifndef FILESYS_H
#define FILESYS_H

#include "types.h"
#include "lib.h"
#include "syscall.h"


#define  MAX_DATA_BLOCK 1023
#define MAX_ENTRIES 63
#define BLOCK_SIZE 4096


typedef struct 
{
    char name[32];     // file name
    
    uint32_t type   ;    // file type
    uint32_t inode ;     // inode #
    uint8_t reserved[24];

}dentry_t;

typedef struct 
{
    
    uint32_t length   ;    // length in byte
    uint32_t data_block[MAX_DATA_BLOCK] ;     // which data block have the file content
    

}inode_t;

typedef struct 
{
    
    uint32_t number_entries   ;    // length in byte
    uint32_t    N;    // number_inode
    uint32_t    D;      // data blocks
    uint8_t reserved[52];
    dentry_t entries[MAX_ENTRIES];
    

}bootblock_t;








void initial_filesys(uint32_t file_sys_addr);
int32_t read_dentry_by_name ( const uint8_t* fname,dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode,uint32_t  offset,uint8_t* buf,uint32_t length);

int32_t file_open_1(uint32_t* inode, char* filename);

int32_t file_close_1();
int32_t file_read_1(uint32_t inode, uint32_t* offset, uint8_t* buf,uint32_t len);
int32_t file_write_1();
int32_t dir_open_1();
int32_t dir_close_1();
int32_t dir_read_1(uint8_t* buf, uint32_t index, uint32_t length);
int32_t dir_write_1();

int32_t file_open(const uint8_t* filename);
int32_t file_close (int32_t fd);
int32_t file_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t file_read (int32_t fd, void* buf, int32_t  nbytes);

int32_t dir_open(const uint8_t* filename);
int32_t dir_close (int32_t fd);
int32_t dir_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_read (int32_t fd, void* buf, int32_t  nbytes);

int32_t number_of_entries();
int32_t size_of_file(uint32_t inode);


#endif
