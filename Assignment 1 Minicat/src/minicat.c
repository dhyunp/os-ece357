#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
//manages error display
int printError(char *msg, char *file, char *detail)
{
    if(!detail)
    {
        fprintf(stderr,"Error: %s\n", msg);
    }
    fprintf(stderr,"Error: %s [%s] - %s \n", msg, file, detail);
    return -1;
}
//manages read/write operations
void doRDWR(int inFD, int outFD, char *input, char *output, char* buffer, int buffersize)
{
    int wrSize, wrBytes;
    while((wrSize = read(inFD, buffer, buffersize)) > 0)
    {
        if(wrSize == -1)
        {
            printError("Failed to read following file to buffer", input, strerror(errno));
        }else{
            while(1)
            {
                if((wrBytes = write(outFD, buffer, wrSize)) <=0)
                {
                    printError("Failed to write following file to output", output, strerror(errno));
                }else if(wrBytes != wrSize){
                    buffer += wrBytes;
                    wrSize -= wrBytes;
                }else{
                    break;
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    int outFD, option;
    int inFD = -1, bflag = 0, oflag = 0, bufsize = 0;
    char *outputName = 0;
//gather option arguments from stdin
    while( (option=getopt(argc, argv, "b:o:")) != -1)
    {
        switch (option)
        {
            case 'b':
                if(++bflag > 1 || !strcmp("-b",optarg))
                {
                    printError("Multiple '-b' flags!", 0, 0);
                }else if((bufsize = strtol(optarg, NULL, 0)) <= 0){
                    printError("Not a valid buffer size", 0, 0);
                }else if(!strcmp("-o",optarg) || !strcmp("-", optarg)){
                    printError("No size of buffer mentioned!", 0, 0);
                }
                break;
            case 'o':
                if(++oflag > 1 || !strcmp("-o", optarg))
                {
                    printError("Multiple '-o' flags!", 0, 0);
                }else if(!strcmp("-b",optarg)){
                    printError("No output file specified!", 0 ,0);
                }else if(!strcmp("-",optarg)){
                    printError("- reserved for stdin", 0, 0);
                }
                outputName = optarg;
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }
    //set default size of buffer and initialize a buffer as bufsize
    if(bufsize <= 0)
    {
        bufsize = 8192;
    }
    char buf[bufsize];
    //if there is outputName = 0, set it to stdout. otherwise, open file with the given name
    if(!outputName)
    {
        outputName = "stdout";
    }else if((outFD = open(outputName, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1){
        printError("Failed to open following file for writing", outputName, strerror(errno));
    }
    //if input files were specified go thorugh loop and RD/WR/Close
    for(; optind < argc; ++optind)
    {
        if(!strcmp(argv[optind],"-"))
        {
            argv[optind] = "stdin";
            inFD = STDIN_FILENO;
        }else if((inFD = open(argv[optind], O_RDONLY)) == -1){
            printError("Failed to open following file for reading", argv[optind], strerror(errno));
        }

        doRDWR(inFD, outFD, argv[optind], outputName, buf, bufsize);

        if(close(inFD) == -1 && inFD != STDIN_FILENO)
        {
            printError("Failed to close following input file", argv[optind], strerror(errno));
        }
    }
    //if no input files were specified then optind = argc so it skips the for loop and reads from stdin
    if(inFD == -1)
    {
        doRDWR(STDIN_FILENO, outFD, "stdin", outputName, buf, bufsize);
    }
    //close output file and error check
    if(close(outFD) == -1 && outFD != STDOUT_FILENO)
    {
        printError("Failed to close following output file", outputName, strerror(errno));
    }
    //there were no errors and program ran successfully
    return 0;
}
