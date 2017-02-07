#ifndef Elements_Quadrupole_h
#define Elements_Quadrupole_h

#include "ElementBase.h"
#include "Drift.h"

namespace Hector
{
  namespace Element
  {
    /// A generic quadrupole object
    class Quadrupole : public ElementBase
    {
      public:
        /// (virtual) class constructor
        Quadrupole( const std::string& name ) : ElementBase( ElementBase::Quadrupole, name ) {}

        virtual Quadrupole* clone() const = 0;
        virtual CLHEP::HepMatrix matrix( float, float, int ) const = 0;

      protected:
    };

    /// Horizontal quadrupole object builder
    class HorizontalQuadrupole : public Quadrupole
    {
      public:
        /// Class constructor
        HorizontalQuadrupole( const std::string& name ) : Quadrupole( name ) {}

        HorizontalQuadrupole* clone() const {
          HorizontalQuadrupole* out = new HorizontalQuadrupole( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * \cos{\omega} & -\sqrt{k}\sin{\omega} & 0 & 0 & 0 & 0\\
         * \frac{1}{\sqrt{k}}\sin{\omega} & \cos{\omega} & 0 & 0 & 0 & 0\\
         * 0 & 0 & \cosh{\omega} & \sqrt{k}\sinh{\omega} & 0 & 0\\
         * 0 & 0 & \frac{1}{\sqrt{k}}\sinh{\omega} & \cosh{\omega} & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$ and \f$ \omega \equiv \omega(k,l) = l \sqrt{|k|} \f$
         */
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };

    /// Vertical quadrupole object builder
    class VerticalQuadrupole : public Quadrupole
    {
      public:
        /// Class constructor
        VerticalQuadrupole( const std::string& name ) : Quadrupole( name ) {}

        VerticalQuadrupole* clone() const {
          VerticalQuadrupole* out = new VerticalQuadrupole( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * \cosh{\omega} & \sqrt{k}\sinh{\omega} & 0 & 0 & 0 & 0\\
         * \frac{1}{\sqrt{k}}\sinh{\omega} & \cosh{\omega} & 0 & 0 & 0 & 0\\
         * 0 & 0 & \cos{\omega} & -\sqrt{k}\sin{\omega} & 0 & 0\\
         * 0 & 0 & \frac{1}{\sqrt{k}}\sin{\omega} & \cos{\omega} & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$ and \f$ \omega \equiv \omega(k,l) = l \sqrt{|k|} \f$
         */
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };
  }
}

#endif
