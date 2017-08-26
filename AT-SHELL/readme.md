This is a BASH script to keep the process of simulation alive whether the user is still connected to the cluster 'wave' after the process has been started by the script. 
The script will check the format of inputs first. User must specify a computing node at argv[1]. For argv[2], enter the path name of the COMCOT project you wish to run on. The script would consider the current directory as your COMCOT project if you kept the argv[2] blank. 
Later, the script will fork an 'at shell'. Inside the shell, ssh to the computing node, start the process, and redirect stdout and stderr to a file (default: JOB_DIARY.dat at the project directory) so that user can keep track of the process. 
An 'at' shell is widly used for system scheduling. 
