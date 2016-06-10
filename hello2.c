

    /*
      FUSE: Filesystem in Userspace
      Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

      This program can be distributed under the terms of the GNU GPL.
      See the file COPYING.

      gcc -Wall filesystem.c `pkg-config fuse --cflags --libs` -o filesystem
      ./filesystem ~/Desktop/fuse -d
This file system has 10000 files.Each block is alotted 4096 bytes.Each block is considered a file.The super block is the first file and consists of data about the files.This file system also consists of a free block list.A free block list basically indicates which files have been written to and which files havent.The 26th file is the root directory.The files from 27 to 399 are written onto fusedata.1 and so on.The remaining files are written onto the free blocks in increments of 400.
    */

    #define FUSE_USE_VERSION 26
    #define _FILE_OFFSET_BITS  64

    #include <fuse.h>
    #include <stdio.h>
    #include <string.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <time.h>

    static const char *hello_str = "Hello World!\n";
    static const char *hello_path = "/hello";
//the following are the limits on the system
    static const int maxblocks = 10000;
    static const int fbs = 25;
    static const int maxfilesize = 1638400;
    static const int block_size = 4096;
    static const char fusedataConst[30] = "fusedata.";
    int directory[100];

//creating the structure for the super block
    struct sb_struct
    {
        int creationTime;
        int mounted;
        int devId;
        int freeStart;
        int freeEnd;
        int root;
        int maxBlocks;
    }sb;
//the init function calls the super block function and the free block function.
    void init()
    {
        init_sb();
        init_fb();
    }
//this function writes the following details into the superblock.i.e the fusedata.0 file.Everytime the file is mounted, the value of mounted gets incremented.
    void modifysb() //updating the superblock
    {
        FILE *f;
        f=fopen("fusedata.0","w+");
        fprintf(f,"%s%d%s%d%s%d%s%d%s%d%s%d%s%d","creationtime:\t",sb.creationTime,"mounted:\t",sb.mounted,"devId:\t",sb.devId,"freeStart:\t",sb.freeStart,"freeEnd:\t",sb.freeEnd,"root:\t",sb.root,"maxBlocks\t",sb.maxBlocks);
        fclose(f);
        sb.mounted++;
    }
//the following values are written to the super block 
    void init_sb()
    {
    sb.creationTime = (int)time(NULL);
    sb.mounted=1;
    sb.devId=20;
    sb.freeStart=1;
    sb.freeEnd=25;
    sb.root=26;
    sb.maxBlocks=10000;
    modifysb();
    }
//Every time a new file is created, the following data is defined.This data will be stored in the inode location referenced in the parent directory.
    struct file_inode{
    int size;
    int uid;
    int gid;
    int mode;
    int linkcount;
    int atime;
    int ctime;
    int mtime;
    int indirect;
    int location;
    };

    struct filename_to_inode_dict{
    char f_or_d;
    int DOT;
    int DOTDOT;
    struct file_inode inode_file; 
    }inode0 , inode1;
//It must store 26-10000 blocks of dictionaries.
    struct directory_struct{
    int size;
    int uid;
    int gid;
    int mode;
    int atime;
    int ctime;
    int mtime;
    int linkcount;
    struct filename_to_inode_dict file_to_inode[10000];
    }rd;


//the root directory is block number 26.the following data is written into it.the current directory and the parent directory are both 26.
    void RootDirectory()
    {
        char file_name[100];
        FILE *fp;
        fp=fopen("fusedata.26","w+");

        inode0.f_or_d = 'd';
        inode1.f_or_d = 'd';
        inode0.DOT = 26;
        inode1.DOT = 26;
            rd.size = 0;
            rd.uid = 1;
            rd.gid = 1;
            rd.mode = 16877;
            rd.atime = (int)time(NULL);
            rd.ctime = (int)time(NULL);
            rd.mtime = (int)time(NULL);
            rd.linkcount = 2;
            rd.file_to_inode[0]=inode0;
            rd.file_to_inode[1]=inode1;        
            fprintf(fp,"{size:%d,uid:%d,gid:%d,mode:%d,atime:%d,ctime:%d,mtime:%d,linkcount:%d,filename_to_inode_dict:{%c:.:%d,%c:..:%d}}",
            rd.size,rd.uid,rd.gid,rd.mode,rd.atime,rd.ctime,rd.mtime,rd.linkcount,rd.file_to_inode[0].f_or_d,rd.file_to_inode[0].DOT,rd.file_to_inode[1].f_or_d,rd.file_to_inode[1].DOTDOT);
            fclose(fp);
    }
