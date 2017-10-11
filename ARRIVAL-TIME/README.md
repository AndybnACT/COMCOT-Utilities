# README for [ARRIVAL-TIME](https://github.com/HandsomeAndy/COMCOT-Utilities/tree/master/ARRIVAL-TIME)
1. Make sure that you're runing COMCOT on the version 1.7, because the patching files are edited based on that version, or modify codes by yourself. It may not be too hard to find the corresponding part in the program by given `.patch` files. 
2. It takes only three steps to upgrade your COMCOT (v1.7):
   - First, download the directory and move the containing files to your COMCOT directory, then execute the patching script:
           
                $ svn checkout https://github.com/HandsomeAndy/COMCOT-Utilities/trunk/ARRIVAL-TIME
                $ mv ./ARRIVAL-TIME/* <PATH/TO/YOUR/COMCOT/DIRECTORY>
                $ cd <PATH/TO/YOUR/COMCOT/DIRECTORY>
                $ ./update.sh
           
   - Second, update your Makefile so that `arrivaltime.f90` will be compiled futher. Or just rename and use the Makefile provided here
                
                $ mv Makefile Makefile.bak
                $ mv Makefile.new Makefile # Modify OPT and F90 flags if 'ifort' is not avaliable.
                
   - And finally, compile and you are ready to run the program.
        
                $ make
                $ ./comcot
                
3. After the update, the program will record arrival time data by itself. The output files format and naming policy are same as `z_max` files while the content of the `arrival` files will be arrival time in seconds.
## Todo list for [ARRIVAL-TIME](https://github.com/HandsomeAndy/COMCOT-Utilities/tree/master/ARRIVAL-TIME)
### Parallelism
- [ ] openmp parallelism of arrivaltime.f90
- [ ] openmp parallelism of output.f90
