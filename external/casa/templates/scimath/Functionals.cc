/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*!
  \file Functionals.cc

  \ingroup scimath

  \brief Template instantiation for the <i>scimath</i> CASA module

  \author Lars B&auml;hren

  \date 2007/01/19
*/

// include of header files 

#include <casa/Arrays/Array.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Functionals/AbstractFunctionFactory.h>
#include <scimath/Functionals/Function1D.h>
#include <scimath/Functionals/SampledFunctional.h>
#include <scimath/Functionals/SpecificFunctionFactory.h>
#include <scimath/Functionals/WrapperBase.h>
#include <scimath/Functionals/WrapperData.h>
#include <scimath/Mathematics/AutoDiff.h>
#include <scimath/Mathematics/AutoDiffMath.h>
#include <scimath/Mathematics/AutoDiffA.h>
#include <scimath/Mathematics/AutoDiffIO.h>

// include of implementation files

#include <scimath/Functionals/Chebyshev.cc>
#include <scimath/Functionals/ChebyshevParam.cc>
#include <scimath/Functionals/Combi2Function.cc>
#include <scimath/Functionals/CombiFunction.cc>
#include <scimath/Functionals/CombiParam.cc>
#include <scimath/Functionals/CompiledFunction.cc>
#include <scimath/Functionals/CompiledParam.cc>
#include <scimath/Functionals/Compound2Function.cc>
#include <scimath/Functionals/CompoundFunction.cc>
#include <scimath/Functionals/CompoundParam.cc>
#include <scimath/Functionals/DiracDFunction.cc>
#include <scimath/Functionals/DiracDParam.cc>
#include <scimath/Functionals/EclecticFunctionFactory.cc>
#include <scimath/Functionals/EvenPolynomial.cc>
#include <scimath/Functionals/EvenPolynomial2.cc>
#include <scimath/Functionals/EvenPolynomialParam.cc>
#include <scimath/Functionals/Function.cc>
#include <scimath/Functionals/FunctionHolder.cc>
#include <scimath/Functionals/FunctionParam.cc>
#include <scimath/Functionals/FunctionWrapper.cc>
#include <scimath/Functionals/GNoiseFunction.cc>
#include <scimath/Functionals/GNoiseParam.cc>
#include <scimath/Functionals/Gaussian1D.cc>
#include <scimath/Functionals/Gaussian1D2.cc>
#include <scimath/Functionals/Gaussian1DParam.cc>
#include <scimath/Functionals/Gaussian2D.cc>
#include <scimath/Functionals/Gaussian2D2.cc>
#include <scimath/Functionals/Gaussian2DParam.cc>
#include <scimath/Functionals/Gaussian3D.cc>
#include <scimath/Functionals/Gaussian3D2.cc>
#include <scimath/Functionals/Gaussian3DParam.cc>
#include <scimath/Functionals/GaussianND.cc>
#include <scimath/Functionals/GaussianNDParam.cc>
#include <scimath/Functionals/HyperPlane.cc>
#include <scimath/Functionals/HyperPlane2.cc>
#include <scimath/Functionals/HyperPlaneParam.cc>
#include <scimath/Functionals/Interpolate1D.cc>
#include <scimath/Functionals/KaiserBFunction.cc>
#include <scimath/Functionals/KaiserBParam.cc>
#include <scimath/Functionals/MarshButterworthBandpass.cc>
#include <scimath/Functionals/MarshallableChebyshev.cc>
#include <scimath/Functionals/OddPolynomial.cc>
#include <scimath/Functionals/OddPolynomial2.cc>
#include <scimath/Functionals/OddPolynomialParam.cc>
#include <scimath/Functionals/Polynomial.cc>
#include <scimath/Functionals/Polynomial2.cc>
#include <scimath/Functionals/PolynomialParam.cc>
#include <scimath/Functionals/SPolynomial.cc>
#include <scimath/Functionals/SPolynomialParam.cc>
#include <scimath/Functionals/ScalarSampledFunctional.cc>
#include <scimath/Functionals/SimButterworthBandpass.cc>
#include <scimath/Functionals/SincFunction.cc>
#include <scimath/Functionals/SincParam.cc>
#include <scimath/Functionals/Sinusoid1D.cc>
#include <scimath/Functionals/Sinusoid1D2.cc>
#include <scimath/Functionals/Sinusoid1DParam.cc>
#include <scimath/Functionals/UnaryFunction.cc>
#include <scimath/Functionals/UnaryParam.cc>
#include <scimath/Functionals/WrapperParam.cc>

