#ifndef Hector_Elements_Quadrupole_h
#define Hector_Elements_Quadrupole_h

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
        Quadrupole( const Type& type, const std::string& name, float spos, float length, float mag_str ) :
          ElementBase( type, name, spos, length ) {
          setMagneticStrength( mag_str );
        }

        virtual Quadrupole* clone() const = 0;
        virtual CLHEP::HepMatrix matrix( float, float, int ) const = 0;

      protected:
    };

    /// Horizontal quadrupole object builder
    class HorizontalQuadrupole : public Quadrupole
    {
      public:
        /// Class constructor
        HorizontalQuadrupole( const std::string& name, float spos, float length, float mag_str ) :
          Quadrupole( anHorizontalQuadrupole, name, spos, length, mag_str ) {}

        HorizontalQuadrupole* clone() const { return new HorizontalQuadrupole( *this ); }
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * \cos{\omega} & \frac{1}{\sqrt{k}}\sin{\omega} & 0 & 0 & 0 & 0\\
         * -\sqrt{k}\sin{\omega} & \cos{\omega} & 0 & 0 & 0 & 0\\
         * 0 & 0 & \cosh{\omega} & \frac{1}{\sqrt{k}}\sinh{\omega} & 0 & 0\\
         * 0 & 0 & \sqrt{k}\sinh{\omega} & \cosh{\omega} & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$ and \f$ \omega \equiv \omega(k,L) = L \sqrt{|k|} \f$
         */
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };

    /// Vertical quadrupole object builder
    class VerticalQuadrupole : public Quadrupole
    {
      public:
        /// Class constructor
        VerticalQuadrupole( const std::string& name, float spos, float length, float mag_str ) :
          Quadrupole( aVerticalQuadrupole, name, spos, length, mag_str ) {}

        VerticalQuadrupole* clone() const { return new VerticalQuadrupole( *this ); }
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * \cosh{\omega} & \frac{1}{\sqrt{k}}\sinh{\omega} & 0 & 0 & 0 & 0\\
         * \sqrt{k}\sinh{\omega} & \cosh{\omega} & 0 & 0 & 0 & 0\\
         * 0 & 0 & \cos{\omega} & \frac{1}{\sqrt{k}}\sin{\omega} & 0 & 0\\
         * 0 & 0 & -\sqrt{k}\sin{\omega} & \cos{\omega} & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$ and \f$ \omega \equiv \omega(k,l) = L \sqrt{|k|} \f$
         */
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };
  }
}

#endif
