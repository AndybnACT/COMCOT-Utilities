1. Make sure that you're runing on v1.7 of COMCOT, because the patching files are created based on the version, or modify codes by yourself without using the patching script. 
2. To upgrade your COMCOT v1.7, simpily type

        $ ./update.sh

    on command line. And then update your Makefile so that 'arrivaltime.f90' will be compiled. Finally, compile and run the program.
3. The update.sh will do backuping before patching files so don't worry if you want to reverse it in the future.
4. The arrival time data would now generate originally by COMCOT. The output files format and naming policy are same as z_max files

