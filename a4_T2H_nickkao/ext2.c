#include "ext2.h"
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fsuid.h>
#include <sys/stat.h>
#include <sys/types.h>

#define EXT2_OFFSET_SUPERBLOCK 1024
#define EXT2_INVALID_BLOCK_NUMBER ((uint32_t) -1)
/*
> gdb ext2test
> break 78
> run /home/c/cs-313/EXTimages/SparseSmall.img
> call read_inode(volume, 2 , &inode)
*/

/* open_volume_file: Opens the specified file and reads the initial
   EXT2 data contained in the file, including the boot sector, file
   allocation table and root directory.
   
   Parameters:
     filename: Name of the file containing the volume data.
   Returns:
     A pointer to a newly allocated volume_t data structure with all
     fields initialized according to the data in the volume file
     (including superblock and group descriptor table), or NULL if the
     file is invalid or data is missing, or if the file is not an EXT2
     volume file system (s_magic does not contain the correct value).
 */
volume_t *open_volume_file(const char *filename) {
  volume_t *volume = NULL;
  
  int fd = open(filename, O_RDONLY);
  if (fd < 0) return NULL;

  /* Seek past the first 1024 bytes and read in a superblock. */
  if (lseek(fd, 1024, SEEK_CUR) != 1024) goto err;

  /* Use calloc, so all fields are 0. */
  if ((volume = calloc(1, sizeof(volume_t))) == NULL) goto err;
  volume->fd = fd;

  /* Now read the superblock. */
  if (read(fd, &volume->super, sizeof(struct superblock)) != sizeof(struct superblock)) goto err;

  /* Now verify that this is a valid ext2 image. */
  if (volume->super.s_magic != EXT2_SUPER_MAGIC) goto err;

  /* Now fill in fields in the volume structure. */
  volume->block_size = 1024 << volume->super.s_log_block_size;
  volume->volume_size = volume->block_size * volume->super.s_blocks_count;

  /*
   * To compute groups, subtract the initial space consumed by the superblock,
   * the reserved space, and the group descriptor table. Assumption: group
   * descriptor table fits in one block. Start with two blocks: one containing
   * the superblock and one containing the group descriptor table.
   */
  int metablocks = 2;
  /*
    If we have a 1024 block size, then we need an extra block for the 1024
    reserved bytes.
   */
  if (volume->block_size == 1024)
	metablocks++;
  volume->num_groups = (volume->super.s_blocks_count - metablocks) /
    volume->super.s_blocks_per_group;

  /*
   * This is a hack: the file system can have fewer blocks than the number
   * set in super.s_blocks_per_group, so handle this explicitly.
   */
  if (volume->num_groups == 0)
	volume->num_groups = 1;

  /* Make sure the block descriptors fit in a single block. */
  size_t groupmeta = volume->num_groups * sizeof(struct group_desc);
  if (groupmeta > volume->block_size) {
    fprintf(stderr, "Group block descriptors exceed block size\n");
    goto err;
  }
  volume->groups = calloc(1, groupmeta);
  if (volume->groups == NULL) goto err;

  /* Now, seek to where we expect to find the group descriptor information. */
  off_t group_desc_offset = volume->block_size * (metablocks - 1);
  if (lseek(fd, group_desc_offset, SEEK_SET) != group_desc_offset) goto err;

  if (read(fd, volume->groups, groupmeta) != groupmeta) {
    fprintf(stderr, "Unable to read groupmeta data\n");
    goto err;
  }

  /*
   * This is a common system paradigm for placing all your cleanup code together
   * so that on an error, you can be sure to free all the resources that you
   * allocated.
   */
  if (0) {
err: 
    close_volume_file(volume);
    volume = NULL;
  }
  return volume;
}

/* close_volume_file: Frees and closes all resources used by a EXT2 volume.
   
   Parameters:
     volume: pointer to volume to be freed.
 */
void close_volume_file(volume_t *volume) {
  if (volume != NULL) {
    close(volume->fd); 
    if (volume->groups != NULL)
      free(volume->groups);
    free(volume);
  }
}

