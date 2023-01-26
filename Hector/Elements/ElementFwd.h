#ifndef Hector_Elements_ElementFwd_h
#define Hector_Elements_ElementFwd_h

#include <memory>
#include <vector>

namespace hector {
  /// Collection of beamline elements
  namespace element {
    class Element;
    /// A smart pointer to a beamline element
    typedef std::shared_ptr<element::Element> ElementPtr;
    /// List of elements
    typedef std::vector<ElementPtr> Elements;
  }  // namespace element
}  // namespace hector

#endif
