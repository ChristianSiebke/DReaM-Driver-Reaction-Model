/*
    Copyright (C) 2012 Modelon AB <http://www.modelon.com>
                  2020, 2021 in-tech GmbH

    You should have received a copy of the LICENCE-FMUChecker.txt
    along with this program. If not, contact Modelon AB.
*/
/**
    \file fmuChecker.c
    Main function and command line options handling of the FMUChecker application.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>

#if defined(_WIN32) || defined(WIN32)
#include <Shlwapi.h>
#include <wchar.h>
#endif

// #include <config_test.h>
#include <JM/jm_portability.h>
#include <fmilib.h>
#include <fmuChecker.h>
//#include <fmu_checker_version.h>
#include <fmilib_config.h>

const char* fmu_checker_module = "FMUCHK";

#define BUFFER 1000

void do_exit(int code)
{
    /* when running on Windows this may be useful:
        printf("Press 'Enter' to exit\n");
        getchar(); */
    exit(code);
}

int allocated_mem_blocks = 0;

void* check_calloc(size_t nobj, size_t size) {
    void* ret = calloc(nobj, size);
    if(ret) allocated_mem_blocks++;
    jm_log_verbose(&cdata_global_ptr->callbacks, fmu_checker_module,
                   "allocateMemory( %u, %u) called. Returning pointer: %p",nobj,size,ret);
    return ret;
}

void  check_free(void* obj) {
    jm_log_verbose(&cdata_global_ptr->callbacks, fmu_checker_module, "freeMemory(%p) called", obj);
    if(obj) {
        free(obj);
        allocated_mem_blocks--;
    }
}

int wcstostr(char* s, const wchar_t* wcs)
{
    int i;  //!< string index
    int c;  //!< converted character


    for (i=0;
         ((*(wcs+i))!= L'\0') && (c = wctob(*(wcs+i))) != EOF; // convert wide to byte
         ++i)
    {
        *(s+i) = (char)c;                // add converted character to byte string
    }
    *(s+i) = '\0';                       // add end of string marker

    if(*(wcs+i) != L'\0')
        return -1;                       // indicate conversion not possible
    else
        return i;                        // successful - return number of characters
}

void checker_logger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message) {
    fmu_check_data_t* cdata = (fmu_check_data_t*)c->context;
    int ret;

    if(log_level == jm_log_level_warning)
        cdata->num_warnings++;
    else if(log_level == jm_log_level_error)
        cdata->num_errors++;
    else if(log_level == jm_log_level_fatal)
        cdata->num_fatal++;

    if(log_level)
        ret = fprintf(cdata->log_file, "[%s][%s] %s\n", jm_log_level_to_string(log_level), module, message);
    else
        ret = fprintf(cdata->log_file, "%s\n", message);

    fflush(cdata->log_file);

    if(ret <= 0) {
        fclose(cdata->log_file);
        cdata->log_file = stderr;
        fprintf(stderr, "[%s][%s] %s\n", jm_log_level_to_string(log_level), module, message);
        fprintf(stderr, "[%s][%s] %s\n", jm_log_level_to_string(jm_log_level_fatal), module, "Error writing to the log file");
        cdata->num_fatal++;
    }
}

//void print_version() {
//    printf("FMI compliance checker " FMUCHK_VERSION " [FMILibrary: "FMIL_VERSION"] build date: "__DATE__
//#ifdef FMILIB_ENABLE_LOG_LEVEL_DEBUG
//        " "__TIME__
//#endif
//        "\n");
//}



