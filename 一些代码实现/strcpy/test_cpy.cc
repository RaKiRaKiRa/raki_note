/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-12-15 00:12
 * Last modified : 2019-12-15 00:12
 * Filename      : 
 * Description   : 
 **********************************************************/
#include "strcpy.h"
#include "stdio.h"
int main()
{
    char dst[200], src[200];
    while(1)
    {
        scanf("%s", &src);
        myStrcpy(dst, src);
        printf("%s\n", dst);
        myStrcpy(dst + 3, dst);
        printf("%s\n", dst);
    }
}
