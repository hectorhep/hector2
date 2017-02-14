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

        RectangularDipole* clone() const { return new RectangularDipole( *this ); }
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * \cos{\frac{l}{r}} & \frac{-1}{r} \sin{\frac{l}{r}} & 0 & 0 & 0 & 0\\
         * r \sin{\frac{l}{r}} & \cos{\frac{l}{r}} & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & 0 & 0 & 0\\
         * 0 & 0 & l & 1 & 0 & 0\\
         * \frac{2r}{E_{\mathrm{b}}} \sin^2{\frac{l}{2r}} & \frac{1}{E_{\mathrm{b}}}\sin{\frac{l}{r}} & 0 & 0 & 1 &0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ 1/r \equiv k = k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$ and \f$ E_{\mathrm{b}}=7000\f$ GeV.
         */
        /// \warning Numerical sensitivity expected with \f$ \frac{r}{E_{\mathrm{b}}} \left(1-\cos{\frac{l}{r}}\right)\f$.
        ///  Using \f$ \cos{2x} = 1-2\sin^{2}{x} \f$ to transform this term (see the variable called "simp")
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };

    /// Sector dipole object builder
    class SectorDipole : public Dipole
    {
      public:
        /// Class constructor
        SectorDipole( const std::string& name ) : Dipole( ElementBase::SectorDipole, name ) {}

        SectorDipole* clone() const { return new SectorDipole( *this ); }

        /// \note Matrix depends if the bending is on or off.
        /** \note \f$
         * \mathbf{M}_{\mathrm{bending-off}} = \left(
         * \begin{array}{cccccc}
         * \cos{\frac{l}{r}} & \frac{-1}{r} \sin{\frac{l}{r}} & 0 & 0 & 0 & 0\\
         * r \sin{\frac{l}{r}} & \cos{\frac{l}{r}} & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & 0 & 0 & 0\\
         * 0 & 0 & l & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right),
         * \f$\f$
         * \mathbf{M}_{\mathrm{bending-on}} = \left(
         * \begin{array}{cccccc}
         * \cos{\frac{l}{r}} & \frac{-1}{r} \sin{\frac{l}{r}} & 0 & 0 & 0 & 0\\
         * r \sin{\frac{l}{r}} & \cos{\frac{l}{r}} & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & 0 & 0 &0\\
         * 0 & 0 & l & 1 & 0 &0\\
         * \frac{2r}{E_{\mathrm{b}}} \sin^2{\frac{l}{2r}} & \frac{1}{E_{\mathrm{b}}} \sin{\frac{l}{r}} & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ \frac{1}{r} \equiv k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$
         */
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };
  }
}

#endif
