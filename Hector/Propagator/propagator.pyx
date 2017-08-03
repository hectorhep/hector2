STUFF = "Hi"

cimport hector
from hector cimport StateVector as CStateVector
from hector cimport Hep2Vector as CHep2Vector
from hector cimport HepLorentzVector as CHepLorentzVector

#----- state vector

cdef class stateVector:
    cdef hector.StateVector* thisptr # c++ instance to be wrapped
    def __cinit__(self):
        self.thisptr = new hector.StateVector()
    def __dealloc__(self):
        del self.thisptr

    cpdef double energy(self):
        return self.thisptr.energy()
    cpdef double xi(self):
        return self.thisptr.xi()
    cpdef double kick(self):
        return self.thisptr.kick()
    #cdef Hep2Vector position(self):
    #    return self.thisptr.position()
    cpdef double x(self):
        return self.thisptr.position().x()
    cpdef double y(self):
        return self.thisptr.position().y()
    cpdef double tx(self):
        return self.thisptr.angles().x()
    cpdef double ty(self):
        return self.thisptr.angles().y()

#----- particle object

cdef class particle:
    cdef hector.Particle* thisptr # c++ instance to be wrapped
    def __cinit__(self):
        self.thisptr = new hector.Particle()
    def __dealloc(self):
        del self.thisptr

    cpdef double mass(self):
        return self.thisptr.mass()
    cpdef int pdgId(self):
        return self.thisptr.pdgId()
    cpdef int charge(self):
        return self.thisptr.charge()
    #cpdef CHepLorentzVector momentumAt(self, float s):
    #    return self.thisptr.momentumAt(s)

