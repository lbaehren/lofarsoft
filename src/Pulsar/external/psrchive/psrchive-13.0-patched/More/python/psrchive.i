%module psrchive
%{
#define SWIG_FILE_WITH_INIT
#include "numpy/noprefix.h"

#include "Reference.h"
#include "Pulsar/IntegrationManager.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ProfileAmps.h"
#include "Pulsar/Profile.h"
%}

// Language independent exception handler
%include exception.i       
%include std_string.i

using namespace std;

%exception {
    try {
        $action
    } catch(Error& error) {
        // Deal with out-of-range errors
        if (error.get_code()==InvalidRange)
            SWIG_exception(SWIG_IndexError, error.get_message().c_str());
        else
            SWIG_exception(SWIG_RuntimeError,error.get_message().c_str());
    } catch(...) {
        SWIG_exception(SWIG_RuntimeError,"Unknown exception");
    }
}

%init %{
  import_array();
%}

// Declare functions that return a newly created object
// (Helps memory management)
%newobject Pulsar::Archive::new_Archive;
%newobject Pulsar::Archive::load;
%newobject Pulsar::Archive::clone;
%newobject Pulsar::Archive::extract;
%newobject Pulsar::Archive::total;
%newobject Pulsar::Integration::clone;
%newobject Pulsar::Integration::total;
%newobject Pulsar::Profile::clone;

// Track any pointers handed off to python with a global list
// of Reference::To objects.  Prevents the C++ routines from
// prematurely destroying objects by effectively making python
// variables act like Reference::To pointers.
%feature("ref")   Reference::Able "pointer_tracker_add($this);"
%feature("unref") Reference::Able "pointer_tracker_remove($this);"
%header %{
std::vector< Reference::To<Reference::Able> > _pointer_tracker;
void pointer_tracker_add(Reference::Able *ptr) {
    _pointer_tracker.push_back(ptr);
}
void pointer_tracker_remove(Reference::Able *ptr) {
    std::vector< Reference::To<Reference::Able> >::iterator it;
    for (it=_pointer_tracker.begin(); it<_pointer_tracker.end(); it++) 
        if ((*it).ptr() == ptr) {
            _pointer_tracker.erase(it);
            break;
        }
}
%}

// SWIG can't handle nested classes, so ignore them
%ignore Pulsar::Archive::get_extension(unsigned);
%ignore Pulsar::Archive::get_extension(unsigned) const;
%ignore Pulsar::Archive::add_extension(Extension*);
%ignore Pulsar::Integration::get_extension(unsigned);
%ignore Pulsar::Integration::get_extension(unsigned) const;
%ignore Pulsar::Integration::add_extension(Extension*);

// Also does not use the assignment operator
%ignore Pulsar::Archive::operator=(const Archive&);
%ignore Pulsar::Integration::operator=(const Integration&);
%ignore Pulsar::Profile::operator=(const Profile&);
%ignore Pulsar::IntegrationManager::operator=(const IntegrationManager&);

// Also does not distinguish between const and non-const overloaded methods
%ignore Pulsar::Archive::get_Profile(unsigned,unsigned,unsigned) const;
%ignore Pulsar::Archive::expert() const;
%ignore Pulsar::Archive::get_type() const;
%ignore Pulsar::Archive::get_state() const;
%ignore Pulsar::Archive::get_scale() const;
%ignore Pulsar::Archive::get_basis() const;
%ignore Pulsar::Integration::get_Profile(unsigned,unsigned) const;
%ignore Pulsar::Integration::new_PolnProfile(unsigned) const;
%ignore Pulsar::Integration::get_epoch() const;
%ignore Pulsar::IntegrationManager::get_Integration(unsigned) const;
%ignore Pulsar::IntegrationManager::get_last_Integration() const;
%ignore Pulsar::IntegrationManager::get_first_Integration() const;
%ignore Pulsar::ProfileAmps::get_amps() const;

// Stokes class not wrapped yet, so ignore it
%ignore Pulsar::Integration::get_Stokes(unsigned,unsigned) const;

