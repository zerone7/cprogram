#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define MYFILE  "/home/zero/file"
#define MEMSIZE pagesize*3

int main(void)
{
    int fd, i, pagesize, offset;
    char *smem;

    offset = 0;
    pagesize = sysconf(_SC_PAGESIZE);
    printf("pagesize is %d\n", pagesize);

    /* create file, file size = pagesize+1 */
    fd = open(MYFILE, O_CREAT | O_RDWR | O_TRUNC, 00777);
    lseek(fd, pagesize, SEEK_SET);
    write(fd, "", 1);

    /* mmap file, mapsize = pagesize*3 */
    smem = mmap(NULL, MEMSIZE, PROT_READ | PROT_WRITE, \
            MAP_SHARED, fd, offset);
    close(fd);

    strcpy(smem, "hello world");
 
    /* 0 ~ pagesize+1: file content, address mapped, page table entry created
     * pagesize+2 ~ pagesize*2: address mapped, page table entry created
     * pagesize*2+1 ~ pagesize*3: address mapped, page table entry not created
     * pagesize*3+1 ~ ... : address not mapped, page table entry not created
     */
    printf("Page2 first byte\n");
    *(smem + pagesize) = 'a'; //access page2 first byte: OK
    printf("Page2 last byte\n");
    *(smem + pagesize*2-1) = 'a'; //access page2 last byte: OK
    printf("Page3 first byte\n");
    *(smem + pagesize*2) = 'a'; //access page3 first byte: bus error;
    printf("Page8 first byte\n");
    *(smem + pagesize*7) = 'a'; //access page8 first byte: segment fault

    munmap(smem, MEMSIZE);

    return 0;
}
