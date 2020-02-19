#! /usr/bin/env python
# Copyright (c) 2019 Xilinx Inc. All Rights Reserved.
import me_regression as me
import cardano_regression as cr
import logging, os, sys

opts = {
    'MODE':     'sa_iss'
}

NAME='conv'
PROJECT=NAME+'.prx'

def setup(tc,opts_extra):
    opts.update(opts_extra)
    cr.setup_lab_test(tc,NAME,mode=opts['MODE'])
    return opts

def run(tc, args): 
    #Files
    data_in1=os.path.join(tc.script_dir,'./data/kernelAndInImage_256x16_k3_gaussblur.txt')
    data_in2=os.path.join(tc.script_dir,'./data/refImage_256x16_k3_gaussblur.txt')
    
    #Configure test case
    sim = me.Simulation(tc)
    #sim.add_core(me.Core((0,1),"testMEAccumulateSquare.prx",args="-DDATADIR={0} -DRESULTDIR={1}".format(datadir,tc.results_dir)))
    sim.set_device(os.path.join(os.environ["CARDANO_ROOT"],"data/devices/1x1.json"))
    core0_1 = me.Core((0,1),PROJECT,args="-DDATA_IN1={} -DDATA_IN2={}".format(data_in1,data_in2))
    sim.add_core(core0_1)

	 #Build
    sim.build_all()

    #Add File IO
    if tc.mode == 'sa_iss':
        sim.get_core((0,1)).add_stream("SS0", 32, data_in1)
        sim.get_core((0,1)).add_stream("SS1", 32, data_in2)

    #Simulate
    sim.simulate()

    me.VerifyStepGrep(tc,"PASSED",sim.log,required=True,num=1).run()
    me.VerifyStepGrep(tc,"FAILED",sim.log,required=False).run()


if __name__ == "__main__":
    me.run_single_testcase(run,setup,opts)