namespace casa {

  template class FunctionFactory<Double>;
  template class FunctionFactory<Float>;

  template class Chebyshev<DComplex>;
  template class Chebyshev<Complex>;
  template class Chebyshev<AutoDiff<DComplex> >;
  template class Chebyshev<AutoDiff<Complex> >;
  template class Chebyshev<AutoDiff<Double> >;
  template class Chebyshev<AutoDiff<Float> >;
  template class Chebyshev<AutoDiffA<Double> >;
  template class Chebyshev<AutoDiffA<Float> >;
  template class Chebyshev<Double>;
  template class Chebyshev<Float>;

  template class ChebyshevParam<AutoDiff<DComplex> >;
  template class ChebyshevParam<AutoDiff<Complex> >;
  template class ChebyshevParamModeImpl<AutoDiff<DComplex> >;
  template class ChebyshevParamModeImpl<AutoDiff<Complex> >;
  template class ChebyshevParam<DComplex>;
  template class ChebyshevParam<Complex>;
  template class ChebyshevParamModeImpl<DComplex>;
  template class ChebyshevParamModeImpl<Complex>;
  template class ChebyshevParam<AutoDiff<Double> >;
  template class ChebyshevParam<AutoDiff<Float> >; 
  template class ChebyshevParamModeImpl<AutoDiff<Double> >;
  template class ChebyshevParamModeImpl<AutoDiff<Float> >;
  template class ChebyshevParam<AutoDiffA<Double> >;
  template class ChebyshevParam<AutoDiffA<Float> >;
  template class ChebyshevParamModeImpl<AutoDiffA<Double> >;
  template class ChebyshevParamModeImpl<AutoDiffA<Float> >;
  template class ChebyshevParam<Double>;
  template class ChebyshevParam<Float>;
  template class ChebyshevParamModeImpl<Double>;
  template class ChebyshevParamModeImpl<Float>;

  template class CombiFunction<AutoDiff<DComplex> >;
  template class CombiFunction<AutoDiff<Complex> >;
  template class CombiFunction<AutoDiff<Double> >;
  template class CombiFunction<AutoDiff<Float> >;

  template class CombiFunction<DComplex>;
  template class CombiFunction<Complex>;
  template class CombiFunction<Double>;
  template class CombiFunction<Float>;

  template class CombiParam<AutoDiff<DComplex> >;
  template class CombiParam<AutoDiff<Complex> >;
  template class CombiParam<DComplex>;
  template class CombiParam<Complex>;
  template class CombiParam<AutoDiff<Double> >;
  template class CombiParam<AutoDiff<Float> >;
  template class CombiParam<Double>;
  template class CombiParam<Float>;

  template class CompiledFunction<AutoDiff<DComplex> >;
  template class CompiledFunction<AutoDiff<Complex> >;
  template class CompiledFunction<DComplex>;
  template class CompiledFunction<Complex>;
  template class CompiledFunction<AutoDiff<Double> >;
  template class CompiledFunction<AutoDiff<Float> >;
  template class CompiledFunction<Double>;
  template class CompiledFunction<Float>;
  template class CompiledFunction<AutoDiffA<Double> >;
  template class CompiledFunction<AutoDiffA<Float> >;

  template class CompiledParam<AutoDiff<DComplex> >;
  template class CompiledParam<AutoDiff<Complex> >;
  template class CompiledParam<DComplex>;
  template class CompiledParam<Complex>;
  template class CompiledParam<AutoDiff<Double> >;
  template class CompiledParam<AutoDiff<Float> >;
  template class CompiledParam<AutoDiffA<Double> >;
  template class CompiledParam<AutoDiffA<Float> >;
  template class CompiledParam<Double>;
  template class CompiledParam<Float>;

