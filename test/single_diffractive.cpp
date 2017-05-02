#include "io/Pythia8Generator.h"

int main()
{
  Hector::Pythia8Generator gen;
  Hector::Particles part = gen.generate();
  
  return 0;
}
