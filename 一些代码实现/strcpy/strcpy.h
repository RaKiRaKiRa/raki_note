/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-12-15 00:07
 * Last modified : 2019-12-15 00:13
 * Filename      : 
 * Description   : 
 **********************************************************/
#include <assert.h>
void myStrcpy(char* dst, const char* src)
{
	if(dst > src)
	{
		int len = 0;
	    const char* tmp = src;
	    while(*tmp != '\0')
	    {
	        ++tmp;
	        ++len;
	    }
	    char* res = dst;
		res += len;
		src += len;
		++len;
		while(len--)
		{
			*res-- = *src--;
		}
	}
	else
	{
		while((*dst++ = *src++) != '\0');
	}

    
}