  template class CompoundFunction<AutoDiff<DComplex> >;
  template class CompoundFunction<AutoDiff<Complex> >;
  template class CompoundFunction<AutoDiff<Double> >;
  template class CompoundFunction<AutoDiff<Float> >;

  template class CompoundFunction<DComplex>;
  template class CompoundFunction<Complex>;
  template class CompoundFunction<Double>;
  template class CompoundFunction<Float>;

  template class CompoundParam<AutoDiff<DComplex> >;
  template class CompoundParam<AutoDiff<Complex> >;
  template class CompoundParam<DComplex>;
  template class CompoundParam<Complex>;
  template class CompoundParam<AutoDiff<Double> >;
  template class CompoundParam<AutoDiff<Float> >;
  template class CompoundParam<Double>;
  template class CompoundParam<Float>;

  template class DiracDFunction<AutoDiff<DComplex> >;
  template class DiracDFunction<AutoDiff<Complex> >;
  template class DiracDFunction<DComplex>;
  template class DiracDFunction<Complex>;
  template class DiracDFunction<AutoDiff<Double> >;
  template class DiracDFunction<AutoDiff<Float> >;
  template class DiracDFunction<Double>;
  template class DiracDFunction<Float>;

  template class DiracDParam<AutoDiff<DComplex> >;
  template class DiracDParam<AutoDiff<Complex> >;
  template class DiracDParam<DComplex>;
  template class DiracDParam<Complex>;
  template class DiracDParam<AutoDiff<Double> >;
  template class DiracDParam<AutoDiff<Float> >;
  template class DiracDParam<Double>;
  template class DiracDParam<Float>;

  template class EclecticFunctionFactory<Double>;
  template class EclecticFunctionFactory<Float>;

  template class EvenPolynomial<DComplex>;
  template class EvenPolynomial<Complex>;
  template class EvenPolynomial<Double>;

  template class EvenPolynomial<AutoDiff<DComplex> >;
  template class EvenPolynomial<AutoDiff<Complex> >;
  template class EvenPolynomial<AutoDiff<Double> >;

  template class EvenPolynomialParam<AutoDiff<DComplex> >;
  template class EvenPolynomialParam<AutoDiff<Complex> >;
  template class EvenPolynomialParam<DComplex>;
  template class EvenPolynomialParam<Complex>;
  template class EvenPolynomialParam<AutoDiff<Double> >;
  template class EvenPolynomialParam<Double>;

  template class Function<AutoDiff<Complex> >;
  template class Function<AutoDiff<DComplex> >;
  template class Function<AutoDiff<Double> >;
  template class Function<AutoDiff<Float> >;
  template class Function<AutoDiffA<Complex> >;
  template class Function<AutoDiffA<DComplex> >;
  template class Function<AutoDiffA<Double> >;
  template class Function<AutoDiffA<Float> >;
  template class Function<Double, Array<Double> >;
  template class Function<Complex>;
  template class Function<DComplex>;
  template class Function<Double, Float>;
  template class Function<Double>;
  template class Function<Float>;

  template class Function1D<AutoDiff<Complex> >;
  template class Function1D<AutoDiff<DComplex> >;
  template class Function1D<AutoDiff<Double> >;
  template class Function1D<AutoDiff<Float> >;
  template class Function1D<AutoDiffA<Complex> >;
  template class Function1D<AutoDiffA<DComplex> >;
  template class Function1D<AutoDiffA<Double> >;
  template class Function1D<AutoDiffA<Float> >;
  template class Function1D<Double, Array<Double> >;
  template class Function1D<Complex>;
  template class Function1D<DComplex>;
  template class Function1D<Double>;
  template class Function1D<Float>;