/* read_disk_block: Given a disk address (physical block number on the drive),
   read the appropriate block from the disk and place it in the buffer. 

   Parameters:
     volume: Pointer to the volume
     daddr: Disk block number to read
     buffer: Pointer to the locatoin in which to place the block

   Returns:
     0 on success; -1 on failure.
*/
int read_disk_block(volume_t *volume, uint32_t daddr, void *buffer) {
  off_t offset = daddr * volume->block_size;
  if (lseek(volume->fd, offset, SEEK_SET) != offset) return -1;
  if (read(volume->fd, buffer, volume->block_size) != volume->block_size) return -1;
  return 0;
}

/* read_inode: Fills an inode data structure with the data from one
   inode in disk. Determines the block group number and index within
   the group from the inode number, then reads the inode from the
   inode table in the corresponding group. Saves the inode data in
   buffer 'buffer'.
   
   Parameters:
     volume: pointer to volume.
     inode_no: Number of the inode to read from disk.
     buffer: Pointer to location where data is to be stored.

   Returns:
     In case of success, returns a positive value. In case of error,
     returns -1.
 */
ssize_t read_inode(volume_t *volume, uint32_t inode_no, inode_t *buffer) {
  uint8_t block_buf[volume->block_size];
  
  /*
   * We find the inode using the following steps:
   * 0. Check for errors.
   * 1. Determine in which block group the inode resides
   * 2. Determine the inode's offset within the block group
   * 3. Identify the block in which that inode resides.
   * 4. Determine the block address for that block
   * 5. Read the block
   * 6. Place the proper bytes into the inode structure.
   */
  /* 0. Error checking. */
  if (inode_no != EXT2_ROOT_INO && inode_no < volume->super.s_first_ino) return -1;

  /* XXX This is awful, but it seems that because inode number 0 is an invalid inode,
   * Linux does not even store this inode, so all inode computation takes the value
   * of the inode and decrements it by one. Don't ever do that when designing a system.
   * if 0 is an invalid object, that you don't use it, but don't change the values
   * of object IDs (which is what an inode number is).
   */
  inode_no--;

  /* 1. Find the block group. */
  int block_group = inode_no / volume->super.s_inodes_per_group;

  /* 2. Find the offset within the group. */
  int inode_in_group = inode_no % volume->super.s_inodes_per_group;

  /* 3. Figure out in which inode block the inode resides. */
  int inodes_per_block = volume->block_size / volume->super.s_inode_size;
  uint32_t block_within_group = inode_in_group / inodes_per_block;

  /* 4. Determine the block address for that block */
  uint32_t inode_daddr = volume->groups[block_group].bg_inode_table + block_within_group;

   /* 5. Read the block */
  if (read_disk_block(volume, inode_daddr, block_buf) != 0) return -1;

   /* 6. Place the proper bytes into the inode structure. */
  memcpy(buffer, block_buf + (inode_in_group % inodes_per_block) * volume->super.s_inode_size, volume->super.s_inode_size);

  return 0;
}

/* lbn_to_daddr: Given a logical block number, returns the physical
   block number or disk address for that block.
   
   Parameters:
     volume: Pointer to volume.
     inode: Pointer to inode structure where data is to be sourced.
     lbn: Logical block number

   Returns:
     In case of success, returns the physical block number (disk address)
     indicating where the given lbn resides on disk. If no disk block has
     been allocated for this lbn, returns 0 (zero).  In case of error,
     returns EXT2_INVALID_BLOCK_NUMBER.
 */

    /*
        12 direct 

    */
