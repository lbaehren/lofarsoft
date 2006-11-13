//#  NoiseTransfer.cc: global methods needed to calculate the noise transfer.
//#
//#  Copyright (C) 2002-2006
//#  ASTRON (Netherlands Foundation for Research in Astronomy)
//#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
//#
//#  This program is free software; you can redistribute it and/or modify
//#  it under the terms of the GNU General Public License as published by
//#  the Free Software Foundation; either version 2 of the License, or
//#  (at your option) any later version.
//#
//#  This program is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#  GNU General Public License for more details.
//#
//#  You should have received a copy of the GNU General Public License
//#  along with this program; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//#
//#  $Id: NoiseTransfer.cc,v 1.10 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/NoiseTransfer.h>
#include <Sinfoni/Math.h>
#include <Sinfoni/Assert.h>
#include <Sinfoni/Debug.h>

using namespace std;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      // This unnamed namespace bundles the methods required to compute the
      // noise transfer.
      namespace
      {
        // The integration time \f$T_e\f$.
        const Real integrationTime = 1.0/350;

        // The loop delay \f$\tau\f$.
        const Real loopDelay = 0.31e-3;

        // Compute the system transfer function, which is defined as:
        // \f[
        //   H_{sys}(f,g_i) = \frac{g_i}{2\pi i f T_e} \exp{(-2\pi i f \tau)}
        // \f]
        // where \f$f\f$ is the frequency \a freq, \f$g_i\f$ is the gain of
        // the electrode \a gain, \f$T_e\f$ is the integration time, and
        // \f$\tau\f$ is the loop delay.
        inline Complex H_sys(Real freq, Real gain)
        {
          Real arg(2*pi*freq);
          return ( gain / integrationTime ) *
            ( polar(Real(1.0), -arg*loopDelay) / Complex(0.0, arg) );
        }
        
        // Compute the transfer function of the wave front sensor, which is
        // defined as:
        // \f[
        //   H_{WFS}(f) = \mathrm{sinc}(\pi f T_e) \exp{(-i\pi f T_e)}
        // \f]
        // where \f$f\f$ is the frequency \a freq, and \f$T_e\f$ is the
        // integration time \a integrationTime.
        inline Complex H_wfs(Real freq)
        {
          Real arg = freq*integrationTime;
          return sinc(arg) * polar(Real(1.0), -pi*arg);
        }

        // Compute the open loop transfer function, which is defined as:
        // \f[
        //   H_{ol}(f,g_i) = H_{WFS}(f) H_{sys}(f,g_i)
        // \f]
        // where \f$f\f$ is the frequency \a freq, \f$g_i\f$ is the gain of
        // the electrode \a gain, \f$H_{WFS}(f)\f$ is the tranfer function of
        // the wave front sensor, and \f$H_{sys}(f,g_i)\f$ is the system
        // tranfer function.
        inline Complex H_ol(Real freq, Real gain)
        {
          return H_sys(freq,gain) * H_wfs(freq);
        }

        // Compute the noise transfer function, which is defined as:
        // \f[
        //   H_n(f,g_i) = \frac{H_{sys}(f,g_i)}{1 + H_{ol}(f,g_i)}
        // \f]
        // where \f$f\f$ is the frequency \a freq, \f$g_i\f$ is the gain of
        // the electrode \a gain, \f$H_{sys}(f,g_i)\f$ is the system transfer
        // function, and \f$H_{ol}(f,g_i)\f$ is the open loop transfer
        // function.
        inline Complex H_n(Real freq, Real gain)
        {
          return H_sys(freq, gain) / (Real(1.0) + H_ol(freq, gain));
        }

        // Compute the upper frequency that must be used in integrating the
        // noise transfer function.
        // 
        // \note The current implementation is based on estimates that were
        // taken from a graph.
        inline Real f_max(Real gain)
        {
          Assert(0 <= gain && gain <= 1.16);
          if (gain > 1.0)   return 133.6;
          if (gain > 0.5)   return 117.5;
          if (gain > 0.25)  return  42.0;
          if (gain > 0.165) return  17.0;
          if (gain > 0.116) return  10.1;
          return 7.0;
        }

      } //# namespace


      Real noiseTransfer(Real gain)
      {
        //# Step-size
        const Real df(0.05);

        //# Running sum of the total noise transfer
        Real sum(0.0);

        //# \c fmax must be calculated, based on the value of \a gain.
        Real fmax(f_max(gain));

        //# Add half a step-size to fmax so we can loop up to fmax inclusive.
        fmax += 0.5*df;

        for(Real f=df; f<fmax; f+=df) {
          sum += df * norm(H_n(f, gain));
        }

        return sum;
      }


      VectorReal noiseTransfer(const VectorReal& gain)
      {
        DBG_TRACE("Computing noise transfer function ...");

        //# Allocate a vector to hold the results
        VectorReal result(gain.shape());

        //# Loop over all elements of \a gain.
        for (Integer i = 0; i < gain.rows(); i++) {
          result(i) = noiseTransfer(gain(i));
        }

        //# Return the result.
        return result;
      }

    } //# namespace PSF

  } //# namespace SINFONI

} //# namespace ASTRON