  template Bool FunctionHolder<DComplex>::getRecord<AutoDiff<DComplex> >(String &, Function<AutoDiff<DComplex> > * &, RecordInterface const &);
  template Bool FunctionHolder<DComplex>::getType<AutoDiff<DComplex> >(String &, Function<AutoDiff<DComplex> > * &, RecordInterface const &);
  template Bool FunctionHolder<DComplex>::getRecord<DComplex>(String &, Function<DComplex> * &, RecordInterface const &);
  template Bool FunctionHolder<DComplex>::getType<AutoDiff<DComplex> >(String &, Function<AutoDiff<DComplex> > * &);
  template Bool FunctionHolder<DComplex>::getType<DComplex>(String &, Function<DComplex> * &);
  template Bool FunctionHolder<DComplex>::getType<DComplex>(String &, Function<DComplex> * &, RecordInterface const &);
  template class FunctionHolder<DComplex>;
  template Bool FunctionHolder<Double>::getRecord<AutoDiff<Double> >(String &, Function<AutoDiff<Double> > * &, RecordInterface const &);
  template Bool FunctionHolder<Double>::getRecord<Double>(String &, Function<Double> * &, RecordInterface const &);
  template Bool FunctionHolder<Double>::getType<AutoDiff<Double> >(String &, Function<AutoDiff<Double> > * &);
  template Bool FunctionHolder<Double>::getType<AutoDiff<Double> >(String &, Function<AutoDiff<Double> > * &, RecordInterface const &);
  template Bool FunctionHolder<Double>::getType<Double>(String &, Function<Double> * &);
  template Bool FunctionHolder<Double>::getType<Double>(String &, Function<Double> * &, RecordInterface const &);
  template class FunctionHolder<Double>;

  template class FunctionParam<Complex>;
  template class FunctionParam<DComplex>;
  template class FunctionParam<AutoDiff<Complex> >;
  template class FunctionParam<AutoDiff<DComplex> >;
  template class FunctionParam<AutoDiff<Double> >;
  template class FunctionParam<AutoDiff<Float> >;
  template class FunctionParam<AutoDiffA<Complex> >;
  template class FunctionParam<AutoDiffA<DComplex> >;
  template class FunctionParam<AutoDiffA<Double> >;
  template class FunctionParam<AutoDiffA<Float> >;
  template class FunctionParam<Double>;
  template class FunctionParam<Float>;

  template class FunctionWrapper<AutoDiff<Double> >;
  template class FunctionWrapper<AutoDiff<Float> >;
  template class FunctionWrapper<Double>;
  template class FunctionWrapper<Float>;

  template class GNoiseFunction<AutoDiff<DComplex> >;
  template class GNoiseFunction<AutoDiff<Complex> >;
  template class GNoiseFunction<DComplex>;
  template class GNoiseFunction<Complex>;
  template class GNoiseFunction<AutoDiff<Double> >;
  template class GNoiseFunction<AutoDiff<Float> >;
  template class GNoiseFunction<Double>;
  template class GNoiseFunction<Float>;

  template class GNoiseParam<AutoDiff<DComplex> >;
  template class GNoiseParam<AutoDiff<Complex> >;
  template class GNoiseParam<DComplex>;
  template class GNoiseParam<Complex>;
  template class GNoiseParam<AutoDiff<Double> >;
  template class GNoiseParam<AutoDiff<Float> >;
  template class GNoiseParam<Double>;
  template class GNoiseParam<Float>;

  template class Gaussian1D<DComplex>;
  template class Gaussian1D<Complex>;
  template class Gaussian1D<AutoDiffA<Double> >;
  template class Gaussian1D<AutoDiffA<Float> >;
  template class Gaussian1D<Double>;
  template class Gaussian1D<Float>;

  template class Gaussian1D<AutoDiff<DComplex> >;
  template class Gaussian1D<AutoDiff<Complex> >;
  template class Gaussian1D<AutoDiff<Double> >;
  template class Gaussian1D<AutoDiff<Float> >;

