/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Projection.h"
#include "MEAL/Composite.h"

//! Default constructor
MEAL::Projection::Projection (Function* _model, Composite* _meta)
{
  model = _model;

  if (_model && _meta)
    _meta->map (this);
}

MEAL::Projection::~Projection ()
{
}

//! Return the mapped index
unsigned MEAL::Projection::get_imap (unsigned index) const
{
  if (!model)
    throw Error (InvalidState, "MEAL::Projection::get_imap",
		 "no Function");

  if (!meta)
    throw Error (InvalidState, "MEAL::Projection::get_imap",
		 "no Composite");

  if (meta->remap_needed)
    meta->remap ();

  if (model->get_nparam() != imap.size())
    throw Error (InvalidRange, "MEAL::Projection::get_imap",
                 "%s.nparam=%d >= nmap=%d", model->get_name().c_str(),
		 model->get_nparam(), imap.size());

  if (index >= imap.size())
    throw Error (InvalidRange, "MEAL::Projection::get_imap",
		 "index=%d >= nmap=%d", index, imap.size());

  return imap[index];
}

MEAL::Function* MEAL::Projection::get_Function ()
{
  return model;
}

void MEAL::Projection::set_Function (Function* _model)
{
  if (model.ptr() == _model)
    return;

  model = _model;

  imap.resize (0);
  meta = 0;
}
