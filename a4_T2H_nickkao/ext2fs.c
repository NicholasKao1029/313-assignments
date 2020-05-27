#include "ext2.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

/* The FUSE version has to be defined before any call to relevant
   includes related to FUSE. */
#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 26
#endif
#include <fuse.h>

static void *ext2_init(struct fuse_conn_info *conn);
static void ext2_destroy(void *private_data);
static int ext2_getattr(const char *path, struct stat *stbuf);
static int ext2_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			off_t offset, struct fuse_file_info *fi);
static int ext2_open(const char *path, struct fuse_file_info *fi);
static int ext2_release(const char *path, struct fuse_file_info *fi);
static int ext2_read(const char *path, char *buf, size_t size, off_t offset,
		     struct fuse_file_info *fi);
static int ext2_readlink(const char *path, char *buf, size_t size);

static volume_t *volume;

static const struct fuse_operations ext2_operations = {
  .init = ext2_init,
  .destroy = ext2_destroy,
  .open = ext2_open,
  .read = ext2_read,
  .release = ext2_release,
  .getattr = ext2_getattr,
  .readdir = ext2_readdir,
  .readlink = ext2_readlink,
};

int main(int argc, char *argv[]) {
  
  if (argc < 2) {
    fprintf(stderr, "Usage: ./ext2fs volumefile\n");
    exit (1);
  }

  char *volumefile = argv[--argc];
  volume = open_volume_file(volumefile);
  argv[argc] = NULL;
  
  if (!volume) {
    fprintf(stderr, "Invalid volume file: '%s'.\n", volumefile);
    exit(1);
  }
  
  fuse_main(argc, argv, &ext2_operations, volume);
  
  return 0;
}

/* ext2_init: Function called when the FUSE file system is mounted.
 */
static void *ext2_init(struct fuse_conn_info *conn) {
  
  printf("init()\n");
  
  return NULL;
}

/* ext2_destroy: Function called before the FUSE file system is
   unmounted.
 */
static void ext2_destroy(void *private_data) {
  
  printf("destroy()\n");
  
  close_volume_file(volume);
}

/* ext2_getattr: Function called when a process requests the metadata
   of a file. Metadata includes the file type, size, and
   creation/modification dates, among others (check man 2
   fstat). Typically FUSE will call this function before most other
   operations, for the file and all the components of the path.
   
   Parameters:
     path: Path of the file whose metadata is requested.
     stbuf: Pointer to a struct stat where metadata must be stored.
   Returns:
     In case of success, returns 0, and fills the data in stbuf with
     information about the file. If the file does not exist, should
     return -ENOENT.
 */
static int ext2_getattr(const char *path, struct stat *stbuf) {
  inode_t inode;
  uint32_t inumber;
  
  inumber = find_file_from_path(volume, path,  &inode);
  if (inumber <= 0)
    return -ENOENT;

  memset(stbuf, 0, sizeof(struct stat));
  /* Not filling in dev or rdef. */
  stbuf->st_ino = inumber;
  stbuf->st_mode = inode.i_mode;
  stbuf->st_nlink = inode.i_links_count;
  stbuf->st_uid = (inode.l_i_uid_high << 16) + inode.i_uid;
  stbuf->st_gid = inode.i_gid;
  stbuf->st_size = inode_file_size(volume, &inode);
  stbuf->st_blksize = volume->block_size;
  stbuf->st_blocks = inode.i_blocks;
  stbuf->st_atime = inode.i_atime;
  stbuf->st_mtime = inode.i_mtime;
  stbuf->st_ctime = inode.i_ctime;
  return 0;
}

/* ext2_readdir: Function called when a process requests the listing
   of a directory.
   
   Parameters:
     path: Path of the directory whose listing is requested.
     buf: Pointer that must be passed as first parameter to filler
          function.
     filler: Pointer to a function that must be called for every entry
             in the directory.  Will accept four parameters, in this
             order: buf (previous parameter), the filename for the
             entry as a string, a pointer to a struct stat containing
             the metadata of the file (optional, may be passed NULL),
             and an offset for the next call to ext2_readdir
             (optional, may be passed 0).
     offset: Will usually be 0. If a previous call to filler for the
             same path passed a non-zero value as the offset, this
             function will be called again with the provided value as
             the offset parameter. Optional.
     fi: Not used in this implementation of readdir.

   Returns:
     In case of success, returns 0, and calls the filler function for
     each entry in the provided directory. If the directory doesn't
     exist, returns -ENOENT.
 */

/* 
 * Helper function: we pass it to follow_directory_entry and that, in turn,
 * will call the function that the caller to ext_readdir wants us to call.
 */
struct readdir_context {
  fuse_fill_dir_t func;	// Function that readdir helper will call
  void *buf;		// Allows pass through
};

int _readdir_helper(const char *name, uint32_t inode_no, void *context) {
  struct readdir_context *cxt = context;
  return (cxt->func(cxt->buf, name, NULL, 0));
}

static int ext2_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi) {

  inode_t inode;
  uint32_t dir_inumber;
  struct readdir_context context;

  dir_inumber = find_file_from_path(volume, path,  &inode);
  if (dir_inumber <= 0)
    return -ENOENT;

  context.buf = buf;
  context.func = filler;
  (void)follow_directory_entries(volume, &inode, &context, NULL, _readdir_helper);

  return 0;
}