  template class Gaussian1DParam<AutoDiff<DComplex> >;
  template class Gaussian1DParam<AutoDiff<Complex> >;
  template class Gaussian1DParam<DComplex>;
  template class Gaussian1DParam<Complex>;
  template class Gaussian1DParam<AutoDiff<Double> >;
  template class Gaussian1DParam<AutoDiff<Float> >;
  template class Gaussian1DParam<AutoDiffA<Double> >;
  template class Gaussian1DParam<AutoDiffA<Float> >;
  template class Gaussian1DParam<Double>;
  template class Gaussian1DParam<Float>;

  template class Gaussian2D<DComplex>;
  template class Gaussian2D<Complex>;
  template class Gaussian2D<AutoDiffA<Double> >;
  template class Gaussian2D<AutoDiffA<Float> >;
  template class Gaussian2D<Double>;
  template class Gaussian2D<Float>;

  template class Gaussian2D<AutoDiff<DComplex> >;
  template class Gaussian2D<AutoDiff<Complex> >;
  template class Gaussian2D<AutoDiff<Double> >;
  template class Gaussian2D<AutoDiff<Float> >;

  template class Gaussian2DParam<AutoDiff<DComplex> >;
  template class Gaussian2DParam<AutoDiff<Complex> >;
  template class Gaussian2DParam<DComplex>;
  template class Gaussian2DParam<Complex>;
  template class Gaussian2DParam<AutoDiff<Double> >;
  template class Gaussian2DParam<AutoDiff<Float> >;
  template class Gaussian2DParam<AutoDiffA<Double> >;
  template class Gaussian2DParam<AutoDiffA<Float> >;
  template class Gaussian2DParam<Double>;
  template class Gaussian2DParam<Float>;

  template class Gaussian3D<DComplex>;
  template class Gaussian3D<Complex>;
  template class Gaussian3D<Float>;
  template class Gaussian3D<Double>;

  template class Gaussian3D<AutoDiff<Float> >;
  template class Gaussian3D<AutoDiff<DComplex> >;
  template class Gaussian3D<AutoDiff<Complex> >;
  template class Gaussian3D<AutoDiff<Double> >;

  template class Gaussian3DParam<AutoDiff<DComplex> >;
  template class Gaussian3DParam<AutoDiff<Complex> >;
  template class Gaussian3DParam<DComplex>;
  template class Gaussian3DParam<Complex>;
  template class Gaussian3DParam<AutoDiff<Double> >;
  template class Gaussian3DParam<AutoDiff<Float> >;
  template class Gaussian3DParam<Double>;
  template class Gaussian3DParam<Float>;

  template class GaussianND<AutoDiff<DComplex> >;
  template class GaussianND<AutoDiff<Complex> >;
  template class GaussianND<DComplex>;
  template class GaussianND<Complex>;
  template class GaussianND<AutoDiff<Double> >;
  template class GaussianND<Double>;

  template class GaussianNDParam<AutoDiff<DComplex> >;
  template class GaussianNDParam<AutoDiff<Complex> >;
  template class GaussianNDParam<DComplex>;
  template class GaussianNDParam<Complex>;
  template class GaussianNDParam<AutoDiff<Double> >;
  template class GaussianNDParam<Double>;

  template class HyperPlane<DComplex>;
  template class HyperPlane<Complex>;
  template class HyperPlane<AutoDiffA<Double> >;
  template class HyperPlane<AutoDiffA<Float> >;
  template class HyperPlane<AutoDiffA<DComplex> >;
  template class HyperPlane<AutoDiffA<Complex> >;
  template class HyperPlane<Double>;
  template class HyperPlane<Float>;

  template class HyperPlane<AutoDiff<DComplex> >;
  template class HyperPlane<AutoDiff<Complex> >;
  template class HyperPlane<AutoDiff<Double> >;
  template class HyperPlane<AutoDiff<Float> >;

