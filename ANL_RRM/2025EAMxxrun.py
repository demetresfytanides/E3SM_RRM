#!/usr/bin/env python3
import os
home=os.getenv('HOME')
case_dir = f'/lustre/orion/cli197/world-shared/dfytanidis/CASES'
#src_dir = f'/ccs/proj/cli197/dfytanidis/E3SM'
#src_dir = f'/lustre/orion/cli115/world-shared/wuda/Frontier/TEST_E3SM_SCORPIO_PERF/E3SM'
src_dir = f'/lustre/orion/cli197/proj-shared/dfytanidis/E3SM'

newcase      = True
config       = True
build        = True
submit       = True

proj = 'CLI197'
compset = 'F2010-SCREAMv1'

grid,nnodes,arch,stop_opt,stop_n,wallt = 'ne30pg2_ne30pg2',2,'gnugpu','ndays',382,'2:00:00'
#grid,nnodes,arch,stop_opt,stop_n,wallt = 'ne30pg2_ne30pg2',1,'gnugpu','ndays',1,'10:00'

grid0 = grid.split('_')[0]
case = '.'.join([grid0,compset,'33'])

#case = case+'.debug-on'

#---------------------------------------------------------------------------------------------------
def run_cmd(cmd): print(f'\n{cmd}') ; os.system(cmd); return
#---------------------------------------------------------------------------------------------------



#if [ "${res}" == "ne1024pg2_ne1024pg2" ]; then
#    pecount="16384x6" # 2048 nodes
#elif [ "${res}" == "ne256pg2_ne256pg2" ]; then
#    pecount="768x6" # 96 nodes
#elif [ "${res}" == "ne30pg2_EC30to60E2r2" ]; then
pecounti="16x6"
#fi



# Create new case
print(f'\n  case : {case}\n')
if newcase :
  if os.path.isdir(f'{case_dir}/{case}'): exit("This case already exists!m\n")
  cmd = f'{src_dir}/cime/scripts/create_newcase -mach frontier -case {case_dir}/{case}'
  cmd = cmd + f' -compset {compset} -res {grid} -compiler craygnu-hipcc -pecount {pecounti} -project {proj} --output-root {case_dir} '
  run_cmd(cmd)
os.chdir(f'{case_dir}/{case}/')

print('case created!')

#---------------------------------------------------------------------------------------------------
if config :
  run_cmd('./xmlchange HIST_N=1')
  run_cmd('./xmlchange HIST_OPTION=ndays')
  run_cmd('./xmlchange REST_N=15')
  run_cmd('./xmlchange REST_OPTION=ndays')
  run_cmd('./xmlchange AVGHIST_OPTION=ndays')
  run_cmd('./xmlchange AVGHIST_N=1')
  run_cmd('./xmlchange STOP_OPTION=ndays')
  run_cmd('./xmlchange STOP_N=382')
  run_cmd('./xmlchange MAX_TASKS_PER_NODE=56')
  run_cmd('./xmlchange MAX_MPITASKS_PER_NODE=8')
#  run_cmd('./xmlchange LND_NTHRDS=7')
  run_cmd('./xmlchange --file env_mach_pes.xml NTHRDS=1')
  run_cmd('./xmlchange --file env_mach_pes.xml NTHRDS_ATM=1')
  run_cmd('./xmlchange --file env_mach_pes.xml NTHRDS_LND=6')
  run_cmd('./xmlchange --file env_mach_pes.xml NTHRDS_ICE=6')
  run_cmd('./xmlchange --file env_mach_pes.xml NTHRDS_OCN=1')
  run_cmd('./xmlchange --file env_mach_pes.xml NTHRDS_ROF=1')
  run_cmd('./xmlchange --file env_mach_pes.xml NTHRDS_CPL=1')
  run_cmd('./xmlchange --file env_mach_pes.xml NTHRDS_GLC=1')
  run_cmd('./xmlchange --file env_mach_pes.xml NTHRDS_WAV=1')

  run_cmd('./xmlchange PIO_NETCDF_FORMAT=64bit_data')

  run_cmd('./xmlchange -file env_run.xml RUN_STARTDATE=2016-12-15')
  run_cmd(f'./atmchange initial_conditions::Filename=/ccs/proj/cli197/dfytanidis/INCITE_test/ne30pg2variableSST/HICCUP.atm_era5.2016-12-15.00.ne30np4.L128.nc')