int parse_options(fmu_check_data_t *cdata)
{

    // set cosimulation
    cdata->require_cs    = 1;

    /* <stopTime>   */
//    cdata->stopTime  = 6000;                 // ms  for testing

    /* <log level>  */
    cdata->callbacks.log_level = jm_log_level_nothing;
    if (cdata->write_log_files)
        cdata->callbacks.log_level = jm_log_level_all;

    /* logfile name is set in constructor */

    /* <csvSeparator> Separator character to be used. Default is ','  */
    cdata->CSV_separator = ',';


    /* Print all variables to the output file. Default is to only print outputs   */
    cdata->do_output_all_vars  = 1;

    /* <tmp-dir> Temporary dir to use for unpacking the fmu. Default is to create an unique temporary dir  */
    cdata->temp_dir            = 0;

    /* <infile>  Name of the CSV file name with input data  */
    cdata->inputFileName       = 0;

    /*                          */
    cdata->do_mangle_var_names = 1;

    /*  Print also left limit values at event points to the output file to investigate event behaviour. */
    /* Default is to only print values after event handling.\n\n"                                       */
    cdata->print_all_event_vars = 1;

    /* <unzip-dir>  */
    //unzip directory was set in constructor

    //    FMUPath provision
    //cdata->FMUPath = FMU_fullName;

    //    Test flags: reset Module Exchange (me) , set CoSimulation (cs)
    cdata->do_test_me = 0;
    cdata->do_test_cs = cdata->require_cs;
    cdata->do_simulate_flg = cdata->do_test_cs;

    // log file handling
    // if name given, try to open
    // if not possible or no file name given, assign stderr
    if(cdata->log_file_name) {
        cdata->log_file = fopen(cdata->log_file_name, "wb");
        if(!cdata->log_file) {
            cdata->log_file = stderr;
            jm_log_fatal(&cdata->callbacks,
                         fmu_checker_module,
                         "Could not open %s for writing", cdata->log_file_name);

            clear_fmu_check_data(cdata, 1);
            return fmi1_false;
        }
    }

    {
        jm_log_level_enu_t log_level = cdata->callbacks.log_level;
        jm_log_verbose(&cdata->callbacks,
                       fmu_checker_module,
                       "Setting log level to [%s]", jm_log_level_to_string(log_level));

#ifndef FMILIB_ENABLE_LOG_LEVEL_DEBUG
        if(log_level == jm_log_level_debug) {
            jm_log_verbose(&cdata->callbacks,
                           fmu_checker_module,
                           "This binary is built without debug log messages."
            "\n Reconfigure with FMUCHK_ENABLE_LOG_LEVEL_DEBUG=ON and rebuild to enable debug level logging");
        }
#endif
    }

    //    check whether FMU file  can be opened
    {
        FILE* tryFMU = fopen(cdata->FMUPath, "r");
        if(tryFMU == 0) {
            jm_log_fatal(&cdata->callbacks,
                         fmu_checker_module,
                         "Cannot open FMU file (%s)",
                         strerror(errno));

            clear_fmu_check_data(cdata, 1);
            return fmi1_false;
        }
        fclose(tryFMU);
    }

    //    open input file, if name was given
    {
        if(cdata->inputFileName) {
            FILE* infile = fopen(cdata->inputFileName, "rb");
            if(infile) {
                fclose(infile);
            }
            else {
                jm_log_fatal(&cdata->callbacks,
                             fmu_checker_module,
                             "Cannot open input data file (%s)",
                             strerror(errno));

                clear_fmu_check_data(cdata, 1);
                return fmi1_false;
            }
        }
    }

    //    set temp_dir, if not given
    if(!cdata->temp_dir) {
        // function returns a wide string,          // NOTE: by in-tech: true for win32 only?
        // conversion to a byte string is necessary
        cdata->temp_dir = (char*) jm_get_system_temp_dir();
#ifdef WIN32
        int ret;
        ret = wcstostr(cdata->temp_dir, (wchar_t*)(cdata->temp_dir));
        // if the string could not be converted, assign the default
        if (ret==-1) cdata->temp_dir = 0;
#endif
        if(!cdata->temp_dir) cdata->temp_dir = "./";
    }

    if(!cdata->tmpPath)
    {
        return fmi1_false;
    }

    //    if outputfile name given, open it and assign the handle to cdata
    if(cdata->output_file_name) {
        cdata->out_file = fopen(cdata->output_file_name, "wb");
        if(!cdata->out_file) {
            jm_log_fatal(&cdata->callbacks,
                         fmu_checker_module,
                         "Could not open %s for writing",
                         cdata->output_file_name);

            clear_fmu_check_data(cdata, 1);
            return fmi1_false;
        }
    }
    return fmi1_true;
}