static uint32_t lbn_to_daddr(volume_t *volume, inode_t *inode, uint64_t lbn) {
    
    // Will be in direct block
    // block numbers are 32 bits 4 bytes
    int block_size = ext2_blocksize(volume);
    // int offset = lbn * block_size;
    int ptrInBlock = block_size/4;
    int interimIndex;
    uint32_t interimBlockAddr;
    //the lbn indicates offset is larger thant he indoe file size so invalid
    // if(offset > inode_file_size(volume, inode)){
    //     return EXT2_INVALID_BLOCK_NUMBER;
    // }

    uint32_t block_buf[block_size];

    int directUpBound, indHighBound, dIndHighBound, tIndHighBound;

    directUpBound = EXT2_NDIR_BLOCKS;
    indHighBound = directUpBound + ptrInBlock;
    dIndHighBound = indHighBound + (ptrInBlock * ptrInBlock);
    tIndHighBound = dIndHighBound + (ptrInBlock * ptrInBlock * ptrInBlock);

    if(lbn >= 0 && lbn < directUpBound){
        return inode->i_block[lbn];
    }

    else if(lbn >= directUpBound && lbn < indHighBound){
        interimIndex = lbn-directUpBound;
        if(read_disk_block(volume, inode->i_block_1ind, block_buf) != 0){
            return EXT2_INVALID_BLOCK_NUMBER;
        }else{
            return block_buf[interimIndex];
        }
    }
    else if(lbn >= indHighBound && lbn < dIndHighBound){
        interimIndex = lbn - indHighBound;
        if(read_disk_block(volume, inode->i_block_2ind, block_buf) != 0){
            return EXT2_INVALID_BLOCK_NUMBER;
        }else{
            interimBlockAddr = block_buf[interimIndex/ptrInBlock];
            if(read_disk_block(volume, interimBlockAddr, block_buf) != 0){
                return EXT2_INVALID_BLOCK_NUMBER;
            }else{
                return block_buf[interimIndex%ptrInBlock];
            }
        }
    }
    else if(lbn >= dIndHighBound && lbn < tIndHighBound){
        interimIndex = lbn - dIndHighBound;
        if(read_disk_block(volume, inode->i_block_3ind, &block_buf) != 0){
            return EXT2_INVALID_BLOCK_NUMBER;
        }else{
            int s = interimIndex/(ptrInBlock * ptrInBlock); 
            interimBlockAddr = block_buf[s];
            if(read_disk_block(volume, interimBlockAddr, &block_buf) != 0){
                return EXT2_INVALID_BLOCK_NUMBER;
            }else{
                interimIndex = interimIndex % (ptrInBlock * ptrInBlock);
                s = interimIndex/ptrInBlock;
                interimBlockAddr = block_buf[s];
                if(read_disk_block(volume, interimBlockAddr, &block_buf) != 0){
                    return EXT2_INVALID_BLOCK_NUMBER;
                }else{
                    s = interimIndex%ptrInBlock;
                    return block_buf[s];
                }
            }
        }
    }
    else{
        return EXT2_INVALID_BLOCK_NUMBER;
    }
}


/* read_file_block: Given a file and logical block number (lbn), reads
   the appropriate block from disk.

   Hint: You might find the previous routine helpful.
   
   Parameters:
     volume: Pointer to volume.
     inode: Pointer to inode structure for the file.
     lbn: Logical block number to be read.
     buffer: Pointer to location where data is to be stored.

   Returns:
     In case of success, returns 0; in case of error, returns -1.
 */
int read_file_block(volume_t *volume, inode_t *inode, uint64_t lbn, void *buffer) {

    uint32_t lbnDaddr = lbn_to_daddr(volume, inode, lbn);
    uint64_t blocksize = volume->block_size;

    if(lbnDaddr == 0){
        //buffer needs to be zeroed out 
        bzero(buffer, blocksize);
        return 0;
    }
    if(lbnDaddr == EXT2_INVALID_BLOCK_NUMBER){
        return -1;
    }

    if(read_disk_block(volume, lbnDaddr, buffer) != 0){
        return -1;
    }else
    {
        return 0;
    }
}


/* read_file_content: This is the file system function that implements
   the read system call for ext2. It reads some number of bytes (possibly
   larger than a single block) from a file, starting at the given offset.
   This function should never try to read past the end of the file.
   
   Parameters:
     volume: Pointer to volume.
     inode: Pointer to inode structure for the file.
     offset: Offset, in bytes from the start of the file, of the data
             to be read.
     nbytes: Maximum number of bytes to read from the file.
     buffer: Pointer to location where data is to be stored.

   Returns:
     In case of success, returns the number of bytes read from the
     disk. In case of error, returns -1.
 */
