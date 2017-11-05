#include "stdio.h"
#include "string.h"

char sourcefile[1000],line[5000];
FILE *source,*target;
long n,m,i,id,id2;
bool from_zero;

int main(int argc, char *argv[])
{
    n = 0; m = 0;
    from_zero = false;
    printf("Please enter the source file name: ");
    scanf("%s",&sourcefile);
    source = fopen(sourcefile,"r");
    target = fopen("./target.col","w");
    // ids
    while ((fscanf(source,"%s ",&line)) != EOF)
    {
        if (! strcmp(line,"id"))
        {
            fscanf(source,"%ld",&id);
            if (id == 0)
            {
                from_zero = true;
            }
            if (from_zero)
            {
                n = ((id+1) > n) ? (id+1) : (n);
            }
            else
            {
                n = (id > n) ? (id) : (n);
            }
        }
        if (! strcmp(line,"source"))
        {
            fscanf(source,"%ld",&id);
            // target overriding
            fscanf(source,"%s ",&line);
            fscanf(source,"%ld",&id2);
            if (from_zero)
            {
                fprintf(target,"e %ld %ld\n",id+1,id2+1);
            }
            else
            {
                fprintf(target,"e %ld %ld\n",id,id2);
            }
            m++;
        }
    }
    fprintf(target,"p edge %ld %ld\n",n,m);
    fclose(target);
    fclose(source);
    return 0;
}