jm_status_enu_t checked_print_quoted_str(fmu_check_data_t* cdata, const char* str) {
    jm_status_enu_t status = jm_status_success;
    if(!str) return status;
    if(strchr(str, '"')) {
        /* replace double quotes with single quotes */
#ifdef _MSC_VER
        char* buf = _strdup(str);
#else
        char* buf = strdup(str);
#endif
        char * ch = strchr(buf, '"');
        while(ch) {
            *ch = '\'';
            ch = strchr(ch + 1, '"');
        }
        status = checked_fprintf(cdata, "\"%s\"", buf);
        free(buf);
    }
    else
        status = checked_fprintf(cdata, "\"%s\"", str);

    return status;
}


jm_status_enu_t checked_fprintf(fmu_check_data_t* cdata, const char* fmt, ...) {
    jm_status_enu_t status = jm_status_success;
    if (cdata->write_output_files)
    {
        va_list args;
        va_start (args, fmt);
        if(vfprintf(cdata->out_file, fmt, args) <= 0) {
            jm_log_fatal(&cdata->callbacks, fmu_checker_module, "Error writing output file (%s)", strerror(errno));
            status = jm_status_error;
        }
        va_end (args);
    }
    return status;
}

jm_status_enu_t check_fprintf_var_name(fmu_check_data_t* cdata, const char* vn) {
    char buf[10000], *cursrc, *curdest;
    int need_quoting = 1;
    jm_status_enu_t status = jm_status_success;
    char replace_sep = ':';

    if(cdata->CSV_separator == ':') {
        replace_sep = '|';
    }

    if(cdata->do_mangle_var_names) {
        /* skip spaces ans repace separator character in column names */
        sprintf(buf, "%s", vn);
        curdest = cursrc = buf;
        while(*cursrc) {
            if(*cursrc != ' ') {
                if(*cursrc == cdata->CSV_separator)
                    *curdest = replace_sep;
                else if(curdest != cursrc)
                    *curdest = *cursrc;
                curdest++;
            }
            cursrc++;
        }
        *curdest = 0;
    }
    else {
        int j = 0;
        while(vn[j]) {
            char ch = vn[j];
            if((ch == cdata->CSV_separator)
               || (ch == '"')
               || (ch == ' ')
               || (ch == '\n')
               || (ch == '\t')
               || (ch == '\r')) {
                need_quoting = 1;
                break;
            }
            j++;
        }
        if(need_quoting) {
            curdest = buf;
            *curdest = '"';
            curdest++;
            j = 0;
            while(vn[j]) {
                char ch = vn[j];
                if(ch == '"') {
                    *curdest = ch;
                    curdest++;
                }
                *curdest = ch;
                curdest++;
                j++;
            }
            *curdest = '"';
            curdest++;
            *curdest = 0;
        }
        else {
            sprintf(buf, "%s", vn);
        }
    }
    status = checked_fprintf(cdata, "%c%s", cdata->CSV_separator, buf);
    if(status != jm_status_success) {
        return jm_status_error;
    }
    return status;
}

void init_fmu_check_data(fmu_check_data_t* cdata) {
    cdata->simulation_initialized = 0;
    cdata->num_errors = 0;
    cdata->num_warnings = 0;
    cdata->num_fatal = 0;
    cdata->num_fmu_messages = 0;
    cdata->printed_instance_name_error_flg = 0;

    cdata->callbacks.malloc = malloc;
    cdata->callbacks.calloc = calloc;
    cdata->callbacks.realloc = realloc;
    cdata->callbacks.free = free;
    cdata->callbacks.logger = checker_logger;
    cdata->callbacks.log_level = jm_log_level_info;
    cdata->callbacks.context = cdata;

    cdata->context = 0;

    cdata->modelIdentifierFMI1 = 0;
    cdata->modelIdentifierME = 0;
    cdata->modelIdentifierCS = 0;
    cdata->modelName = 0;
    cdata->GUID = 0;
    cdata->instanceNameSavedPtr = 0;
    cdata->instanceNameToCompare = 0;

    cdata->maxOutputPts = DEFAULT_MAX_OUTPUT_PTS;
    cdata->maxOutputPtsSetByUser = 0;
    cdata->nextOutputTime = 0.0;
    cdata->nextOutputStep = 0;
    cdata->CSV_separator = ',';
#ifdef SUPPORT_out_enum_as_int_flag
    cdata->out_enum_as_int_flag = 0;
#endif
    cdata->out_file = stdout;
    cdata->log_file = stderr;
    cdata->do_simulate_flg = 1;
    cdata->do_test_me = 1;
    cdata->do_test_cs = 1;
    cdata->require_me = 0;
    cdata->require_cs = 0;
    cdata->do_mangle_var_names = 0;
    cdata->do_output_all_vars = 0;
    cdata->print_all_event_vars = 0;

    cdata->version = fmi_version_unknown_enu;

    cdata->fmu1 = 0;
    cdata->fmu1_kind = fmi1_fmu_kind_enu_unknown;
    cdata->vl = 0;

    cdata->fmu2 = 0;
    cdata->fmu2_kind = fmi2_fmu_kind_unknown;
    cdata->vl2 = 0;
    assert(cdata_global_ptr == 0);
    cdata_global_ptr = cdata;
}