ssize_t read_file_content(volume_t *volume, inode_t *inode, uint64_t offset, uint64_t nbytes, void *buffer) {

  uint64_t bytes_read = 0;
  uint64_t lbn;
  uint8_t block_buf[volume->block_size];/* Buffer to hold a single block. */

  /* 
     If offset + nbytes exceeds the size of the file, we will read fewer
     than max_size bytes; compute that value now.
  */
  if (offset >= inode_file_size(volume, inode)) return 0;

  if (offset + nbytes > inode_file_size(volume, inode))
    nbytes = inode_file_size(volume, inode) - offset;
  
  /*
     We read files in block sized units, but the beginning of the read
     and the end of the read may not be block aligned. One could combine
     the first block handling with the rest of the blocks, but we handle
     it separately for clarity in reading.
  */
  lbn = offset / ext2_blocksize(volume);

  /* Check for partial block read at beginning. */
  uint64_t block_offset = offset % ext2_blocksize(volume);
  if (block_offset != 0) {
    int ret = read_file_block(volume, inode, lbn, block_buf);
    if (ret < 0)
        return ret;
    bytes_read = ext2_blocksize(volume) - offset;
    if (bytes_read > nbytes)
        bytes_read = nbytes;
    memcpy(buffer, block_buf + block_offset, bytes_read);
    lbn++;
  }

  /* Now loop through remaining blocks. */
  uint64_t copy_bytes = ext2_blocksize(volume);
  while (bytes_read < nbytes) {
    int ret = read_file_block(volume, inode, lbn, block_buf);
    if (ret < 0)
        return ret;

    /* Check for last block. */
    if (nbytes - bytes_read < ext2_blocksize(volume))
        copy_bytes = nbytes - bytes_read;
    memcpy(buffer + bytes_read, block_buf, copy_bytes);
    bytes_read += copy_bytes;
    lbn++;
  }
  return bytes_read;
}

/* follow_directory_entries: Reads all entries in a directory, calling
   function 'f' for each entry in the directory. Stops when the
   function returns a non-zero value, or when all entries have been
   traversed.
   
   Parameters:
     volume: Pointer to volume.
     inode: Pointer to inode structure for the directory.
     context: This pointer is passed as an argument to function 'f'
              unmodified.
     buffer: If function 'f' returns non-zero for any file, and this
             pointer is set to a non-NULL value, this buffer is set to
             the directory entry for which the function returned a
             non-zero value. If the pointer is NULL, nothing is
             saved. If none of the existing entries returns non-zero
             for 'f', the value of this buffer is unspecified.
     f: Function to be called for each directory entry. Receives three
        arguments: the file name as a NULL-terminated string, the
        inode number, and the context argument above.

   Returns:
     If the function 'f' returns non-zero for any directory entry,
     returns the inode number for the corresponding entry. If the
     function returns zero for all entries, or the inode is not a
     directory, or there is an error reading the directory data,
     returns 0 (zero).
 */
uint32_t follow_directory_entries(volume_t *volume,
  inode_t *inode, void *context, dir_entry_t *buffer,
   int (*f)(const char *name, uint32_t inode_no, void *context)) {


    uint32_t maxDirEntSize = sizeof(dir_entry_t);
    uint8_t buf[maxDirEntSize];
    uint64_t fsize = inode_file_size(volume, inode);
    uint64_t offset = 0;
    ssize_t sizeRead;
    dir_entry_t *currEntry = malloc(maxDirEntSize);
    uint16_t ftype= inode->i_mode & S_IFMT;
    //if not inode type isn't directory it's wrong
    if(ftype != 0x4000){
        return 0;
    }

    while (offset < fsize){
        sizeRead = read_file_content(volume, inode, offset, maxDirEntSize, &buf);
        if(sizeRead == -1){
            return 0;
        }
        //8 is size of all the "gaurenteed" dir entry fields
        //rest will be char array of name
        bzero(currEntry, maxDirEntSize);
        memcpy(currEntry, buf , 8);
        //if not invalid inode num 
        //there is a name copy into entry struct
        if(currEntry->de_inode_no != 0){
            memcpy(currEntry->de_name, buf + 8, currEntry->de_name_len);
            // char * tempDeName = malloc(currEntry->de_name_len + 1);
            // strcpy(tempDeName, currEntry.de_name);
            currEntry->de_name[currEntry->de_name_len] = '\0';
            if(f(currEntry->de_name, currEntry->de_inode_no, context) != 0){
                if(buffer != NULL){
                    buffer = currEntry;
                }
                return currEntry->de_inode_no;
            }
        }
        offset += currEntry->de_rec_len;
    }
  /* TO BE COMPLETED BY THE STUDENT */
  return 0;
}

