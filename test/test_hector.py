import pyhector as hector

sv = hector.StateVector()
sv.x = 1.e-3 # in m
for i in range(15):
    sv.xi = 0.+i*0.01
    part = hector.Particle(sv, 0.)
    part.charge = 1
    #part.addPosition(0., sv)
    print part.__iter__()
    #for s,v in part:
    #    print s
    #for s, sv in part:
        #print sv.xi

#beamline = hector.madXparser('CTPPS_Optics2016_public/result/twiss_lhcb1.tfs', 'IP5')
