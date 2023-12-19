#! /bin/bash

### which file extension you want to examine
fileExtns=${1:-"ER"}
#### runid for the output job name in the grid, increased by 1 for each job
runid=0

# Define the input file from a text file
INFILE=${2:-"inputFileNames.txt"}
sample=${3:-"MC"}

while read -r LINE
do
    printf '%s\n' "$LINE"
    ### a counter value
    b=1
    ### runId increased by one
    runid=$(( $runid + $b ))
    echo "runid: "$runid
    ### the place where the output and error file of the grid will live
    ### this should be the same as sendJobs.sh
    DESTINATION="/storage/agrp/arkas/MCPGridOutput"
    # DESTINATION="/storage/agrp/arkas/MCPGridOutput2Muons"

    ### this should be the same as sendJobs.sh
    OUTDIRLOC="/storage/agrp/arkas/MCPWorkArea"
    # OUTDIRLOC="/storage/agrp/arkas/MCPWorkArea2Muons"

    ### prepare the outfile
    suffix=".root"
    ### get only the last part of total path
    withoutRoot=`echo $LINE | rev | cut -d/ -f1 | rev`
    ### cutting out the suffix part from the input line
    withoutRoot=${withoutRoot%"$suffix"}

    ### name of the outputfile without root
    echo "outFile base: ${withoutRoot}"
    ### final output file name - it goes to OUTDIRLOC
    outFile=${OUTDIRLOC}"/"${withoutRoot}"_Histogram.root"
    #### from where you are submitting jobs
    PRESENTDIRECTORY=${PWD}
    ### examine the error log for any of the following keywords.
    ### if present, the job is resubmitte.
    flag=0
    if ls ${DESTINATION}/run_$runid/*${fileExtns} &> /dev/null
    then
        if grep -rnwq ${DESTINATION}/run_$runid/*${fileExtns} -e "error"
        then
            flag=1
        elif grep -rnwq ${DESTINATION}/run_$runid/*${fileExtns} -e "Killed"
        then
            flag=1
        elif grep -rnwq ${DESTINATION}/run_$runid/*${fileExtns} -e "killed"
        then
            flag=1
        elif grep -rnwq ${DESTINATION}/run_$runid/*${fileExtns} -e "Aborting"
        then
            flag=1
        elif grep -rnwq ${DESTINATION}/run_$runid/*${fileExtns} -e "Caught Geant4 exception"
        then
            flag=1
        elif grep -rnwq ${DESTINATION}/run_$runid/*${fileExtns} -e "Interrupted"
        then
            flag=1
        elif grep -rnwq ${DESTINATION}/run_$runid/*${fileExtns} -e "Aborted"
        then
            flag=1
        else
            flag=0
        fi
    else
        flag=2
    fi
    
    #### if there is an error, resubmit them
    if [[ $flag -eq 1 ]]
    then 
        echo "At least one error for ${DESTINATION}/run_$runid"
        
        if [[ -d "${DESTINATION}/run_$runid" ]]; then
            echo "Found a directory with output ${DESTINATION}/run_$runid! Deleting the previous one."
            rm -rf ${DESTINATION}/run_$runid
        fi
        
        #### create the run directory
        mkdir -p ${DESTINATION}"/run_"$runid
        #### from where you are submitting jobs
        PRESENTDIRECTORY=${PWD}
        qsub -l ncpus=1,mem=6gb -v parname1=${PRESENTDIRECTORY},parname2=${LINE},parname3=${outFile},parname4=${sample} -q N -N "run_"$runid -o "${DESTINATION}/run_"${runid} -e "${DESTINATION}/run_"${runid} gridScript.sh
        ### sleep for 1 s, so that there is no problem in submitting jobs to the grid
        sleep 1s
    elif [[ $flag -eq 2 ]]
    then
        echo "This folder does not exist: ${DESTINATION}/run_$runid"
    else
        echo "No problem in this set: ${DESTINATION}/run_$runid"
    fi
done < "$INFILE"