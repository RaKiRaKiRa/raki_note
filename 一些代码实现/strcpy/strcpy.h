#include <assert.h>
char* myStrcpy(char* dst, const char* src)
{
    assert(dst && src);
    int len = 0;
    const char* tmp = src;
    while(tmp != '\0')
    {
        ++tmp;
        ++len;
    }
    tmp = dst;

    
}