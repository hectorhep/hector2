#ifndef Elements_Dipole_h
#define Elements_Dipole_h

#include "ElementBase.h"
#include "Drift.h"
#include "core/Exception.h"

namespace Hector
{
  namespace Element
  {
    /// A generic dipole object
    class Dipole : public ElementBase
    {
      public:
        /// (virtual) class constructor
        Dipole( const ElementBase::Type& type, const std::string& name ) : ElementBase( type, name ) {}

        virtual Dipole* clone() const = 0;
        virtual CLHEP::HepMatrix matrix( float, float, int ) const = 0;

      protected:
    };

    /// Rectangular dipole object builder
    class RectangularDipole : public Dipole
    {
      public:
        /// Class constructo
        RectangularDipole( const std::string& name ) : Dipole( ElementBase::RectangularDipole, name ) {}

        RectangularDipole* clone() const {
          RectangularDipole* out = new RectangularDipole( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }
        /** \note \f$
          \mathbf{M} = \left(
          \begin{array}{cccccc}
          \cos(l/r) & \frac{-1}{r} \sin(l/r) & 0 & 0 & 0 & 0\\
          r \sin(l/r) & \cos(l/r) & 0 & 0 & 0 & 0\\
          0 & 0 & 1 & 0 & 0 & 0\\
          0 & 0 & l & 1 & 0 & 0\\
          2r \sin^2(l/2r)/E_{\mathrm{beam}} & \sin(l/r)/E_{\mathrm{beam}} & 0 & 0 & 1 &0\\
          0 & 0 & 0 & 0 & 0 & 1\\
          \end{array}
          \right)
          \f$
          assuming \f$ 1/r \equiv k = k_{0} \cdot \frac{p_{0}}{p_{0} - dp} \cdot \frac{q_{\mathrm{particle}}}{q_{\mathrm{beam}}} \f$ and \f$ E_{\mathrm{beam}}=7000\f$ GeV.
        */
        /// \warning numerical sensitivity with \f$ r(1-\cos(l/r))/E_{\mathrm{beam}}\f$.
        ///  Using \f$ 2\sin^2(x/2) = 1-\cos(x)\f$ instead (see the variable called "simp")
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };

    /// Sector dipole object builder
    class SectorDipole : public Dipole
    {
      public:
        /// Class constructor
        SectorDipole( const std::string& name ) : Dipole( ElementBase::SectorDipole, name ) {}

        SectorDipole* clone() const {
          SectorDipole* out = new SectorDipole( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }

        /// \note Matrix depends if the bending is on or off.
        /** \note \f$
          \mathbf{M}_{\mathrm{bending-off}} = \left(
          \begin{array}{cccccc}
          \cos(l/r) & \frac{-1}{r} \sin(l/r) & 0 & 0 & 0 & 0\\
          r \sin(l/r) & \cos(l/r) & 0 & 0 & 0 & 0\\
          0 & 0 & 1 & 0 & 0 & 0\\
          0 & 0 & l & 1 & 0 & 0\\
          0 & 0 & 0 & 0 & 1 & 0\\
          0 & 0 & 0 & 0 & 0 & 1\\
          \end{array}
          \right),
          \f$\f$
          \mathbf{M}_{\mathrm{bending-on}} = \left(
          \begin{array}{cccccc}
          \cos(l/r) & \frac{-1}{r} \sin(l/r) & 0 & 0 & 0 & 0\\
          r \sin(l/r) & \cos(l/r) & 0 & 0 & 0 & 0\\
          0 & 0 & 1 & 0 & 0 &0\\
          0 & 0 & l & 1 & 0 &0\\
          2r \sin^2(l/2r)/E_{\mathrm{beam}} & \sin(l/r)/E_{\mathrm{beam}} & 0 & 0 & 1 & 0\\
          0 & 0 & 0 & 0 & 0 & 1\\
          \end{array}
          \right)
          \f$
         assuming \f$ 1/r \equiv k =  k_{0} \cdot \frac{p_{0}}{p_{0} - dp} \cdot \frac{q_{\mathrm{particle}}}{q_{\mathrm{beam}}} \f$
        */
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };
  }
}

#endif
