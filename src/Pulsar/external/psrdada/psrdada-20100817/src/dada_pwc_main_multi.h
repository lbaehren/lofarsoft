#ifndef __DADA_PWC_MAIN_MULTI_H
#define __DADA_PWC_MAIN_MULTI_H

/* ************************************************************************

   dada_pwc_main_multi_t - a struct and associated routines for creation and
   execution of a dada primary write client main loop

   ************************************************************************ */

#include "dada_pwc.h"
#include "multilog.h"
#include "dada_hdu.h"
#include "ipcio.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct dada_pwc_main_multi {

    /*! The primary write client control connection */
    dada_pwc_t* pwc;

    /*! The status and error logging interface */
    multilog_t* log;

		/*! The HDU blocks */
		dada_hdu_t ** hdus;

    /*! Pointer to the function that starts data transfer */
    time_t (*start_function) (struct dada_pwc_main_multi*, time_t utc);

    /*! Pointer to the function that returns an array of  data buffers */
    void** (*buffer_function) (struct dada_pwc_main_multi*, uint64_t* size);

    /*! Pointer to the function that stops data transfer */
    int (*stop_function) (struct dada_pwc_main_multi*);

    /*! Pointer to the function that handles data transfer error*/
    int (*error_function) (struct dada_pwc_main_multi*);

    /*! Pointer to the function that checks header validity */
    int (*header_valid_function) (struct dada_pwc_main_multi*);

    /*! Additional context information */
    void* context;

    /*! The current command from the PWC control connection */
    dada_pwc_command_t command;

    /*! verbosity level */
    int verbose;

    /*! flag to determine if we have a valid header */
    int header_valid;

		/*! number of HDU blocks */
		unsigned num_hdus;

  } dada_pwc_main_multi_t;

  /*! Create a new DADA primary write client main loop */
  dada_pwc_main_multi_t* dada_pwc_main_multi_create (unsigned num_hdus);

  /*! Destroy a DADA primary write client main loop */
  void dada_pwc_main_multi_destroy (dada_pwc_main_multi_t* pwcm);

  /*! Run the DADA primary write client main loop */
  int dada_pwc_main_multi (dada_pwc_main_multi_t* pwcm);

  /*! process error value, and set state accordingly */
  void dada_pwc_main_multi_process_error(dada_pwc_main_multi_t* pwcm, int rval); 


#ifdef __cplusplus
	   }
#endif

#endif