void clear_fmu_check_data(fmu_check_data_t* cdata, int close_log) {
    if(cdata->fmu1) {
        fmi1_import_free(cdata->fmu1);
        cdata->fmu1 = 0;
    }
    if(cdata->fmu2) {
        fmi2_import_free(cdata->fmu2);
        cdata->fmu2 = 0;
    }
    if(cdata->context) {
        fmi_import_free_context(cdata->context);
        cdata->context = 0;
    }
    if(cdata->tmpPath && (cdata->tmpPath != cdata->unzipPath)) {
        jm_rmdir(&cdata->callbacks,cdata->tmpPath);
    }
    if(cdata->out_file && (cdata->out_file != stdout)) {
        fclose(cdata->out_file);
    }
    if(cdata->vl) {
        fmi1_import_free_variable_list(cdata->vl);
        cdata->vl = 0;
    }
    if(cdata->vl2) {
        fmi2_import_free_variable_list(cdata->vl2);
        cdata->vl2 = 0;
    }
    if(close_log && cdata->log_file && (cdata->log_file != stderr)) {
        fclose(cdata->log_file);
        cdata->log_file = stderr;
    }
    cdata_global_ptr = 0;
}

/* Prepare the time step, time end and number of steps info
    for the simulation.
    Input/output: information from default experiment
*/
void prepare_time_step_info(fmu_check_data_t* cdata, double* timeEnd, double* timeStep) {
    if(cdata->stopTime > 0) {
        *timeEnd = cdata->stopTime;
    }
    else {
        cdata->stopTime = *timeEnd;
    }

    if(cdata->stepSizeSetByUser) {
        *timeStep = cdata->stepSize;
    }
    else if(cdata->maxOutputPtsSetByUser) {
        if(cdata->maxOutputPts) {
            *timeStep = cdata->stopTime / cdata->maxOutputPts;
        }
        else {
            *timeStep = cdata->stopTime / DEFAULT_MAX_OUTPUT_PTS;
        }
    }
    else {
        *timeStep = cdata->stopTime / cdata->maxOutputPts;
    }
}

fmu_check_data_t* cdata_global_ptr = 0;

static int direxists(const char* dirname) {
    struct stat s;
    return stat(dirname, &s) == 0 && (S_IFDIR & s.st_mode);
}

static int check_dir_structure(fmu_check_data_t *cdata)
{
    char *bindir;
    char *srcdir;
    int is_valid = 0;

    size_t pathlen = strlen(cdata->tmpPath);
    size_t binlen = pathlen + strlen(FMI_FILE_SEP) + 8; /*strlen("binaries") == 8*/
    size_t srclen = pathlen + strlen(FMI_FILE_SEP) + 7; /*strlen("sources") == 7*/

    bindir = cdata->callbacks.calloc(binlen + 1, sizeof(char));
    srcdir = cdata->callbacks.calloc(srclen + 1, sizeof(char));
    if (bindir == NULL || srcdir == NULL) {
        jm_log_fatal(&cdata->callbacks,
                     fmu_checker_module,
                     "Failed to allocate memory");
        clear_fmu_check_data(cdata, 1);
        do_exit(1);
    }

    jm_snprintf(bindir, binlen + 1, "%s" FMI_FILE_SEP "binaries", cdata->tmpPath);
    jm_snprintf(srcdir, srclen + 1, "%s" FMI_FILE_SEP "sources", cdata->tmpPath);

    is_valid = direxists(bindir) || direxists(srcdir);

    cdata->callbacks.free(bindir);
    cdata->callbacks.free(srcdir);

    return is_valid;
}