//the fb list ranges from 1 to 25.These indicate which files have been written to and which files are free.Every time a file is written to, the free block list must get updated.Files 27 to 399 are written in fusedata.1;files 400 to 800 are written to fusedata.2 and so on,until 10,000.
    void init_fb(){

    int i,j=0; 
        char file_name[100];
        FILE *fp;
        for(i=1;i<=25;i++)
        {
            sprintf(file_name,"fusedata.%d",i);
            fp=fopen(file_name,"w+");
            if(i==1)
            {
                for(j=27;j<=399;j++)
                {
                    fprintf(fp,"%d",j);
                }
            }
            else
            {
                for(j=400*(i-1);j<400*i;j++)
                {
                    fprintf(fp,"%d",j);
                }
            }
            fclose(fp);
        }
    }

    //this function creates the 10,000 files and writes the digit '0',4096 times in the files fusedata.1 to fusedata.9999
    static void* filesys(struct fuse_conn_info *conn)
    {
    init_sb();
        char file_name[50];
        int i,j;
        
            for(i=1;i<10000;i++)
        {
        FILE *fp;
            sprintf(file_name,"fusedata.%d",i);
                if(i!=0)
                {
                fp=fopen(file_name,"w+");
                    for(j=1;j<=4096;j++)
                        {
                            fprintf(fp,"0");
                        }
                    }

        fclose(fp);
    }
        init_fb();
        RootDirectory();
        return 0;
    }
   



    
    /*
    Existng Hello.c functions:
    */
    static int hello_getattr(const char *path, struct stat *stbuf)
    {
        int res = 0;

        memset(stbuf, 0, sizeof(struct stat));
        if (strcmp(path, "/") == 0) {
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
        } else if (strcmp(path, hello_path) == 0) {
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;
            stbuf->st_size = strlen(hello_str);
        } else
            res = -ENOENT;

        return res;
    }

    static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi)
    {
        (void) offset;
        (void) fi;

        if (strcmp(path, "/") != 0)
            return -ENOENT;

        filler(buf, ".", NULL, 0);
        filler(buf, "..", NULL, 0);
        filler(buf, hello_path + 1, NULL, 0);

        return 0;
    }

    static int hello_open(const char *path, struct fuse_file_info *fi)
    {
        if (strcmp(path, hello_path) != 0)
            return -ENOENT;

        if ((fi->flags & 3) != O_RDONLY)
            return -EACCES;

        return 0;
    }

    static int hello_read(const char *path, char *buf, size_t size, off_t offset,
                  struct fuse_file_info *fi)
    {
        size_t len;
        (void) fi;
        if(strcmp(path, hello_path) != 0)
            return -ENOENT;

        len = strlen(hello_str);
        if (offset < len) {
            if (offset + size > len)
                size = len - offset;
            memcpy(buf, hello_str + offset, size);
        } else
            size = 0;

        return size;
    }

    static struct fuse_operations hello_oper = {
        .getattr    = hello_getattr,
        .readdir    = hello_readdir,
        .open        = hello_open,
        .read        = hello_read,
        .init       = filesys,
        //.mkdir      = mkdirfilesys,
    };

    int main(int argc, char *argv[])
    {
        return fuse_main(argc, argv, &hello_oper, NULL);
    }

        //Displaying filesystem.c.
