#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
// Command for Compiling: gcc -std=gnu99 -lm [source filename] -o [executable]
// Usage                : [executable] [xyzfile1] [xyzfile2]...
// The program check the starting and ending lines to obtain the region of any given xyz files
// resolution is recorded by seeking the first changing value of x and y  (first and second column)
// Data within input files can be sperated either in comma (',') or spaces (' ')
// The program produce an adjacency matrix indicating wehther a xyz file can be a submap of the others files after reading them

#define sealevel 0
#define BUFFER 80
#define EARTH_R 6371
#define PI_F 3.1415926535897932384626433832795f

typedef struct MAP *MAPpointer;
typedef struct MAP{
    const char *name;
    float region[4];
    float compare;
    MAPpointer NEXT;
}MAP;

int main(int argc, char const *argv[]) {
    FILE *xyzfile;
    void str2xyz(FILE *,double *);
    long size;
    char seekEOL;
    double delta;
    int index;
    char *unit[] = {"Byte","KByte","MByte","GByte"};
    double xyz[2][2]={0};
    double tmp[2][3]={0};
    double res[3][3]={0};
    //LIST
    MAPpointer MAPs[2]={NULL,NULL}, tmpmap[2]={NULL,NULL};
    float area;
    float resolution;

    void insert(MAPpointer *,MAPpointer);
    void printlist(MAPpointer, int **,int );
    for (size_t i = 0; i < 2; i++) {
        tmpmap[i] = (MAPpointer) malloc(sizeof(MAP));
    }
    //Submaps
    int** check_submaps(MAPpointer, int);

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
            // format checking, initializing
            str2xyz(xyzfile,NULL);
            for (size_t i = 0; i < 3; i++) {
                for (size_t j = 0; j < 3; j++) {
                    res[i][j] = 0;
                    xyz[i % 2][j % 2] = 0;
                    tmp[i % 2][j] = 0;
                }
            }
            // get bonding box
            str2xyz(xyzfile,*tmp); // first line of the file
            fseek(xyzfile,-2,SEEK_END);
            size = ftell(xyzfile)+2;//get file size
            seekEOL = '\0';
            while (seekEOL != '\n') {
                fseek(xyzfile,-2,SEEK_CUR);
                seekEOL = fgetc(xyzfile);//seek back to starting character of the last line
            }
            str2xyz(xyzfile,*(tmp+1)); // last line of the file
            for (int i = 0; i < 2; i++) {// x and y axis
                if (tmp[0][i] >= tmp[1][i]) { // stored in ascending order
                    xyz[0][i] = tmp[0][i];
                    xyz[1][i] = tmp[1][i];
                }else{
                    xyz[0][i] = tmp[1][i];
                    xyz[1][i] = tmp[0][i];
                }
            }
            rewind(xyzfile);
            // get resolution
            str2xyz(xyzfile,*res);
            while (res[2][0]*res[2][1] == 0) { // haven't been recorded
                str2xyz(xyzfile,*(res+1));
                for (int i = 0; i < 2; i++) { // x and y axis
                    delta = fabs(res[1][i] - res[0][i]);
                    if (res[2][i] == 0. && delta != 0.) {// first changing value
                        res[2][i] = delta;
                    }
                }
                if (ftell(xyzfile) >= size/2) {
                    printf("!!can't get resolution\n");
                    break;
                }
            }
            rewind(xyzfile);

            // check global or local (lon-lat or meters)
            if ( ((fabs(xyz[1][0]) <= 180.0 && fabs(xyz[0][0]) <= 180.0) || (fabs(xyz[1][0]) >= 0.0 && fabs(xyz[0][0]) <= 360.0)) &&
                   fabs(xyz[0][1]) <= 90.0 &&
                   fabs(xyz[1][1]) <= 90.0 ){
                res[2][1] = 2.0*EARTH_R*PI_F*(res[2][1]/360.0);
                res[2][0] = 2.0*EARTH_R*cos((xyz[1][1] + xyz[0][1])/360.0*PI_F)*PI_F*(res[2][0]/360.0);

            }

            // order of size of file
            index = 0;
            if (size/1e3 > 1) {
                index = 1;  // KB
                if (size/1e6 > 1) {
                    index = 2; // MB
                    if (size/1e9 > 1) {
                        index = 3; // GB
                    }
                }
            }
            size = size/(int)pow(10,index*3);
            printf("\t size: %ld %s\n",size,unit[index]);
            printf("\t Lontitude: %8.4lf %8.4lf/ %9.7lf\n",xyz[1][0], xyz[0][0],res[2][0] );
            printf("\t Latitude : %8.4lf %8.4lf/ %9.7lf\n",xyz[1][1], xyz[0][1],res[2][1] );

            //Store in the list
            resolution = (float) (res[2][0]+res[2][1])/2;
            area = (float) (xyz[0][0]-xyz[1][0])*(xyz[0][1]-xyz[1][1]);
            for (size_t j = 0; j < 2; j++) {
                tmpmap[j]->name = *(argv+fileno);
                for (size_t i = 0; i < 4; i++) {
                    tmpmap[j]->region[i] = xyz[ i<2 ? 1:0 ][ (i) % 2];
                }
                tmpmap[j]->compare = j%2 == 0 ? area:resolution;
                tmpmap[j]->NEXT = NULL;
            }
            //Insertion sort of area & resolution
            insert(MAPs, *tmpmap);
            insert(MAPs+1,*(tmpmap+1));
        }else{
            printf("Could not find %s\n",argv[fileno]);
            continue;
        }
        fclose(xyzfile);
    }
    printf("#####==||== file name ====||== area ==||============= region =============||== is submap of  ==================\n");
    printlist(*MAPs,check_submaps(*MAPs,argc-1),argc-1 );
    printf("#####==||== file name ====||resolution||============= region =============||== is submap of  ==================\n");
    printlist(*(MAPs+1),check_submaps(*(MAPs+1),argc-1),argc-1 );
    return 0;
}