jm_status_enu_t fmuChecker( fmu_check_data_t  *cdata)
{
    jm_status_enu_t status = jm_status_success;
    jm_callbacks* callbacks;

    init_fmu_check_data(cdata);
    callbacks = &(cdata->callbacks);
    if(!parse_options(cdata))
    {
        return jm_status_error;
    }
#ifdef FMILIB_GENERATE_BUILD_STAMP
    jm_log_debug(callbacks,fmu_checker_module,"FMIL build stamp:\n%s\n", fmilib_get_build_stamp());
#endif

    //#ifdef FMILIB_ENABLE_LOG_LEVEL_DEBUG
    //	jm_log_info(callbacks,fmu_checker_module,"FMI compliance checker " FMUCHK_VERSION " [FMILibrary: "FMIL_VERSION"] build date: "__DATE__ " "__TIME__);
    //#else
    //	jm_log_info(callbacks,fmu_checker_module,"FMI compliance checker " FMUCHK_VERSION " [FMILibrary: "FMIL_VERSION"] build date: "__DATE__ );
    //#endif


    jm_log_info(callbacks,fmu_checker_module,"Will process FMU %s",cdata->FMUPath);

    cdata->context = fmi_import_allocate_context(callbacks);
    fmi_import_set_configuration(cdata->context, FMI_IMPORT_NAME_CHECK);

    cdata->version = fmi_import_get_fmi_version(cdata->context, cdata->FMUPath, cdata->tmpPath);
    if(cdata->version == fmi_version_unknown_enu) {
        jm_log_fatal(callbacks,fmu_checker_module,"Error in FMU version detection");
        do_exit(1);
    }

    if (!check_dir_structure(cdata)) {
        jm_log_error(&cdata->callbacks,
                     fmu_checker_module,
                     "FMU must contain either a \"sources\" or a \"binaries\" folder");
    }

    switch(cdata->version) {
        case  fmi_version_1_enu:
            status = fmi1_check(cdata);
            break;
        case  fmi_version_2_0_enu:
            status = fmi2_check(cdata);
            break;
        default:
            clear_fmu_check_data(cdata, 1);
            jm_log_fatal(callbacks,fmu_checker_module,"Only FMI version 1.0 and 2.0 are supported so far");
            do_exit(1);
    }

    return status;
}

jm_status_enu_t fmi1_end_handling(fmu_check_data_t *cdata)
{

    fmi1_status_t       fmistatus;
    fmi1_import_t       *fmu                = cdata->fmu1;
    jm_status_enu_t     status              = jm_status_success;
    jm_callbacks        *callbacks          = &cdata->callbacks;

    if(cdata->simulation_initialized)
    {
        fmistatus = fmi1_import_terminate_slave(fmu);

        fmi1_import_free_slave_instance(fmu);

        cdata->simulation_initialized = 0;

    }
    clear_fmu_check_data(cdata, 0);

    if(allocated_mem_blocks)  {
        if(allocated_mem_blocks > 0) {
            jm_log_error(callbacks,fmu_checker_module,
                         "Memory leak: freeMemory was not called for %d block(s) allocated by allocateMemory",
                         allocated_mem_blocks);
        }
        else {
            jm_log_error(callbacks,fmu_checker_module,
                         "Memory mamagement: freeMemory was called without allocateMemory for %d block(s)",
                         -allocated_mem_blocks);
        }
    }
    if(cdata->write_log_files)
    {
        jm_log(callbacks, fmu_checker_module, jm_log_level_nothing, "FMU check summary:");

        jm_log(callbacks, fmu_checker_module, jm_log_level_nothing, "FMU reported:\n\t%u warning(s) and error(s)\nChecker reported:", cdata->num_fmu_messages);

        if(callbacks->log_level < jm_log_level_error) {
            jm_log(callbacks, fmu_checker_module, jm_log_level_nothing,
                   "\tWarnings and non-critical errors were ignored (log level: %s)", jm_log_level_to_string( callbacks->log_level ));
        }
        else {
            if(callbacks->log_level < jm_log_level_warning) {
                jm_log(callbacks, fmu_checker_module, jm_log_level_nothing,
                       "\tWarnings were ignored (log level: %s)", jm_log_level_to_string( callbacks->log_level ));
            }
            else
                jm_log(callbacks, fmu_checker_module, jm_log_level_nothing, "\t%u Warning(s)", cdata->num_warnings);
            if ((cdata->num_fatal > 0)) { cdata->num_errors=cdata->num_errors+cdata->num_fatal;
            }
            jm_log(callbacks, fmu_checker_module, jm_log_level_nothing, "\t%u Error(s)", cdata->num_errors);
        }
    }
    if((status == jm_status_success) && (cdata->num_fatal == 0)) {
        if(cdata->log_file && (cdata->log_file != stderr))
            fclose(cdata->log_file);
        return jm_status_success;
    }
    else {
        jm_log(callbacks, fmu_checker_module, jm_log_level_nothing,
               "\t%u Fatal error(s) occurred during processing",cdata->num_fatal);
        if(cdata->log_file && (cdata->log_file != stderr))
            fclose(cdata->log_file);
        return jm_status_error;
    }
}

