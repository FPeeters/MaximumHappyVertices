@echo off
::##############################################################################
:: This script is run in the execution directory (execDir, --exec-dir).
::
:: PARAMETERS:
:: %%1 is the candidate configuration number
:: %%2 is the instance ID
:: %%3 is the seed
:: %%4 is the instance name
:: The rest are parameters to the target-algorithm
::
:: RETURN VALUE:
:: This script should print one numerical value: the cost that must be minimized.
:: Exit with 0 if no error, with 1 in case of error
::##############################################################################

:: Please change the EXE and FIXED_PARAMS to the correct ones
SET "exe=cmake-build-visual-studio\main.exe"
SET "fixed_params=-minimize -a growth -selectRandom -red none"

FOR /f "tokens=1-4*" %%a IN ("%*") DO (
	SET candidate=%%a
	SET instance_id=%%b
	SET seed=%%c
	SET instance=%%d
	SET candidate_parameters=%%e
)

SET "stdout=c%candidate%-%instance_id%-%seed%.stdout"
SET "stderr=c%candidate%-%instance_id%-%seed%.stderr"

%exe% %instance% %fixed_params% -r %seed% %candidate_parameters% 1>%stdout% 2>%stderr%

setlocal EnableDelayedExpansion
for /f "tokens=*" %%a in (%stdout%) do set COST=%%a
echo %COST%

del %stdout% %stderr%
exit 0
