/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Complex2Math.h"

#include "MEAL/SumRule.h"
#include "MEAL/NegationRule.h"
#include "MEAL/ProductRule.h"
#include "MEAL/InverseRule.h"
#include "MEAL/RuleMath.h"

Reference::To<MEAL::Complex2>
operator + (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b)
{
  return MEAL::Sum<MEAL::Complex2> (a, b);
}

Reference::To<MEAL::Complex2>
operator - (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b)
{
  return MEAL::Difference<MEAL::Complex2> (a, b);
}

Reference::To<MEAL::Complex2>
operator * (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b)
{
  return MEAL::Product<MEAL::Complex2> (a, b);
}

Reference::To<MEAL::Complex2>
operator / (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b)
{
  return MEAL::Quotient<MEAL::Complex2> (a, b);
}

