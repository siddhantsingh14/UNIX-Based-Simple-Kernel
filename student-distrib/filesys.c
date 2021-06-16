#include "lib.h"
#include "filesys.h"
#include "syscall.h"


#define FAILURE         -1
#define SUCCESS         0

#define FILENAME        8
#define BB_DIR_OFFSET   16 //(4+4+4+52)/4

#define FILESYS_SIZE    4096
#define FILESYS_BYTES   1024
#define FILESYS_STATS   64

#define RTC_FILETYPE    0
#define DIR_FILETYPE    1
#define REG_FILETYPE    2

static uint32_t dir_count;
static uint32_t inode_count;
static uint32_t data_count;
static bootblock_t* bootblock_addr;

static uint8_t num_files_read;
inode_t* inodes;

/*
 * init_filesys
 * DESCRIPTION: init boot block vars with appropriate values
 * INPUTS: start -- file system address
 * OUTPUTS: none
 * RETURN VALS: none
 * SIDE EFFECTS: file system is initialized
 */
void init_filesys (uint32_t* start){
    // get location of file system
    bootblock_addr = (bootblock_t*)start;

    // get num of dentries 
    dir_count = bootblock_addr->dir_count;

    // get num of inodes
    inode_count = bootblock_addr->inode_count;

    // get num of data blocks
    data_count = bootblock_addr->data_count;

    // set location of inode array
    inodes = ((inode_t*)bootblock_addr + 1);
}

/*
 * file_open
 * DESCRIPTION: open file for reading
 * INPUTS: filename -- file to open
 * OUTPUTS: none
 * RETURN VALS: FAILURE, SUCCESS
 * SIDE EFFECTS: none
 */
int32_t file_open (const uint8_t* filename){
    return SUCCESS;
}


/*
 * file_close
 * DESCRIPTION: closes file for reading
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALS: FAILURE, SUCCESS
 * SIDE EFFECTS: none
 */
int32_t file_close (int32_t fd) {
    return SUCCESS;
}

/*
 * file_read
 * DESCRIPTION: closes file for reading
 * INPUTS: fname -- file descriptor
 *         buf -- where to read to
 *         nbytes - how much to read 
 * OUTPUTS: none
 * RETURN VALS: FAILURE or # bytes read if call was successful
 * SIDE EFFECTS: none
 */
int32_t file_read (int32_t fd, void* buf, int32_t nbytes) {
    uint32_t inode;
    uint8_t* data_to_read = (uint8_t*)buf;
    pcb_t *pcb = get_pcb();
    uint32_t offset = pcb->fds[fd].file_pos;
    inode = pcb->fds[fd].inode;
    int bytes_read = read_data(inode, offset, data_to_read, nbytes);
    offset += bytes_read;
    pcb->fds[fd].file_pos = offset;
    return bytes_read;
}

/*
 * file_write
 * DESCRIPTION: returns -1 bc implementation is for read-only
 * INPUTS: none	
 * OUTPUTS: none
 * SIDE EFFECTS: none
 */
int32_t file_write (int32_t fd, const void* buf, int32_t nbytes) {
    return FAILURE;
}

/*
 * directory_open
 * DESCRIPTION: open directory for reading
 * INPUTS: filename -- directory to open
 * OUTPUTS: none
 * RETURN VALS: FAILURE, SUCCESS
 * SIDE EFFECTS: none
 */
int32_t directory_open (const uint8_t* filename){
    num_files_read = 0;
    return SUCCESS;
}


/*
 * directory_close
 * DESCRIPTION: closes directory for reading
  * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: none
 */
int32_t directory_close (int32_t fd) {
    return SUCCESS;
}

/*
 * directory_read
 * DESCRIPTION: closes file for reading
 * INPUTS: fd -- which file to read
 *         buf -- where to read to
 *         nbytes - how much to read 
 * OUTPUTS: none
 * RETURN VALS: FAILURE or # bytes read if call was successful
 * SIDE EFFECTS: none
 */
int32_t directory_read (int32_t fd, void* buf, int32_t nbytes) {

    // if file is closed or there's no data, return failure
    //printf("check read");
    if (!((uint8_t*)buf))
        return FAILURE;

    // init new dentry
    dentry_t curr_dentry;

    // call read_data with correct params 
    if (read_dentry_by_index(num_files_read, &curr_dentry) == SUCCESS) {
        //copy filename into buffer
        strncpy(buf, curr_dentry.filename, FILENAME_LEN);
        ((uint8_t*) buf)[32] = 0;
        num_files_read++;

        return strlen(buf);
    }

    return FAILURE;
}

/*
 * directory_write
 * DESCRIPTION: returns -1 bc implementation is for read-only
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: none
 */
int32_t directory_write (int32_t fd, const void* buf, int32_t nbytes) {
    return FAILURE;
}

/*
 * read_dentry_by_name
 * DESCRIPTION: Scans through the directory entries in the “boot block” to find the file name
 * INPUTS: fname -- name of file
 *         dentry -- struct to set up
 * OUTPUTS: FAILURE OR SUCCESS
 * SIDE EFFECTS: none
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
    int i;
    if(strlen((int8_t*)fname) > 32){ //max length
            return FAILURE;
    }
      
    for (i = 0; i < NUM_MAX_DENTRIES; i++) {
        if (strncmp(bootblock_addr->direntries[i].filename, (int8_t*)fname, 32) == 0) {
            // get dentry's filename
                strncpy(dentry->filename, bootblock_addr->direntries[i].filename, 32);
                // get dentry's filetype
                dentry->filetype = bootblock_addr->direntries[i].filetype;
                // get dentry's inode
                dentry->inode_num = bootblock_addr->direntries[i].inode_num;
                return SUCCESS;
        }
    }
    return FAILURE;
}

/*
 * read_dentry_by_index
 * DESCRIPTION: Populates the dentry parameter -> file name, file type, inode number
 * INPUTS: index -- index for dentry to be read
 * OUTPUTS: dentry -- returned with fields all populated
 * RETURN VALUES: FAILURE or SUCCESS
 * SIDE EFFECTS: none
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {

    // copy filename
    strncpy(dentry->filename, bootblock_addr->direntries[index].filename, FILENAME_LEN);
    
    // copy file type and inode num
    dentry->filetype = bootblock_addr->direntries[index].filetype;
    dentry->inode_num = bootblock_addr->direntries[index].inode_num;
    return SUCCESS;
}

/*
 * read_data
 * DESCRIPTION: open a file; set up the file object -> inode, fops, flags, position 
 * INPUTS: inode -- file number to be read
 *         offset -- start of file
 *         length -- how much data to read
 * OUTPUTS: buf -- how much data was read
 * RETURN VALS: FAILURE, SUCCESS
 * SIDE EFFECTS: none
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
 
    if (inode >= bootblock_addr->inode_count)
        return FAILURE;

    inode_t* curr_inode = (( inode_t*) bootblock_addr + inode + 1);

    int i;

    for (i = offset; i < offset + length; i++) {
        int block_idx = *(curr_inode->data_block_num + (i/4096));

        if (block_idx >= data_count) {
            return i - offset - 1;
        }

        data_block_t* data_struct = ((data_block_t*)bootblock_addr + (bootblock_addr->inode_count + 1 + block_idx));
        *(buf++) = *(data_struct->data + i % 4096);

        if (i == curr_inode->length)
            return i - offset;
    }
    return i - offset;
}
