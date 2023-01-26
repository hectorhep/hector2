#ifndef Hector_Elements_ElementBaseFwd_h
#define Hector_Elements_ElementBaseFwd_h

#include <memory>
#include <vector>

namespace hector {
  /// Collection of beamline elements
  namespace element {
    class ElementBase;
    /// A smart pointer to a beamline element
    typedef std::shared_ptr<element::ElementBase> ElementPtr;
    /// List of elements
    typedef std::vector<ElementPtr> Elements;
  }  // namespace element
}  // namespace hector

#endif