/* ext2_open: Function called when a process opens a file in the file
   system.
   
   Parameters:
     path: Path of the file being opened.
     fi: Data structure containing information about the file being
         opened. Some useful fields include:
	 flags: Flags for opening the file. Check 'man 2 open' for
                information about which flags are available.
	 fh: File handle. The value you set to this handle will be
             passed unmodified to all other file operations involving
             the same file.
   Returns:
     In case of success, returns 0. In case of error, may return one
     of these error codes:
       -ENOENT: If the file does not exist;
       -EISDIR: If the path corresponds to a directory;
       -EACCES: If the open operation was for writing, and the file is
                read-only.
 */
static int ext2_open(const char *path, struct fuse_file_info *fi) {
  
  if (fi->flags & O_WRONLY || fi->flags & O_RDWR)
    return -EACCES;

  inode_t inode;
  uint32_t inumber;
  
  inumber = find_file_from_path(volume, path,  &inode);
  if (inumber <= 0)
    return -ENOENT;

  if (S_ISDIR(inode.i_mode))
    return -EISDIR;

  fi->fh = (uintptr_t) malloc(sizeof(inode_t));
  if (fi->fh == (uintptr_t) NULL)
    return -ENOMEM;

  memcpy((void *)fi->fh, &inode, sizeof(inode_t));

  return 0;
}

/* ext2_release: Function called when a process closes a file in the
   file system. If the open file is shared between processes, this
   function is called when the file has been closed by all processes
   that share it.
   
   Parameters:
     path: Path of the file being closed.
     fi: Data structure containing information about the file being
         opened. This is the same structure used in ext2_open.
   Returns:
     In case of success, returns 0. There is no expected error case.
 */
static int ext2_release(const char *path, struct fuse_file_info *fi) {

  if (fi->fh != (uintptr_t)NULL)
    free((void *)fi->fh);
  return 0;
}

/* ext2_read: Function called when a process reads data from a file in
   the file system. This function stores, in array 'buf', up to 'size'
   bytes from the file, starting at offset 'offset'. It may store less
   than 'size' bytes only in case of error or if the file is smaller
   than size+offset.
   
   Parameters:
     path: Path of the open file.
     buf: Pointer where data is expected to be stored.
     size: Maximum number of bytes to be read from the file.
     offset: Byte offset of the first byte to be read from the file.
     fi: Data structure containing information about the file being
         opened. This is the same structure used in ext2_open.
   Returns:
     In case of success, returns the number of bytes actually read
     from the file--which may be smaller than size, or even zero, if
     (and only if) offset+size is beyond the end of the file. In case
     of error, may return one of these error codes (not all of them
     are required):
       -ENOENT: If the file does not exist;
       -EISDIR: If the path corresponds to a directory;
       -EIO: If there was an I/O error trying to obtain the data.
 */
static int ext2_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi) {

  if(fi == NULL || fi->fh == (uintptr_t)NULL) {
    int ret = ext2_open(path, fi);
    if (ret < 0)
      return ret;
  }

  /* The file in the handle cannot be a directory as open already checked for that. */
  return (read_file_content(volume, (inode_t *)fi->fh, offset, size, buf));
}

/* ext2_readlink: Function called when FUSE needs to obtain the target
   of a symbolic link. The target is stored in buffer 'buf', which
   stores up to 'size' bytes, as a NULL-terminated string. If the
   target is too long to fit in the buffer, the target should be
   truncated.
   
   Parameters:
     path: Path of the symbolic link file.
     buf: Pointer where symbolic link target is expected to be stored.
     size: Maximum number of bytes to be stored into the buffer,
           including the NULL byte.
   Returns:
     In case of success, returns 0 (zero). In case of error, may
     return one of these error codes (not all of them are required):
       -ENOENT: If the file does not exist;
       -EINVAL: If the path does not correspond to a symbolic link;
       -EIO: If there was an I/O error trying to obtain the data.
 */
static int ext2_readlink(const char *path, char *buf, size_t size) {
  inode_t inode;
  uint32_t inumber;
  
  inumber = find_file_from_path(volume, path,  &inode);
  if (inumber <= 0)
    return -ENOENT;

  if (!S_ISLNK(inode.i_mode))
    return -EINVAL;

  /* Check if symlink is stored in inode. */
  int len = inode_file_size(volume, &inode);
  size_t in_ino_max = sizeof(inode.i_symlink_target);

  /*
   * We are going to use size to tell us how many bytes we are actually going to
   * place in target. That will either be len (the actual size of the target), or
   * if that's too big, then we'll just copy size bytes. So we'll use size to hold
   * the actual number of bytes we are going to copy.
   */
  if (len < size)
    size = len;
	
  if (len > in_ino_max) {
    /* Symlink target is stored as file data, so we have to read it. */
    size = read_file_content(volume, &inode, 0, size, buf);
    if (size <= 0)
      return -EIO;
  } else {
    /* Get target name from inode. */
    memcpy(buf, inode.i_symlink_target, size);
  }
  buf[size] = 0;
  return 0;
}