// Same for Option
%ignore Pulsar::Profile::rotate_in_phase_domain;
%ignore Pulsar::Profile::transition_duty_cycle;
%ignore Pulsar::Profile::default_duty_cycle;

// Also Functor
%ignore Pulsar::Profile::peak_edges_strategy;
%ignore Pulsar::Profile::baseline_strategy;
%ignore Pulsar::Profile::onpulse_strategy;
%ignore Pulsar::Profile::snr_strategy;

// Header files included here will be wrapped
%include "ReferenceAble.h"
%include "Pulsar/Container.h"
%include "Pulsar/IntegrationManager.h"
%include "Pulsar/Archive.h"
%include "Pulsar/Integration.h"
%include "Pulsar/ProfileAmps.h"
%include "Pulsar/Profile.h"

// Python-specific extensions to the classes:

%extend Pulsar::Profile
{
    // Allow indexing
    float __getitem__(int i)
    {
        return self->get_amps()[i];
    }
    void __setitem__(int i, float val)
    {
        self->get_amps()[i] = val;
    }
    int __len__()
    {
        return self->get_nbin();
    }

    // Return a numpy array view of the data.
    // This points to the actual Profile data, not a separate copy.
    PyObject *get_amps()
    {
        PyArrayObject *arr;
        float *ptr;
        npy_intp n;
        
        n = self->get_nbin();
        ptr = self->get_amps();
        arr = (PyArrayObject *)                                         \
            PyArray_SimpleNewFromData(1, &n, PyArray_FLOAT, (char *)ptr);
        if (arr == NULL) return NULL;
        arr->flags |= OWN_DATA;
        PyArray_INCREF(arr);
        return (PyObject *)arr;
    }
}

%extend Pulsar::Integration
{
    // Return MJD as double
    // TODO: probably can do this better with typemap?
    double get_epoch() { return self->get_epoch().in_days(); }

    // Return baseline_stats as numpy arrays
    PyObject *baseline_stats() {

        // Call C++ routine
        std::vector< std::vector< Estimate<double> > > mean;
        std::vector< std::vector<double> > var;
        self->baseline_stats(&mean, &var);
        npy_intp size[2]; // Chan and pol
        size[0] = mean.size();
        size[1] = mean[0].size();

        // Pack values into new numpy arrays
        PyArrayObject *npy_mean, *npy_var;
        npy_mean = (PyArrayObject *)PyArray_SimpleNew(2, size, PyArray_DOUBLE);
        npy_var  = (PyArrayObject *)PyArray_SimpleNew(2, size, PyArray_DOUBLE);
        for (int ii=0; ii<size[0]; ii++) 
            for (int jj=0; jj<size[1]; jj++) {
                ((double *)npy_mean->data)[ii*size[1]+jj] = mean[ii][jj].get_value();
                ((double *)npy_var->data)[ii*size[1]+jj] = var[ii][jj];
            }

        // Pack arrays into tuple for output
        PyTupleObject *result = (PyTupleObject *)PyTuple_New(2);
        PyTuple_SetItem((PyObject *)result, 0, (PyObject *)npy_mean);
        PyTuple_SetItem((PyObject *)result, 1, (PyObject *)npy_var);
        return (PyObject *)result;
    }

    // Return cal levels as numpy arrays
    PyObject *cal_levels() {

        // Call C++ routine for values
        std::vector< std::vector< Estimate<double> > > hi, lo;
        self->cal_levels(hi, lo);
        npy_intp dims[2]; // Chan and pol
        dims[0] = hi.size();
        dims[1] = hi[0].size();

        // Create, fill numpy arrays
        PyArrayObject *hi_arr, *lo_arr, *sig_hi_arr, *sig_lo_arr;
        hi_arr = (PyArrayObject *)PyArray_SimpleNew(2, dims, PyArray_DOUBLE);
        lo_arr = (PyArrayObject *)PyArray_SimpleNew(2, dims, PyArray_DOUBLE);
        sig_hi_arr = (PyArrayObject *)PyArray_SimpleNew(2, dims, 
            PyArray_DOUBLE);
        sig_lo_arr = (PyArrayObject *)PyArray_SimpleNew(2, dims, 
            PyArray_DOUBLE);
        for (int ii=0; ii<dims[0]; ii++) {
            for (int jj=0; jj<dims[1]; jj++) {
                ((double *)hi_arr->data)[ii*dims[1]+jj] = 
                    hi[ii][jj].get_value();
                ((double *)lo_arr->data)[ii*dims[1]+jj] = 
                    lo[ii][jj].get_value();
                ((double *)sig_hi_arr->data)[ii*dims[1]+jj] = 
                    sqrt(hi[ii][jj].get_variance());
                ((double *)sig_lo_arr->data)[ii*dims[1]+jj] = 
                    sqrt(lo[ii][jj].get_variance());
            }
        }

        // Pack arrays into tuple
        PyTupleObject *result = (PyTupleObject *)PyTuple_New(4);
        PyTuple_SetItem((PyObject *)result, 0, (PyObject *)hi_arr);
        PyTuple_SetItem((PyObject *)result, 1, (PyObject *)lo_arr);
        PyTuple_SetItem((PyObject *)result, 2, (PyObject *)sig_hi_arr);
        PyTuple_SetItem((PyObject *)result, 3, (PyObject *)sig_lo_arr);
        return (PyObject *)result;
    }

}

