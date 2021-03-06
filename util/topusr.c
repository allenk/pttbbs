/* 使用者 上站記錄/文章篇數 排行榜 */
#define _UTIL_C_
#include "bbs.h"

#define TYPE_POST       0
#define TYPE_MONEY      1

#define TYPE_COUNT	2

#define REAL_INFO
struct manrec
{
    char userid[IDLEN + 1];
    char nickname[23];
    int values[TYPE_COUNT];
};
typedef struct manrec manrec;
struct manrec *allman[TYPE_COUNT];

userec_t aman;
manrec theman;
int num;
FILE *fp;


void
 top(type)
{
    static char *str_type[TYPE_COUNT] =
    {"發表次數", " 大富翁 "};
    int i, j, rows = (num + 1) / 2;
    char buf1[80], buf2[80];

    assert(type < TYPE_COUNT);
    if (type != TYPE_COUNT-1)
	fprintf(fp, "\n\n");

    fprintf(fp, "\
[1;36m  ╭─────╮           [%dm    %8.8s排行榜    [36;40m               ╭─────╮[m\n\
[1;36m  名次─代號───暱稱──────數目──名次─代號───暱稱──────數目[m\
", type + 44, str_type[type]);
    for (i = 0; i < rows; i++)
    {
        char ch=' ';
        int value;

        if(allman[type][i].values[type] > 1000000000)
		{ value=allman[type][i].values[type]/1000000; ch='M';}
        else if(allman[type][i].values[type] > 1000000)
		{ value=allman[type][i].values[type]/1000; ch='K';}
        else {value=allman[type][i].values[type]; ch=' ';}
	sprintf(buf1, "[%2d] %-11.11s%-16.16s%5d%c",
		i + 1, allman[type][i].userid, allman[type][i].nickname,
	        value, ch);
	j = i + rows;
        if(allman[type][j].values[type] > 1000000000)
		{ value=allman[type][j].values[type]/1000000; ch='M';}
        else if(allman[type][j].values[type] > 1000000)
		{ value=allman[type][j].values[type]/1000; ch='K';}
        else {value=allman[type][j].values[type]; ch=' ';}

	sprintf(buf2, "[%2d] %-11.11s%-16.16s%4d%c",
		j + 1, allman[type][j].userid, allman[type][j].nickname,
		value, ch);
	if (i < 3)
	    fprintf(fp, "\n [1;%dm%-40s[0;37m%s", 31 + i, buf1, buf2);
	else
	    fprintf(fp, "\n %-40s%s", buf1, buf2);
    }
}


#ifdef  HAVE_TIN
int
 post_in_tin(char *name)
{
    char buf[256];
    FILE *fh;
    int counter = 0;

    sprintf(buf, "%s/home/%c/%s/.tin/posted", home_path, name[0], name);
    fh = fopen(buf, "r");
    if (fh == NULL)
	return 0;
    else
    {
	while (fgets(buf, 255, fh) != NULL)
	    counter++;
	fclose(fh);
	return counter;
    }
}
#endif				/* HAVE_TIN */

int main(int argc, char **argv)
{
    int i, j;

    if (argc < 3)
    {
	printf("Usage: %s <num_top> <out-file>\n", argv[0]);
	exit(1);
    }

    num = atoi(argv[1]);
    if (num == 0)
	num = 30;

    attach_SHM();
    if(passwd_init())
    {
	printf("Sorry, the data is not ready.\n");
	exit(0);
    }
    for(i=0; i<TYPE_COUNT; i++)
    {
	allman[i]=malloc(sizeof(manrec) * num);
	memset(allman[i],0,sizeof(manrec) * num);    
    }
    for(j = 1; j <= MAX_USERS; j++) {
	passwd_query(j, &aman);
        aman.userid[IDLEN]=0;
        aman.nickname[22]=0;
	if((aman.userlevel & PERM_NOTOP) || !aman.userid[0] || 
	   !is_validuserid(aman.userid) || 
	   strchr(aman.userid, '.'))
	{
	    continue;
	}
	else {
	    strcpy(theman.userid, aman.userid);
	    strcpy(theman.nickname, aman.nickname);
            theman.values[TYPE_POST] =  aman.numposts;
            theman.values[TYPE_MONEY] = aman.money;
            for(i=0; i<TYPE_COUNT; i++)
	     {
	        int k,l;
                for(k=num-1; k>=0 && allman[i][k].values[i]<theman.values[i];
			 k--);
	        k++;
	        if(k<num)
	                {
		          for(l=num-1; l>k; l--)
				  memcpy(&allman[i][l], &allman[i][l-1], 
					  sizeof(manrec));
	                  memcpy(&allman[i][k], &theman, sizeof(manrec));	
		        } 
	     }
	}
    }
    

    if ((fp = fopen(argv[2], "w")) == NULL)
    {
	printf("cann't open topusr\n");
	return 0;
    }

    top(TYPE_MONEY);
    top(TYPE_POST);

    fclose(fp);
    return 0;
}
