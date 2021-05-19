#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define IN_FILE "./test_file.mp4"
#define OUT_FILE "./test_file_out.mp4"

int main(int argc, char** argv)
{
    if (0 == access(IN_FILE, F_OK)) {
        fprintf(stderr, "exist\n");  
        int fd = open(IN_FILE, O_RDONLY);
        if (fd > 0) {
            int want_len = 1*1024*1024;
            char buf[1024];
            int read_len = 0;
            int less_len = 0;
            int idx_len = 0;

            int o_fd = open(OUT_FILE, O_CREAT|O_RDWR, 0755);

            while (idx_len < want_len) {
                less_len = want_len - idx_len;
                read_len = read(fd, buf, (less_len>1024)?1024:less_len); 
                if (read_len > 0) {
                    //fprintf(stderr, "op: %d cur: %d\n", want_len, idx_len);
                    write(o_fd, buf, read_len);
                    idx_len += read_len; 
                }
            }
            close(o_fd);
        }
        close(fd);
    }
    return 0;
}