/*
 * Function to compare two null-terminated strings. Used by
 * find_file_in_directory to compare a component name against the names
 * stored in directory entries. Note that the inode argument is ignored.
 * Super tricky: strcmp returns 0 when the two strings are the same. Note
 * that we are returning !strcmp -- thus this returns non-zero when the
 * strings are the same!
 */
static int compare_file_name(const char *name, uint32_t inode_no, void *context) {
  return !strcmp(name, (char *) context);
}

/* find_file_in_directory: Searches for a file in a directory.
   
   Parameters:
     volume: Pointer to volume.
     inode: Pointer to inode structure for the directory.
     name: NULL-terminated string for the name of the file. The file
           name must match this name exactly, including case.
     buffer: If the file is found, and this pointer is set to a
             non-NULL value, this buffer is set to the directory entry
             of the file. If the pointer is NULL, nothing is saved. If
             the file is not found, the value of this buffer is
             unspecified.

   Returns:
     If the file exists in the directory, returns the inode number
     associated to the file. If the file does not exist, or the inode
     is not a directory, or there is an error reading the directory
     data, returns 0 (zero).
 */
uint32_t find_file_in_directory(volume_t *volume, inode_t *inode, const char *name, dir_entry_t *buffer) {
  
  return follow_directory_entries(volume, inode, (char *) name, buffer, compare_file_name);
}

/* find_file_from_path: Searches for a file based on its full path.
   
   Parameters:
     volume: Pointer to volume.
     path: NULL-terminated string for the full absolute path of the
           file. Must start with '/' character. Path components
           (subdirectories) must be delimited by '/'. The root
           directory can be obtained with the string "/".
     dest_inode: If the file is found, and this pointer is set to a
                 non-NULL value, this buffer is set to the inode of
                 the file. If the pointer is NULL, nothing is
                 saved. If the file is not found, the value of this
                 buffer is unspecified.

   Returns:
     If the file exists, returns the inode number associated to the
     file. If the file does not exist, or there is an error reading
     any directory or inode in the path, returns 0 (zero).
 */
uint32_t find_file_from_path(volume_t *volume, const char *path, inode_t *dest_inode) {
    //split by /
    //each pathSplit is a dir name or file entry 
    
    //start path finding from the root
    uint32_t inoNum = EXT2_ROOT_INO;

    //strok disturbs const char path
    //just root
    if(strcmp(path, "/") == 0){
        if(dest_inode != NULL){
            if(read_inode(volume, inoNum, dest_inode) != 0){
                return 0;
            }else{
                return inoNum;
            }
        }
    }

    char* pathCopy = strdup(path);
    char * pathSplit = strtok(pathCopy, "/");
    inode_t interimInode;
    read_inode(volume, inoNum, &interimInode);

    //loop while there exists a path component
    while(pathSplit != NULL){
        //get the inode number using find_file_in_directory of the current pathSplit
        //if 0 then something went wrong 
        inoNum = find_file_in_directory(volume, &interimInode, pathSplit, NULL);
        if(inoNum == 0){
            return 0;
        }
        read_inode(volume, inoNum, &interimInode);
        pathSplit = strtok(NULL, "/");
    }

    if(inoNum == 0){
        return 0;
    }else{
        if(dest_inode){
            read_inode(volume, inoNum, dest_inode);
        }
        return inoNum;
    }
}

/*
./ext2fs -d -f -s -o use_ino /tmp/y7m1b/ ~cs-313/EXTimages/SparseSmall.img
~cs-313/EXTimages/extList.csh /tmp/y7m1b/ > listing.txt
diff -b listing.txt  ~cs-313/EXTimages/DiskListings/SparseSmall_Listing.txt
fusermount -u /tmp/y7m1b
~cs-313/EXTimages/runall.csh /tmp/y7m1b
*/