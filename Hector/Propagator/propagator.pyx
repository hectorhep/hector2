cimport hector

cdef class stateVector:
    cdef hector.StateVector* thisptr # c++ instance to be wrapped

    def __cinit__(self):
        print 'haha'
        self.thisptr = new hector.StateVector()

    def __dealloc__(self):
        del self.thisptr

    cpdef energy(self):
        return self.thisptr.energy()
    cpdef xi(self):
        return self.thisptr.xi()
    cpdef kick(self):
        return self.thisptr.kick()
    cpdef x(self):
        return self.thisptr.position().x()
