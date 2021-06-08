/*
    Copyright (C) 2012 Modelon AB <http://www.modelon.com>

	You should have received a copy of the LICENCE-FMUChecker.txt
    along with this program. If not, contact Modelon AB.
*/
/**
	\file fmi2_cs_sim.c
	Simulation loop for the FMI 2.0 Co-simulation FMUs
*/
#include <fmuChecker.h>
#include <fmilib.h>

jm_status_enu_t fmi2_cs_prep_init(fmu_check_data_t* cdata)
{	
	fmi2_status_t fmistatus;
    jm_status_enu_t jmstatus = jm_status_success;
    jm_callbacks* cb = &cdata->callbacks;

	fmi2_import_t* fmu = cdata->fmu2;
	fmi2_string_t fmuGUID = fmi2_import_get_GUID(fmu);
	fmi2_boolean_t visible = fmi2_false;

	fmi2_boolean_t toleranceControlled = fmi2_false;
	fmi2_real_t relativeTolerance = fmi2_import_get_default_experiment_tolerance(fmu);

	fmi2_real_t tstart = fmi2_import_get_default_experiment_start(fmu);
	fmi2_real_t hstep;
	fmi2_real_t tend = fmi2_import_get_default_experiment_stop(fmu);
	fmi2_boolean_t canHandleVarStepSize = fmi2_import_get_capability(fmu,fmi2_cs_canHandleVariableCommunicationStepSize);
		

    prepare_time_step_info(cdata, &tend, &hstep);

	cdata->instanceNameToCompare = "Test FMI 2.0 CS";
	cdata->instanceNameSavedPtr = 0;
	jmstatus = fmi2_import_instantiate(fmu, cdata->instanceNameToCompare, fmi2_cosimulation, 0, visible);

	cdata->instanceNameSavedPtr = cdata->instanceNameToCompare;

	if (jmstatus == jm_status_error) {
		jm_log_fatal(cb, fmu_checker_module, "Could not instantiate the model");
		return jm_status_error;
	}
	
	//fmistatus = fmi2_import_initialize(fmu, 0 /* relTolerance */, tstart, StopTimeDefined, tend);
    if (!fmi2_status_ok_or_warning(fmistatus =  fmi2_import_setup_experiment(fmu, toleranceControlled,relativeTolerance, tstart, fmi2_false, 0.0)))
    {
        jm_log_fatal(cb, fmu_checker_module, "Could not setup expirement");
        return jm_status_error;
    }
    if (fmi2_status_ok_or_warning(fmistatus = fmi2_import_enter_initialization_mode(fmu)))
    {
        jm_log_info(cb, fmu_checker_module, " Entered initialization mode at time %g", tstart);
        fmistatus = fmi2_status_ok;
    }
    return jmstatus;
}

jm_status_enu_t fmi2_cs_prep_simulate(fmu_check_data_t* cdata)
{
    fmi2_status_t fmistatus;
    fmi2_import_t* fmu = cdata->fmu2;
    jm_callbacks* cb = &cdata->callbacks;
    jm_status_enu_t jmstatus = jm_status_success;
    fmi2_real_t tstart = fmi2_import_get_default_experiment_start(fmu);
    if(fmi2_status_ok_or_warning(fmi2_import_exit_initialization_mode(fmu))){
        cdata->slave_initialized = 1;
			jm_log_info(cb, fmu_checker_module, "Initialized FMU for simulation starting at time %g", tstart);
			fmistatus = fmi2_status_ok;
	}
	else {
			jm_log_fatal(cb, fmu_checker_module, "Failed to initialize FMU for simulation (FMU status: %s)", fmi2_status_to_string(fmistatus));
			jmstatus = jm_status_error;
	}

	if(jmstatus != jm_status_error) {
		jm_log_verbose(cb, fmu_checker_module, "Writing simulation output for start time");
		if(fmi2_write_csv_data(cdata, tstart) != jm_status_success){
			jmstatus = jm_status_error;
		}
    }

    return jmstatus;
}

jm_status_enu_t fmi2_cs_simulate_step(fmu_check_data_t* cdata, fmi1_real_t tcur)
{
    fmi2_status_t       fmistatus           = fmi2_status_ok;
    jm_status_enu_t     jmstatus            = jm_status_success;
    jm_callbacks        *cb                 = &cdata->callbacks;

    fmi2_import_t       *fmu                = cdata->fmu2;
    fmi2_real_t         hstep               = cdata->stepSize;

    fmi2_boolean_t newStep = fmi2_true;
    fmi2_real_t tnext = tcur + hstep;

    jm_log_verbose(cb, fmu_checker_module, "Simulation step from time: %g until: %g", tcur, tnext);


    fmistatus = fmi2_import_do_step(fmu, tcur, hstep, newStep);

    tcur = tnext;

    if(fmi2_write_csv_data(cdata, tcur) != jm_status_success){
        jmstatus = jm_status_error;
    }

    if((fmistatus == fmi2_status_ok) || (fmistatus == fmi2_status_warning)) {
        fmistatus = fmi2_status_ok;
    }
    else if(fmistatus == fmi2_status_discard) {
        fmi2_boolean_t bstatus = fmi2_false;
        fmi2_real_t lastTime;
        fmistatus = fmi2_import_get_real_status(fmu, fmi2_last_successful_time, &lastTime);
        if((fmistatus != fmi2_status_ok) && (fmistatus != fmi2_status_warning)) {
            jm_log_error(cb, fmu_checker_module, "Could not retrive fmiLastSuccessfulTime status since FMU returned: %s",fmi2_status_to_string(fmistatus));
        }
        else
            tcur = lastTime;

        fmistatus = fmi2_import_get_boolean_status(fmu, fmi2_terminated, &bstatus);
        if((fmistatus != fmi2_status_ok) && (fmistatus != fmi2_status_warning))
        {
            jm_log_error(cb, fmu_checker_module, "Could not retrive fmiTerminated status since FMU returned: %s",fmi2_status_to_string(fmistatus));
            jmstatus = jm_status_error;
        }
        else if(bstatus) {
            jm_log_info(cb, fmu_checker_module, "FMU requests to terminate simulation at time %g", lastTime);
            fmistatus = fmi2_status_ok;
        }
        else {
            fmistatus = fmi2_status_discard;
            jmstatus = jm_status_error;
        }
    }
    else
        jmstatus = jm_status_error;

	return jmstatus;
}
