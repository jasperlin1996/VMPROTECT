#include "../include/drivers/linuxsysbus.hpp"

#ifdef _LINUX_DEV_ENVIRONMENT
    int UNIX::createDirectory(std::string dirName, int dirMode)
    {
        struct stat st = {0};
        if (stat(dirName.c_str(), &st) == -1) // checking if the directory exists
        {
            if(!mkdir(dirName.c_str(), getMode(dirMode))) return DIR_CREATED_ERROR;
            return DIR_CREATED;
        }
        else return DIR_EXIST;
    }

    int UNIX::deleteDirectory(std::string dirName)
    {
        struct stat st = {0};
        if(stat(dirName.c_str(), &st) == 0)
        {
            if(!rmdir(dirName.c_str())) return DELETE_DIR;
            else return OTHER_DIR_ERROR;
        }
        else return OTHER_DIR_ERROR;
    }

    int UNIX::moveDirectory(std::string dirSource, std::string dirDest)
    {
        // No implemented yet
        return NO_IMPLEMENTED_ERROR;
    }

    int UNIX::copyDirectory(std::string dirSource, std::string dirDest)
    {
        // No implemented yet
        return NO_IMPLEMENTED_ERROR;
    }

    int UNIX::deleteFile(std::string filePath)
    {
        if(!remove(filePath.c_str())) return DELETE_FILE;
        else return OTHER_FILE_ERROR;
    }

    int UNIX::moveFile(std::string sourcePath, std::string destPath)
    {
        if(!rename(sourcePath.c_str(), destPath.c_str())) return MOVE_FILE;
        else return OTHER_FILE_ERROR;
    }

    int UNIX::copyFile(std::string sourcePath, std::string destPath)
    {
        const char *to = destPath.c_str();
        const char *from = sourcePath.c_str();
        int fd_to, fd_from;
        char buf[4096];
        ssize_t nread;

        fd_from = open(from, O_RDONLY);
        if (fd_from < 0) return OTHER_FILE_ERROR;

        fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
        if (fd_to < 0)
        {
            close(fd_from);
            if (fd_to >= 0) close(fd_to);
            return OTHER_FILE_ERROR;
        }

        while (nread = read(fd_from, buf, sizeof buf), nread > 0)
        {
            char *out_ptr = buf;
            ssize_t nwritten;

            do {
                nwritten = write(fd_to, out_ptr, nread);

                if (nwritten >= 0)
                {
                    nread -= nwritten;
                    out_ptr += nwritten;
                }
                else if (errno != EINTR)
                {
                    close(fd_from);
                    if (fd_to >= 0) close(fd_to);
                    return OTHER_FILE_ERROR;
                }
            } while (nread > 0);
        }

        if (nread == 0)
        {
            if (close(fd_to) < 0)
            {
                fd_to = -1;
                close(fd_from);
                if (fd_to >= 0) close(fd_to);
                return OTHER_FILE_ERROR;
            }
            close(fd_from);
            return COPY_FILE;
        }

        close(fd_from);
        if (fd_to >= 0) close(fd_to);
        return OTHER_FILE_ERROR;
    }

    mode_t UNIX::getMode(int m)
    {
        mode_t mode = 0;
        std::string s = std::to_string(m);
        // 1111111111 == rwxrwxrwx
        // 1000000000 == ---------
        const char *perm = s.c_str();

        if (perm[1] == '1') mode |= 0400;
        if (perm[2] == '1') mode |= 0200;
        if (perm[3] == '1') mode |= 0100;
        if (perm[4] == '1') mode |= 0040;
        if (perm[5] == '1') mode |= 0020;
        if (perm[6] == '1') mode |= 0010;
        if (perm[7] == '1') mode |= 0004;
        if (perm[8] == '1') mode |= 0002;
        if (perm[9] == '1') mode |= 0001;

        return mode;
    }

#endif