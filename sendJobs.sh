#! /bin/bash

### how many jobs you want to submit, if -1, then submits all jobs
nJobs=${1:-"-1"}
#### runid for the output job name in the grid, increased by 1 for each job
runid=0


# Define the input file
### give the list of root files (along with the absolute path on WIS cluster) on which you want to run the histogram maker
INFILE=${2:-"inputFileNames.txt"}
## Data/MC
sample=${3:-"MC"}


### first prepare the .so file from makeMuonHist.cpp
# root -l -b << EOF
# .L makeMuonHist.cpp++
# EOF

# Read the input text file line by line
while read -r LINE
do
    printf 'Working on: %s\n' "$LINE"
    ### a counter value
    b=1
    ### runid increased by one
    runid=$(( $runid + $b ))
    echo "runid: "$runid
    ### the place where the output and error file of the grid will live
    ### change to the submitter's work area
    DESTINATION="/storage/agrp/arkas/MCPGridOutput"
    ### create the main directory if it does not exists
    mkdir -p ${DESTINATION}
    
    ### if main directory/run_id exists, delete
    if [[ -d "${DESTINATION}/run_$runid" ]]; then
        echo "Found a directory with output ${DESTINATION}/run_$runid! Deleting the previous one."
        rm -rf ${DESTINATION}/run_$runid
    fi

    #### the location where the histograms will live, change to your area
    OUTDIRLOC="/storage/agrp/arkas/MCPWorkArea"
    #### create the run directory
    mkdir -p ${DESTINATION}"/run_"$runid
    mkdir -p ${OUTDIRLOC}

    ### prepare the outfile
    ### the location where Zmumu MC files live: this should STAY THE SAME
    prefix="/storage/agrp/arkas/MuonWorkingPointFiles/user.asantra.mc23_13p6TeV.601190.PhPy8EG_AZNLO_Zmumu.recon.AOD.e8514_s4159_r14799.MCP_TESTNTUP_ANALYSIS.root/"
    suffix=".root"
    ### cutting out the prefix part from the input line 
    withoutRoot=${LINE#"$prefix"}
    ### cutting out the suffix part from the input line
    withoutRoot=${withoutRoot%"$suffix"}

    ### name of the outputfile without root
    echo "outFile base: ${withoutRoot}"

    ### final output file name - it goes to OUTDIRLOC
    outFile=${OUTDIRLOC}"/"${withoutRoot}"_Histogram.root"
    #### from where you are submitting jobs
    PRESENTDIRECTORY=${PWD}
    #### submit jobs to the PBS system
    qsub -l ncpus=1,mem=6gb -v parname1=${PRESENTDIRECTORY},parname2=${LINE},parname3=${outFile},parname4=${sample} -q N -N "run_"$runid -o "${DESTINATION}/run_"${runid} -e "${DESTINATION}/run_"${runid} gridScript.sh
    ### sleep for 1 s, so that there is no problem in submitting jobs to the grid
    sleep 1s
    ### if number of jobs required is reached then break the loop
    if [[ $runid -eq $nJobs ]]; then
        break
    fi
done < "$INFILE"