  template class HyperPlaneParam<AutoDiff<DComplex> >;
  template class HyperPlaneParam<AutoDiff<Complex> >;
  template class HyperPlaneParam<DComplex>;
  template class HyperPlaneParam<Complex>;
  template class HyperPlaneParam<AutoDiff<Double> >;
  template class HyperPlaneParam<AutoDiff<Float> >;
  template class HyperPlaneParam<AutoDiffA<Double> >;
  template class HyperPlaneParam<AutoDiffA<Float> >;
  template class HyperPlaneParam<AutoDiffA<Complex> >;
  template class HyperPlaneParam<AutoDiffA<DComplex> >;
  template class HyperPlaneParam<Double>;
  template class HyperPlaneParam<Float>;

  template class Interpolate1D<Double, Array<Double> >;
  template class Interpolate1D<Double, Float>;
  template class Function1D<Double, Float>;
  template class Interpolate1D<Double, Double>;

  template class KaiserBFunction<AutoDiff<DComplex> >;
  template class KaiserBFunction<AutoDiff<Complex> >;
  template class KaiserBFunction<DComplex>;
  template class KaiserBFunction<Complex>;
  template class KaiserBFunction<AutoDiff<Double> >;
  template class KaiserBFunction<AutoDiff<Float> >;
  template class KaiserBFunction<Double>;
  template class KaiserBFunction<Float>;

  template class KaiserBParam<AutoDiff<DComplex> >;
  template class KaiserBParam<AutoDiff<Complex> >;
  template class KaiserBParam<DComplex>;
  template class KaiserBParam<Complex>;
  template class KaiserBParam<AutoDiff<Double> >;
  template class KaiserBParam<AutoDiff<Float> >;
  template class KaiserBParam<Double>;
  template class KaiserBParam<Float>;

  template class SimButterworthBandpass<AutoDiff<Complex> >;
  template class SimButterworthBandpass<AutoDiff<DComplex> >;
  template class SimButterworthBandpass<AutoDiff<Double> >;
  template class SimButterworthBandpass<AutoDiff<Float> >;
  template class SimButterworthBandpass<AutoDiffA<Complex> >;
  template class SimButterworthBandpass<AutoDiffA<DComplex> >;
  template class SimButterworthBandpass<AutoDiffA<Double> >;
  template class SimButterworthBandpass<AutoDiffA<Float> >;
  template class SimButterworthBandpass<Complex>;
  template class SimButterworthBandpass<DComplex>;
  template class SimButterworthBandpass<Double>;
  template class SimButterworthBandpass<Float>;

  template class MarshButterworthBandpass<Double>;
  template class MarshButterworthBandpass<Float>;

  template class MarshallableChebyshev<Double>;
  template class MarshallableChebyshev<Float>;

  template class OddPolynomial<DComplex>;
  template class OddPolynomial<Complex>;
  template class OddPolynomial<Double>;

  template class OddPolynomial<AutoDiff<DComplex> >;
  template class OddPolynomial<AutoDiff<Complex> >;
  template class OddPolynomial<AutoDiff<Double> >;

  template class OddPolynomialParam<AutoDiff<DComplex> >;
  template class OddPolynomialParam<AutoDiff<Complex> >;
  template class OddPolynomialParam<DComplex>;
  template class OddPolynomialParam<Complex>;
  template class OddPolynomialParam<AutoDiff<Double> >;
  template class OddPolynomialParam<Double>;

  template class Polynomial<Complex>;
  template class Polynomial<DComplex>;
  template class Polynomial<AutoDiffA<Double> >;
  template class Polynomial<AutoDiffA<Float> >;
  template class Polynomial<Double>;
  template class Polynomial<Float>;

  template class Polynomial<AutoDiff<Complex> >;
  template class Polynomial<AutoDiff<DComplex> >;
  template class Polynomial<AutoDiff<Double> >; 
  template class Polynomial<AutoDiff<Float> >;

  template class PolynomialParam<AutoDiff<Complex> >;
  template class PolynomialParam<AutoDiff<DComplex> >;
  template class PolynomialParam<Complex>;
  template class PolynomialParam<DComplex>;
  template class PolynomialParam<AutoDiff<Double> >;
  template class PolynomialParam<AutoDiff<Float> >;
  template class PolynomialParam<AutoDiffA<Double> >;
  template class PolynomialParam<AutoDiffA<Float> >;
  template class PolynomialParam<Double>;
  template class PolynomialParam<Float>;