void str2xyz(FILE *infile, double *outarr){
    // convert string of the xyz file into a float array
    static int SWITCH=0;
    char instr[BUFFER];
    long file_loc;

    if (!outarr) {
        file_loc = ftell(infile);
        fgets(instr,BUFFER,infile);
        if (strchr(instr,' ') || strchr(instr,'\t')) {
            SWITCH = 0;
            printf("\t Format: general xyz file\n");
        }else if (strchr(instr,',')) {
            SWITCH = 1;
            printf("\t Format: xyz file in csv form\n");
        }
        fseek(infile,file_loc,SEEK_SET);
    }else{
        switch (SWITCH) {
            case 0:
                fscanf(infile,"%lf %lf %lf\n",outarr,outarr+1,outarr+2);
                break;
            case 1:
                fscanf(infile,"%lf,%lf,%lf\n",outarr,outarr+1,outarr+2);
                break;
        }
    }
}

void insert(MAPpointer *maps,MAPpointer compare ){
    MAPpointer tmp;
    MAPpointer insertptr=NULL;

    tmp = (MAPpointer)malloc(sizeof(MAP));
    for (size_t i = 0; i < 4; i++) {
        tmp->region[i] = compare->region[i];
    }
    tmp->name = compare->name;
    tmp->compare = compare->compare;
    tmp->NEXT = NULL;

    // insertion sort
    if (!(*maps)){//empty list
        *maps = tmp;
    }else{//nonempty list
        insertptr = *maps;
        while (insertptr->NEXT && insertptr->NEXT->compare > tmp->compare) {
            insertptr=insertptr->NEXT;
        }
        if (insertptr == *maps) {//first one
            if ((*maps)->compare <= tmp->compare) {
                tmp->NEXT = *maps;
                *maps = tmp;
            }else{
                tmp->NEXT = (*maps)->NEXT;
                (*maps)->NEXT = tmp;
            }
        }else{
            tmp->NEXT = insertptr->NEXT;
            insertptr->NEXT = tmp;
        }
    }
}

int** check_submaps(MAPpointer print,int num){
    // return a relationship matrix showing if the map is a submap of the others maps
    int **is_submap;
    int numcheck=0, numget=0;
    MAPpointer get, check;
    get = print;
    check = print;

    is_submap = (int **) malloc((num)*sizeof(int *));
    for (size_t i = 0; i < num; i++) {
        *(is_submap+i) =(int *) calloc(num, sizeof(int));
    }

    for (get = print; get; get = get->NEXT) {
        for (check = print; check; check = check->NEXT) {
            is_submap[numget][numcheck] =
            get->region[0] >= check->region[0] &&
            get->region[1] >= check->region[1] &&
            get->region[2] <= check->region[2] &&
            get->region[3] <= check->region[3] &&
            get->compare < check->compare ? 1:0;

            numcheck++;
        }
        numget++;
        numcheck=0;
    }
    return is_submap;
}

void printlist(MAPpointer print, int **submaps, int num) {
    // print the sorted results
    size_t index=0;
    int name_len;
    while (print) {
        name_len = strlen(print->name);
        if (name_len > 15) {//print name
            printf("NO.%2d ... %15s :",index+1, print->name+name_len-15);
        }else{
            printf("NO.%2d     %15s :",index+1,print->name);
        }

        printf("%10.3e ||", print->compare);
        for (size_t i = 0; i < 4; i++) {
            printf("%7.2f ",print->region[i]);
        }
        printf("  ||  No");
        for (size_t i = 0; i < num; i++) {
            if (*(*(submaps+index)+i) == 1) {
                printf(".%d ",i+1);
            }
        }
        printf("\n");
        print = print->NEXT;
        index++;
    }
    printf("--------------------------------------------------------------------------------------------------------\n\n");
}
