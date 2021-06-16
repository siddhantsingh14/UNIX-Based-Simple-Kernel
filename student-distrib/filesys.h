#ifndef FILESYS_H
#define FILESYS_H

#include "types.h"
#include "multiboot.h"
#include "keyboard.h"

#ifndef ASM

#define FILENAME_LEN        32
#define MAX_FILESIZE        1023
#define BLOCKSIZE           4096
#define DENTRY_RES_BITS     24
#define NUM_MAX_DENTRIES    63
#define RESERVED_BITS       52

typedef struct file_operations_t {
    int32_t (*open)(const uint8_t*);
    int32_t (*close)(int32_t);
    int32_t (*read)(int32_t, void*, int32_t );
    int32_t (*write)(int32_t , const void* , int32_t);
} file_operations_t;

typedef struct dentry_t {
    int8_t filename[FILENAME_LEN];
    int32_t filetype;
    int32_t inode_num;
    int8_t reserved[DENTRY_RES_BITS];
} dentry_t;

typedef struct bootblock_t {
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t reserved[RESERVED_BITS];
    dentry_t direntries[NUM_MAX_DENTRIES];
} bootblock_t;

typedef struct inode_t {
    int32_t length;
    int32_t data_block_num[MAX_FILESIZE];
} inode_t;

typedef struct datablock_t {
    uint8_t data[BLOCKSIZE];
} data_block_t;

typedef struct file_descriptor_t {
    file_operations_t* fops_ptr;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags;
} file_descriptor_t;

typedef struct pcb_t {
    file_descriptor_t fds[8];
    uint8_t args[128];  //keep this for later use, creating a skeleton
    uint32_t ksp;
    uint32_t kbp;
    uint32_t pid;
    uint8_t state;
    uint32_t ss0, esp0; //tss state
    uint32_t terminal;
    uint32_t vidmap;
    struct pcb_t *parent;
} pcb_t;

void init_filesys (uint32_t* start);

int32_t file_open (const uint8_t* filename);
int32_t file_close (int32_t fd);
int32_t file_read (int32_t fd, void* buf, int32_t nbytes);
int32_t file_write (int32_t fd, const void* buf, int32_t nbytes);

int32_t directory_open (const uint8_t* filename);
int32_t directory_close (int32_t fd);
int32_t directory_read (int32_t fd, void* buf, int32_t nbytes);
int32_t directory_write (int32_t fd, const void* buf, int32_t nbytes);

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t get_inode(uint32_t inode);

#endif /* ASM */
#endif /* FILESYS_H */
