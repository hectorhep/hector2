#ifndef Hector_Elements_Dipole_h
#define Hector_Elements_Dipole_h

#include "ElementBase.h"

namespace Hector
{
  namespace Element
  {
    /// A generic dipole object
    class Dipole : public ElementBase
    {
      public:
        /// (virtual) class constructor
        Dipole( const Type& type, const std::string& name, float spos, float length, float mag_str ) :
          ElementBase( type, name, spos, length ) {
          setMagneticStrength( mag_str );
        }
    };

    /// Rectangular dipole object builder
    class RectangularDipole : public Dipole
    {
      public:
        /// Class constructor
        RectangularDipole( const std::string& name, float spos, float length, float mag_str ) :
          Dipole( aRectangularDipole, name, spos, length, mag_str ) {}

        std::shared_ptr<ElementBase> clone() const override { return std::make_shared<RectangularDipole>( *this ); }
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * \cos{\theta} & r \sin{\theta} & 0 & 0 & \frac{2r}{E_{\mathrm{b}}} \sin^2{\frac{\theta}{2}} & 0\\
         * \frac{-1}{r} \sin{\theta} & \cos{\theta} & 0 & 0 & \frac{1}{E_{\mathrm{b}}}\sin{\theta} & 0\\
         * 0 & 0 & 1 & L & 0 & 0\\
         * 0 & 0 & 0 & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 &0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$\theta = L/r\f$, \f$ 1/r \equiv k = k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$, and \f$ E_{\mathrm{b}}\f$ = 7000 GeV.
         */
        /// \note Numerical sensitivity (~\f$10^{-8}\f$ relative precision on a 64-bit Intel machine) expected with \f$ \frac{r}{E_{\mathrm{b}}} \left(1-\cos{\theta}\right)\f$.
        ///  Using \f$ \cos{2x} = 1-2\sin^{2}{x} \f$ to transform this term (see the variable called "simp")
        Matrix matrix( float, float, int ) const override;
    };

    /// Sector dipole object builder
    class SectorDipole : public Dipole
    {
      public:
        /// Class constructor
        SectorDipole( const std::string& name, float spos, float length, float mag_str ) :
          Dipole( aSectorDipole, name, spos, length, mag_str ) {}

        std::shared_ptr<ElementBase> clone() const override { return std::make_shared<SectorDipole>( *this ); }

        /// \note Matrix depends if the bending is on or off.
        /** \note \f$
         * \mathbf{M}_{\mathrm{no~bend.}} = \left(
         * \begin{array}{cccccc}
         * \cos{\theta} & r \sin{\theta} & 0 & 0 & 0 & 0\\
         * \frac{-1}{r} \sin{\theta} & \cos{\theta} & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & L & 0 & 0\\
         * 0 & 0 & 0 & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right),
         * \f$\f$
         * \mathbf{M}_{\mathrm{bend.}} = \left(
         * \begin{array}{cccccc}
         * \cos{\theta} & r \sin{\theta} & 0 & 0 & \frac{2r}{E_{\mathrm{b}}} \sin^2{\frac{\theta}{2}} & 0\\
         * \frac{-1}{r} \sin{\theta} & \cos{\theta} & 0 & 0 & \frac{1}{E_{\mathrm{b}}} \sin{\theta} & 0\\
         * 0 & 0 & 1 & L & 0 &0\\
         * 0 & 0 & 0 & 1 & 0 &0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$\theta = L/r\f$, \f$ \frac{1}{r} \equiv k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$
         */
        Matrix matrix( float, float, int ) const override;
    };
  }
}

#endif
