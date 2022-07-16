# A system bus

## Values
CODE | RETURN VALUE
--- | ---
DIR_CREATED | 200000
DIR_CREATED_ERROR | 200001
DIR_EXIST | 200002
DELETE_DIR | 200011
MOVE_DIR | 200003
COPY_DIR | 200004
OTHER_DIR_ERROR | 200005
FILE_CREATED | 200006
FILE_EXIST | 200007
DELETE_FILE | 200012
MOVE_FILE | 200008
COPY_FILE | 200009
OTHER_FILE_ERROR | 200010
NO_IMPLEMENTED_ERROR | 200110

## Functions
Arguments to functions pass via the stack.

```c++
int createDirectory(std::string, int));
int deleteDirectory(std::string);
int moveDirectory(std::string, std::string);
int copyDirectory(std::string, std::string);
int createFile(std::string, uint8_t*, int);
int deleteFile(std::string);
int moveFile(std::string, std::string);
int copyFile(std::string, std::string);
```
## OS support
FUNC | CMD | CODE | Windows | Linux | MacOS
--- | --- | --- | --- | --- | ---
createDirectory | sysdircr | 1 | YES | YES | NO |
deleteDirectory | sysdirdel | 2 | YES | YES | NO |
moveDirectory | sysdirmv | 3 | YES | NO | NO |
copyDirectory | sysdircp | 4 | YES | NO | NO |
createFile | sysfilecr | 5 | YES | YES | NO |
deleteFile | sysfiledel | 6 | YES | YES | NO |
moveFile | sysfilemv | 7 | YES | YES | NO |
copyFile | sysfilecp | 8 | YES | YES | NO |