# use GHG levels more appropriate for sim
# Average from 19940101 - 20150101
#  run_cmd(f'./atmchange co2vmr=377.2e-6')
#  run_cmd(f'./atmchange ch4vmr=1786.6e-9')
#  run_cmd(f'./atmchange n2ovmr=318.6e-9')
#  run_cmd(f'./atmchange orbital_year=-9999')
# use CO2 the same in land model
#  run_cmd(f'./atmchange CCSM_CO2_PPMV=377.2')

# Change lambda_high
#  run_cmd(f'./atmchange lambda_high=0.08')

# Set temperature cut off in dycore threshold to 180K
#  run_cmd(f'./atmchange vtheta_thresh=180')


  run_cmd('./xmlchange -file env_run.xml SSTICE_DATA_FILENAME=/ccs/proj/cli197/dfytanidis/INCITE_test/ne30pg2variableSST/HICCUP.sst_noaa.2016-12-15.00.nc')
  run_cmd('./xmlchange -file env_run.xml SSTICE_YEAR_ALIGN=2016')
  run_cmd('./xmlchange -file env_run.xml SSTICE_YEAR_START=2016')
  run_cmd('./xmlchange -file env_run.xml SSTICE_YEAR_END=2018')



  run_cmd('./xmlchange PIO_TYPENAME=pnetcdf')
  run_cmd('./xmlchange PIO_REARRANGER=1')  # use PIO_REARRANGER=3, for ADIOS; PIO_REARRANGER=1 for pnetcdf

if 'debug-on' in case : run_cmd(f'./xmlchange --append -id SCREAM_CMAKE_OPTIONS -val \" CMAKE_CXX_FLAGS -DYAKL_DEBUG \" ')

run_cmd('./case.setup --reset')



# use GHG levels more appropriate for sim
# Average from 19940101 - 20150101
run_cmd(f'./atmchange co2vmr=377.2e-6')
run_cmd(f'./atmchange ch4vmr=1786.6e-9')
run_cmd(f'./atmchange n2ovmr=318.6e-9')
run_cmd(f'./atmchange orbital_year=-9999')
# use CO2 the same in land model
run_cmd(f'./atmchange CCSM_CO2_PPMV=377.2')

# Change lambda_high
run_cmd(f'./atmchange lambda_high=0.08')

# Set temperature cut off in dycore threshold to 180K
run_cmd(f'./atmchange vtheta_thresh=180')



run_cmd('./case.setup --reset')



#---------------------------------------------------------------------------------------------------
if build :
  if 'debug-on' in case : run_cmd('./xmlchange -file env_build.xml -id DEBUG -val TRUE ')

  run_cmd(f'./xmlchange GMAKE_J={16}')
  run_cmd('./case.build')
#---------------------------------------------------------------------------------------------------
if submit :
  run_cmd(f'./xmlchange STOP_OPTION={stop_opt},STOP_N={stop_n},JOB_WALLCLOCK_TIME={wallt}')
  run_cmd(f'./xmlchange CHARGE_ACCOUNT={proj},PROJECT={proj}')

  run_cmd(f'./xmlchange --append SCREAM_ATMCHANGE_BUFFER=output_yaml_files=/ccs/proj/cli197/dfytanidis/INCITE_test/ne30pg2variableSST/scream_output.SurfVars1.yaml-ATMCHANGE_SEP-')
  run_cmd(f'./xmlchange --append SCREAM_ATMCHANGE_BUFFER=output_yaml_files+=/ccs/proj/cli197/dfytanidis/INCITE_test/ne30pg2variableSST/scream_output.SurfVars2.yaml-ATMCHANGE_SEP-')
  run_cmd(f'./xmlchange --append SCREAM_ATMCHANGE_BUFFER=output_yaml_files+=/ccs/proj/cli197/dfytanidis/INCITE_test/ne30pg2variableSST/scream_output.SurfVars3.yaml-ATMCHANGE_SEP-')
  run_cmd(f'./xmlchange --append SCREAM_ATMCHANGE_BUFFER=output_yaml_files+=/ccs/proj/cli197/dfytanidis/INCITE_test/ne30pg2variableSST/scream_output.3DFields.yaml-ATMCHANGE_SEP-')




run_cmd('./case.submit')
#---------------------------------------------------------------------------------------------------
print(f'\n  case : {case}\n') # Print the case name again for reference