jm_status_enu_t fmi2_end_handling(fmu_check_data_t *cdata)
{

    fmi2_status_t       fmistatus;
    fmi2_import_t       *fmu                = cdata->fmu2;
    jm_status_enu_t     status              = jm_status_success;
    jm_callbacks        *callbacks          = &cdata->callbacks;

    if(cdata->simulation_initialized)
    {
        fmistatus = fmi2_import_terminate(fmu);

        fmi2_import_free_instance(fmu);

        cdata->simulation_initialized = 0;

    }
    clear_fmu_check_data(cdata, 0);

    if(allocated_mem_blocks)  {
        if(allocated_mem_blocks > 0) {
            jm_log_error(callbacks,fmu_checker_module,
                         "Memory leak: freeMemory was not called for %d block(s) allocated by allocateMemory",
                         allocated_mem_blocks);
        }
        else {
            jm_log_error(callbacks,fmu_checker_module,
                         "Memory mamagement: freeMemory was called without allocateMemory for %d block(s)",
                         -allocated_mem_blocks);
        }
    }
    if(cdata->write_log_files)
    {
        jm_log(callbacks, fmu_checker_module, jm_log_level_nothing, "FMU check summary:");

        jm_log(callbacks, fmu_checker_module, jm_log_level_nothing, "FMU reported:\n\t%u warning(s) and error(s)\nChecker reported:", cdata->num_fmu_messages);

        if(callbacks->log_level < jm_log_level_error) {
            jm_log(callbacks, fmu_checker_module, jm_log_level_nothing,
                   "\tWarnings and non-critical errors were ignored (log level: %s)", jm_log_level_to_string( callbacks->log_level ));
        }
        else {
            if(callbacks->log_level < jm_log_level_warning) {
                jm_log(callbacks, fmu_checker_module, jm_log_level_nothing,
                       "\tWarnings were ignored (log level: %s)", jm_log_level_to_string( callbacks->log_level ));
            }
            else
                jm_log(callbacks, fmu_checker_module, jm_log_level_nothing, "\t%u Warning(s)", cdata->num_warnings);
            if ((cdata->num_fatal > 0)) { cdata->num_errors=cdata->num_errors+cdata->num_fatal;
            }
            jm_log(callbacks, fmu_checker_module, jm_log_level_nothing, "\t%u Error(s)", cdata->num_errors);
        }
    }
    if((status == jm_status_success) && (cdata->num_fatal == 0)) {
        if(cdata->log_file && (cdata->log_file != stderr))
            fclose(cdata->log_file);
        return jm_status_success;
    }
    else {
        jm_log(callbacks, fmu_checker_module, jm_log_level_nothing,
               "\t%u Fatal error(s) occurred during processing",cdata->num_fatal);
        if(cdata->log_file && (cdata->log_file != stderr))
            fclose(cdata->log_file);
        return jm_status_error;
    }
    return jm_status_success;
}