  template class SPolynomial<Complex>;
  template class SPolynomial<DComplex>;
  template class SPolynomial<AutoDiff<Complex> >;
  template class SPolynomial<AutoDiff<DComplex> >;
  template class SPolynomial<AutoDiff<Double> >;
  template class SPolynomial<AutoDiff<Float> >;
  template class SPolynomial<AutoDiffA<Double> >;
  template class SPolynomial<AutoDiffA<Float> >;
  template class SPolynomial<Double>;
  template class SPolynomial<Float>;

  template class SPolynomialParam<AutoDiff<Complex> >;
  template class SPolynomialParam<AutoDiff<DComplex> >;
  template class SPolynomialParam<Complex>;
  template class SPolynomialParam<DComplex>;
  template class SPolynomialParam<AutoDiff<Double> >;
  template class SPolynomialParam<AutoDiff<Float> >;
  template class SPolynomialParam<AutoDiffA<Double> >;
  template class SPolynomialParam<AutoDiffA<Float> >;
  template class SPolynomialParam<Double>;
  template class SPolynomialParam<Float>;

  template class SampledFunctional<Array<Double> >;
  template class SampledFunctional<Double>;
  template class SampledFunctional<Float>;

  template class ScalarSampledFunctional<Array<Double> >;
  template class ScalarSampledFunctional<Double>;
  template class ScalarSampledFunctional<Float>;

  template class SincFunction<AutoDiff<DComplex> >;
  template class SincFunction<AutoDiff<Complex> >;
  template class SincFunction<DComplex>;
  template class SincFunction<Complex>;
  template class SincFunction<AutoDiff<Double> >;
  template class SincFunction<AutoDiff<Float> >;
  template class SincFunction<Double>;
  template class SincFunction<Float>;

  template class SincParam<AutoDiff<DComplex> >;
  template class SincParam<AutoDiff<Complex> >;
  template class SincParam<DComplex>;
  template class SincParam<Complex>;
  template class SincParam<AutoDiff<Double> >;
  template class SincParam<AutoDiff<Float> >;
  template class SincParam<Double>;
  template class SincParam<Float>;

  template class Sinusoid1D<DComplex>;
  template class Sinusoid1D<Complex>;
  template class Sinusoid1D<AutoDiffA<Double> >;
  template class Sinusoid1D<AutoDiffA<Float> >;
  template class Sinusoid1D<Double>;
  template class Sinusoid1D<Float>;

  template class Sinusoid1D<AutoDiff<DComplex> >;
  template class Sinusoid1D<AutoDiff<Complex> >;
  template class Sinusoid1D<AutoDiff<Double> >;
  template class Sinusoid1D<AutoDiff<Float> >;

  template class Sinusoid1DParam<AutoDiff<DComplex> >;
  template class Sinusoid1DParam<AutoDiff<Complex> >;
  template class Sinusoid1DParam<DComplex>;
  template class Sinusoid1DParam<Complex>;
  template class Sinusoid1DParam<AutoDiff<Double> >;
  template class Sinusoid1DParam<AutoDiff<Float> >;
  template class Sinusoid1DParam<AutoDiffA<Double> >;
  template class Sinusoid1DParam<AutoDiffA<Float> >;
  template class Sinusoid1DParam<Double>;
  template class Sinusoid1DParam<Float>;

  template class SpecificFunctionFactory<Double, MarshButterworthBandpass<Double> >;
  template class SpecificFunctionFactory<Float, MarshButterworthBandpass<Float> >;
  template class SpecificFunctionFactory<Double, MarshallableChebyshev<Double> >;
  template class SpecificFunctionFactory<Float, MarshallableChebyshev<Float> >;

