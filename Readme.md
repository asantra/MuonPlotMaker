# This is the histogram maker for MCP work.
The histogram maker code is `makeMuonHist.cpp`.

The plotting script is `muonPlots.py`

## Usage
1. Add relevant histograms in `makeMuonHist.cpp`.
2. One may need to add new columns to the dataframe using `.Define` corresponding to new histograms.
3. One can use filtering (adding new cuts) using `.Filter`.
4. Once the histograms are prepared (from both data and MC), the plotting can be done using `muonPlots.py`.

### A. Making histograms from Data:
a. Setup Root:
```
echo "Installing Root>>>>>>"
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
### for the latest root
lsetup "views LCG_104b_ATLAS_2 x86_64-centos7-gcc11-opt"
```

b. Inside the root prompt: 
``` 
.L makeMuonHist.cpp++
makeMuonHist()
```

c. This will prepare histogram file (one file) from all the input data root files. 



### B. Making histograms from MC:
a. Change the `DESTINATION` and `OUTDIRLOC` in `sendJobs.sh` and `resendJobs.sh`

b. Run `sendJobs.sh` using: 
```
source sendJobs.sh
```

c. This will submit jobs to WIS cluster per input line from `inputFileNames.txt`.

d. Once all the jobs are completed, check if there is any problem: run 
```
source resendJobs.sh
```

e. This script will examine the cluster log and will resubmit only those jobs which had any problem. 

f. Once satisifed with the output, one has to 'hadd' all the histogram files to get a final histogram file.


### C. Plotting and comparing histograms from Data and MC:
a. Use `muonPlots.py`

b. When no comparison plots are needed: run 
```
python muonPlots.py -i1 <root file containing Data or MC hisotgram>
```

c. When data vs MC comparison plots are needed: run (order of the Data and MC histogram file is IMPORTANT)
```
python muonPlots.py -i1 <root file containing Data hisotgram> -comp -i2 <root file containing MC histogram>
```

d. The plots will be kept inside a folder prefixed with `muCompDist_`.