%extend Pulsar::Archive
{

    // Allow indexing of Archive objects
    Pulsar::Integration *__getitem__(int i)
    {
        return self->get_Integration(i);
    }
    int __len__()
    {
        return self->get_nsubint();
    }

    // String representation of the Archive
    std::string __str__()
    {
        return "PSRCHIVE Archive object: " + self->get_filename();
    }

    // String representation of various enums
    std::string get_type() { return Source2string(self->get_type()); }
    std::string get_state() { return State2string(self->get_state()); }
    std::string get_basis() { return Basis2string(self->get_basis()); }
    std::string get_scale() { return Scale2string(self->get_scale()); }
    void convert_state(std::string s) { 
        self->convert_state(Signal::string2State(s)); 
    }

    // Return a copy of all the data as a numpy array
    PyObject *get_data()
    {
        PyArrayObject *arr;
        npy_intp ndims[4];  // nsubint, npol, nchan, nbin
        int ii, jj, kk;
        
        ndims[0] = self->get_nsubint();
        ndims[1] = self->get_npol();
        ndims[2] = self->get_nchan();
        ndims[3] = self->get_nbin();
        arr = (PyArrayObject *)PyArray_SimpleNew(4, ndims, PyArray_FLOAT);
        for (ii = 0 ; ii < ndims[0] ; ii++)
            for (jj = 0 ; jj < ndims[1] ; jj++)
                for (kk = 0 ; kk < ndims[2] ; kk++)
                    memcpy(arr->data + sizeof(float) * 
                           (ndims[3] * (kk + ndims[2] * (jj + ndims[1] * ii))), 
                           self->get_Profile(ii, jj, kk)->get_amps(),
                           ndims[3]*sizeof(float));
        return (PyObject *)arr;
    }

    // Return a copy of all the weights as a numpy array
    PyObject *get_weights()
    {
        PyArrayObject *arr;
        npy_intp ndims[2];  // nsubint, nchan
        int ii, jj;
        
        ndims[0] = self->get_nsubint();
        ndims[1] = self->get_nchan();
        arr = (PyArrayObject *)PyArray_SimpleNew(2, ndims, PyArray_FLOAT);
        for (ii = 0 ; ii < ndims[0] ; ii++)
            for (jj = 0 ; jj < ndims[1] ; jj++)
                ((float *)arr->data)[ii*ndims[1]+jj] = \
                    self->get_Integration(ii)->get_weight(jj);
        return (PyObject *)arr;
    }
}
