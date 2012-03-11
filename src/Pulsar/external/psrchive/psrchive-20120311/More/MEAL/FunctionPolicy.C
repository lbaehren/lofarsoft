#include "MEAL/FunctionPolicy.h"

static unsigned policy_instances = 0;

MEAL::FunctionPolicy::FunctionPolicy (Function* _context)
{
  policy_instances ++;
  // cerr << "policy instances " << policy_instances << endl;
  context = _context;
}

    //! Virtual destructor
MEAL::FunctionPolicy::~FunctionPolicy ()
{
  policy_instances --;
  // cerr << "policy instances " << policy_instances << endl;
}
