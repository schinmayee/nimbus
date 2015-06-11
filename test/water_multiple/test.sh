#!/bin/bash

# **************************
# Text Reset
RCol='\x1B[0m'    
# Regular           
Bla='\x1B[0;30m';
Red='\x1B[0;31m';
Gre='\x1B[0;32m';
Yel='\x1B[0;33m';
Blu='\x1B[0;34m';
Pur='\x1B[0;35m';
Cya='\x1B[0;36m';
Whi='\x1B[0;37m';
# **************************

if [ "$1" = t ]
then
killall -v worker
exit
fi

# echo -e "${Pur}launching scheduler version I ...${RCol}"
# ../scheduler_v1/scheduler 2  > scheduler.txt &
# sleep 1
 
echo -e "${Pur}Launching $1 workers  each with $2 threads...${RCol}"
for i in `seq 1 $1`;
do
  # ./worker --cip localhost --cport 5900 --othread $2 -p 590$i --pnx 4 --pny 4 --pnz 4 --ppnx 4 --ppny 4 --ppnz 4 -s 80 -e 4&
  ./worker --cip localhost --cport 5900 --othread $2 -p 590$i & 
done

