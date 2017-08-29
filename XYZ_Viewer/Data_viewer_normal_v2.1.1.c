#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define sealevel 0

int main(int argc, char const *argv[]) {
    FILE *xyzfile;
    long size;
    int index;
    int countline=0;
    char *unit[] = {"Byte","KByte","MByte","GByte"};
    double xyz[3][2]={0};
    double tmp[3];
    double res[3][3]={0},delta;
    float level_hash[10]={0};
    char *level_str[] ={
         "( -inf,-4000]|"
        ,"(-4000,-3000]|"
        ,"(-3000,-2000]|"
        ,"(-2000,-1000]|"
        ,"(-1000,    0]|"
        ,"(    0, 1000]|"
        ,"( 1000, 2000]|"
        ,"( 2000, 3000]|"
        ,"( 3000, 4000]|"
        ,"( 4000, inf ]|"
    };
    float hash;
    void HIST(float *, char **);

    if (argc < 2) {
        printf("Please specify a '.xyz' file while input\n");
        return 1;
    }

    for (int fileno = 1; fileno < argc; fileno++){
        printf("\n%s:\n",argv[fileno]);
        if (strstr(argv[fileno],".xyz") == NULL && strstr(argv[fileno],".XYZ") == NULL) {
            printf("%s not available\n",argv[fileno]);
            printf("Only accept '.xyz' file\n" );
            continue;
        }


        xyzfile = fopen(argv[fileno],"rt");
        if (xyzfile) {
            //initialize for computing imformation
            fscanf(xyzfile,"%lf %lf %lf\n",*xyz,*(xyz+1),*(xyz+2)); //for selecting max x y z
            fscanf(xyzfile,"%lf %lf %lf\n",*(xyz)+1,*(xyz+1)+1,*(xyz+2)+1);//for selecting min x y z
            //resolution
            for (int i = 0; i < 3; i++) {
                res[i][0] = xyz[i][0];
                res[i][1] = xyz[i][1];
                res[i][2] = 1e20;
            }
            // height
            for (int i = 0; i < 10; i++) {
                level_hash[i]=0;
            }
            rewind(xyzfile);
            countline = 0;
            while (!feof(xyzfile)) {
                //min max
                fscanf(xyzfile,"%lf %lf %lf\n",tmp,tmp+1,tmp+2);
                for (int i = 0; i < 3; i++) {
                    if (tmp[i] > xyz[i][0] && !isnan(tmp[i])) {
                        xyz[i][0] = tmp[i]; // max
                    }else if (tmp[i] < xyz[i][1] && !isnan(tmp[i])){
                        xyz[i][1] = tmp[i]; // min
                    }
                }
                //resolution
                for (int i = 0; i < 3; i++) {
                    res[i][countline%2] = tmp[i];
                    delta = fabs(res[i][countline%2] - res[i][(countline-1)%2]);
                    if (res[i][2] > delta && delta != 0) {
                        res[i][2] = delta;
                    }
                }

                // Land Sea
                hash = ceil((*(tmp+2) + 4000.)/1000.);
                if (hash > 9) {
                    hash = 9;
                }else if (hash < 0){
                    hash = 0;
                }
                level_hash[(int) hash]++;
                countline++;
            }
            size = ftell(xyzfile);
        }else{
            printf("Could not find %s\n",argv[fileno]);
            return 2;
        }



        index = 0;
        if (size/1e3 > 1) {
            index = 1;
            if (size/1e6 > 1) {
                index = 2;
                if (size/1e9 > 1) {
                    index = 3;
                }
            }
        }
        size = size/(int) pow(10,index*3);
        printf("\tsize: %ld %s\n",size,unit[index]);
        printf("\tnumber of data: %d\n",countline );
        printf("\tresolution:\n\tLon: %12.9lf\n\tLat: %12.9lf\n\tHeight: %12.9lf\n",res[0][2],
                                                                                 res[1][2],
                                                                                 res[2][2]);
        printf("\t------------------------------------\n");
        printf("\tbounding box:\n\t\t    min\t\t    max\n\tLon:\t%9.3lf\t%9.3lf\n",xyz[0][1],xyz[0][0]);
        printf("\tLat:\t%9.3lf\t%9.3lf\n",xyz[1][1],xyz[1][0]);
        printf("\tHeight:\t%9.1lf\t%9.1lf\n",xyz[2][1],xyz[2][0]);
        printf("\t------------------------------------\n");

        for (int i = 0; i < 10; i++) {
            level_hash[i]/=countline;
        }
        printf("\tdata construction(height):\n");
        HIST(level_hash,level_str);
        fclose(xyzfile);
    }
    return 0;
}

void HIST(float *value, char **key) {
    int numsign = 0;
    for (int i = 9; i >= 0; i--) {
        printf("\t%s",*(key+i));
        numsign = (int) roundf(100*(*(value+i)));
        for (int j = 0; j < numsign; j++) {
            printf("=");
        }
        printf("[%5.2f]\n",100*(*(value+i)));
    }
}
