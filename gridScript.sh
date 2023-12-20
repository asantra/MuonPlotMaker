#! /bin/bash
#PBS -m n
#PBS -l walltime=24:00:00

#### script that run the makeMuonHist (prepare histogram from ntuple)
echo "Installing Root>>>>>>"
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
### for latest root
lsetup "views LCG_104c_ATLAS_2 x86_64-centos7-gcc11-opt"

directory=${parname1}
inFile=${parname2}
outFile=${parname3}
sample=${parname4}

#### go to the directory where the files live
cd ${directory}
echo "I am now in "${PWD}
echo "running root"
root -l -b << EOF
gSystem->Load("makeMuonHist_cpp.so")
makeMuonHist("${inFile}", "${outFile}", "${sample}", true)
EOF
