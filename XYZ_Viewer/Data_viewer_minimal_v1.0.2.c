#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define sealevel 0

int main(int argc, char const *argv[]) {
    FILE *xyzfile;
    long size,linebuffer;
    double delta;
    int index;
    char *unit[] = {"Byte","KByte","MByte","GByte"};
    double xyz[2][2]={0};
    double tmp[3][2]={0};
    double res[3][3]={0};

    if (argc < 2) {
        printf("Please specify a '.xyz' file while input\n");
        return 1;
    }

    for (int fileno = 1; fileno < argc; fileno++){
        printf("\n%s:\n",argv[fileno]);
        //check if the input file is valid
        if (strstr(argv[fileno],".xyz") == NULL && strstr(argv[fileno],".XYZ") == NULL) {
            printf("Only accept '.xyz' file\n" );
            continue;
        }


        xyzfile = fopen(argv[fileno],"rt");
        if (xyzfile) {
            // Get region of the bonding box
            fscanf(xyzfile,"%lf %lf %lf\n",*tmp,*(tmp+1),*(tmp+2));
            linebuffer=ftell(xyzfile);
            fseek(xyzfile,-linebuffer,SEEK_END);
            fscanf(xyzfile,"%lf %lf %lf\n",*(tmp)+1,*(tmp+1)+1,*(tmp+2)+1);
            for (int i = 0; i < 2; i++) {
                if (tmp[i][0] > tmp[i][1]) {
                    xyz[i][0] = tmp[i][0];//max
                    xyz[i][1] = tmp[i][1];//min
                }else{
                    xyz[i][0] = tmp[i][1];
                    xyz[i][1] = tmp[i][0];
                }
            }


            // -----------------------------

            // Get resolution
            for (int i = 0; i < 2; i++) {
                //initialize
                res[i][2] = 0;
                fseek(xyzfile,0,SEEK_SET);
                fscanf(xyzfile,"%lf %lf %lf\n",*(res),*(res+1),*(res+2));
                while (res[i][2] == 0) {
                    int j = 1;
                    fscanf(xyzfile,"%lf %lf %lf\n",*(res)+(j%2),*(res+1)+(j%2),*(res+2)+(j%2));
                    delta = res[i][j%2] - res[i][(j-1)%2];
                    if (delta != 0) {
                        res[i][2] = fabs(delta);
                    }
                    j++;
                }
            }
            rewind(xyzfile);
            // --------------------------


            fseek(xyzfile,0,SEEK_END);
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
        size = size/(int)pow(10,index*3);
        printf("\tsize: %ld %s\n",size,unit[index]);
        printf("\t-----------------------------------------------------------------------\n");
        printf("\tbounding box:\n\t\t\t    min\t\t    max\t\t  resolution\n");
        printf("\tLontitude:\t%9.3lf\t%9.3lf\t%9.5f\n",xyz[0][1],xyz[0][0],res[0][2]);
        printf("\tLatitude:\t%9.3lf\t%9.3lf\t%9.5f\n",xyz[1][1],xyz[1][0],res[1][2]);
        printf("\t-----------------------------------------------------------------------\n");
        fclose(xyzfile);
    }
    return 0;
}
