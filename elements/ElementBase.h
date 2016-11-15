#ifndef elements_ElementBase_h
#define elements_ElementBase_h

namespace Element
{
  class ElementBase
  {
    public:
      ElementBase( const char* name="invalid element" );
      virtual ~ElementBase();

    protected:
      const char* name_;
  };
}

#endif
