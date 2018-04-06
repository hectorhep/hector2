#!/usr/bin/python

import pyhector
from sys import argv
from ROOT import TFile, TH1D, TH2D

def main(argv):
    num_particles = 100
    if len(argv) > 1:
        num_particles = int(argv[1])

    parser = pyhector.Twissparser(argv[0], 'IP5', 250.)
    print 'Twiss file generated on:', parser.header['production_date']
    #parser.beamline.offsetElementsAfter(120., pyhector.TwoVector(-0.097, 0.))
    prop = pyhector.Propagator(parser.beamline)
    #print parser.beamline.elements
    rps = parser.beamline.find('XRPH\.')
    #print [rp.name for rp in rps]
    max_s = max([rp.s+rp.length for rp in rps])*1.01
    hm_pot = {}
    stopping_elements = {}
    for rp in rps:
        hm_pot[rp] = TH2D('hm_pot_'+rp.name, rp.name+';x (mm);y (mm)', 200, -25., 25., 200, -25., 25. )

    gun = pyhector.GaussianParticleGun()
    gun.smearX(0., 10.e-6) # vertex size
    gun.smearY(0., 10.e-6)
    gun.smearTx(191.546e-6, 20.e-6) # crossing angle / beam divergence
    gun.smearTy(0., 20.e-6)

    xi_values = [0.05]

    hm_vertex = TH2D('hm_vertex', ';x (mm);y (mm)', 200, -1.e-1, 1.e-1, 200, -1.e-1, 1.e-1)
    h_tx = TH1D('h_tx', ';#theta_{x} (mrad);Events', 250, -1., 1. )
    for i in range(num_particles):
        if i%(num_particles/10) == 0:
            print 'particle', i, '/', num_particles, 'generated'
        p = gun.shoot()
        initial_state = p.positions[0]
        hm_vertex.Fill(initial_state.x*1.e3, initial_state.y*1.e3)
        h_tx.Fill(initial_state.Tx*1.e3)
        for xi in xi_values:
            p.xi = xi
            try:
                prop.propagate(p, max_s)
            except pyhector.ParticleStoppedException, e:
                elem = e.stoppingElement.name+' '+str(e.stoppingElement.type)
                #print e.stoppingElement.s
                if elem not in stopping_elements.keys(): stopping_elements[elem] = 1
                else: stopping_elements[elem] += 1
                continue
            for rp in rps:
                sv = p.stateVectorAt(rp.s)
                hm_pot[rp].Fill(sv.x*1.e3, sv.y*1.e3)

    for elem, num_stop in sorted(stopping_elements.iteritems(), key=lambda(k,v): (v,k), reverse=True):
        print '%.2f%% of particles stopped in %s' % (100.*num_stop/num_particles, elem)

    f = TFile('output.root', 'recreate')
    hm_vertex.Write()
    h_tx.Write()
    for rp in rps:
        hm_pot[rp].Write()
    f.Write()

if __name__ == '__main__':
    if len(argv) < 2:
        print 'Usage:', argv[0], 'Twiss-file'
        exit(0)
    main(argv[1:])