  template class UnaryFunction<AutoDiff<DComplex> >;
  template class UnaryFunction<AutoDiff<Complex> >;
  template class UnaryFunction<DComplex>;
  template class UnaryFunction<Complex>;
  template class UnaryFunction<AutoDiff<Double> >;
  template class UnaryFunction<AutoDiff<Float> >;
  template class UnaryFunction<Double>;
  template class UnaryFunction<Float>;

  template class UnaryParam<AutoDiff<DComplex> >;
  template class UnaryParam<AutoDiff<Complex> >;
  template class UnaryParam<DComplex>;
  template class UnaryParam<Complex>;
  template class UnaryParam<AutoDiff<Double> >;
  template class UnaryParam<AutoDiff<Float> >;
  template class UnaryParam<Double>;
  template class UnaryParam<Float>;

  template class WrapperBase<AutoDiff<Double> >;
  template class WrapperBase<AutoDiff<Float> >;
  template class WrapperBase<Double>;
  template class WrapperBase<Float>;

  template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, AutoDiff<Double>, True, True>;
  template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, AutoDiff<Double>, True, False>;
  template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, AutoDiff<Double>, False, True>;
  template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, AutoDiff<Double>, False, False>;
  template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, Vector<AutoDiff<Double> >, True, True>;
  template class WrapperData<AutoDiff<Double>, AutoDiff<Double>, Vector<AutoDiff<Double> >, False, True>;
  template class WrapperData<AutoDiff<Double>, Vector<AutoDiff<Double> >, AutoDiff<Double>, True, True>;
  template class WrapperData<AutoDiff<Double>, Vector<AutoDiff<Double> >, AutoDiff<Double>, True, False>;
  template class WrapperData<AutoDiff<Double>, Vector<AutoDiff<Double> >, Vector<AutoDiff<Double> >, True, True>;
  template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, AutoDiff<Float>, True, True>;
  template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, AutoDiff<Float>, True, False>;
  template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, AutoDiff<Float>, False, True>;
  template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, AutoDiff<Float>, False, False>;
  template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, Vector<AutoDiff<Float> >, True, True>;
  template class WrapperData<AutoDiff<Float>, AutoDiff<Float>, Vector<AutoDiff<Float> >, False, True>;
  template class WrapperData<AutoDiff<Float>, Vector<AutoDiff<Float> >, AutoDiff<Float>, True, True>;
  template class WrapperData<AutoDiff<Float>, Vector<AutoDiff<Float> >, AutoDiff<Float>, True, False>;
  template class WrapperData<AutoDiff<Float>, Vector<AutoDiff<Float> >, Vector<AutoDiff<Float> >, True, True>;
  template class WrapperData<Double, Double, Double, True, True>;
  template class WrapperData<Double, Double, Double, True, False>;
  template class WrapperData<Double, Double, Double, False, True>;
  template class WrapperData<Double, Double, Double, False, False>;
  template class WrapperData<Double, Double, Vector<Double>, True, True>;
  template class WrapperData<Double, Double, Vector<Double>, False, True>;
  template class WrapperData<Double, Vector<Double>, Double, True, True>;
  template class WrapperData<Double, Vector<Double>, Double, True, False>;
  template class WrapperData<Double, Vector<Double>, Vector<Double>, True, True>;
  template class WrapperData<Float, Float, Float, True, True>;
  template class WrapperData<Float, Float, Float, True, False>;
  template class WrapperData<Float, Float, Float, False, True>;
  template class WrapperData<Float, Float, Float, False, False>;
  template class WrapperData<Float, Float, Vector<Float>, True, True>;
  template class WrapperData<Float, Float, Vector<Float>, False, True>;
  template class WrapperData<Float, Vector<Float>, Float, True, True>;
  template class WrapperData<Float, Vector<Float>, Float, True, False>;
  template class WrapperData<Float, Vector<Float>, Vector<Float>, True, True>;

  template class WrapperParam<AutoDiff<Double> >;
  template class WrapperParam<AutoDiff<Float> >;
  template class WrapperParam<Double>;
  template class WrapperParam<Float>;

     } // namescape casa -- END
