#!/usr/bin/env python

print('##################################################################')
print('############## Siconos/Mechanics mechanisms module ###############')
print('##################################################################')


import numpy
import array
import os
import sys

from Siconos.Mechanics.Mechanisms import mbtb
from Siconos.Mechanics.Mechanisms import cadmbtb

# simple hdf5 output
# a dummy SpaceFilter is needed for model encapsulation
# a (pseudo) Contactor class is needed for the object-shape association.
from Siconos.Mechanics.ContactDetection import SpaceFilter
from Siconos.Mechanics import IO
from Siconos.Mechanics.ContactDetection import Avatar, Contactor

install_path= "@CMAKE_INSTALL_PREFIX@"+"/bin"
print("install_path :", install_path)

build_path= "@CMAKE_BINARY_DIR@" +"/src/mechanisms/"
print("build_path : ", build_path)

SiconosMechanisms_BUILD=build_path

from math import pi

# why this truncation is needed ?
my_PI=3.14159265

# would be better in install_path/share ...
# would be better in install_path/share ...
exec(compile(open(install_path+"/mbtbDefaultOptions.py").read(), install_path+"/mbtbDefaultOptions.py", 'exec'))
print("siconos-mechanisms.py: ", install_path+"/mbtbDefaultOptions.py loaded")

try:
    exec(compile(open("mbtbLocalOptions.py").read(), "mbtbLocalOptions.py", 'exec'))
    print("siconos-mechanisms.py: mbtbLocalOptions.py loaded")
except :
    print("siconos-mechanisms.py, info: mbtbLocalOptions.py not defined")

exec(compile(open("bodydef.py").read(), "bodydef.py", 'exec'))
print("run.py: bodydef.py loaded")

if with3D:
    from OCC.BRepPrimAPI import *
    from OCC.gp import *
    from OCC.TopLoc import *
    from OCC.AIS import *
    from OCC.Display.SimpleGui import *
    display, start_display, add_menu, add_function_to_menu = init_display()
    cadmbtb.CADMBTB_setGraphicContext(display.Context)
    v = display.GetView().GetObject()
    cadmbtb.CADMBTB_setGraphicView(v)
    display.View_Iso()


mbtb.MBTB_init(NBBODIES,NBJOINTS,NBCONTACTS)

if NBCONTACTS>0:
    mbtb.MBTB_ContactSetDParam(2,0,0,ContactArtefactLength)
    mbtb.MBTB_ContactSetDParam(3,0,0,ArtefactThershold)
    mbtb.MBTB_ContactSetDParam(4,0,0,NominalForce)

for idContact in range(NBCONTACTS):
    cadmbtb.CADMBTB_setContactDParam(0,idContact,0,contactTrans1[idContact]) # visu
    cadmbtb.CADMBTB_setContactDParam(0,idContact,1,contactTrans2[idContact]) # visu


#build contacts (must be done before the dynamical system building
for idContact in range(NBCONTACTS):
    mbtb.MBTB_ContactLoadCADFile(idContact,afileContact1[idContact],afileContact2[idContact],contactDraw1[idContact],contactDraw2[idContact])
    mbtb.MBTB_ContactBuild(idContact,contactName[idContact],contactBody1[idContact],contactBody2[idContact],contactType3D[idContact],contactmu[idContact],contacten[idContact],et)
    mbtb.MBTB_ContactSetDParam(1,idContact,0,contactOffset[idContact]) # offset
    mbtb.MBTB_ContactSetIParam(0,idContact,0,contactOffsetP1[idContact]) # application point P1 or Trans P1.
    mbtb.MBTB_ContactSetIParam(1,idContact,0,contactNormalFromFace1[idContact]) # normal computed from Face1.

for idBody in range(NBBODIES):
    cadmbtb.CADMBTB_setShapeDParam(0,idBody,bodyTrans[idBody]) # visu

#build dynamical systems
print("User plugin : ", plugin)

for idBody in range(NBBODIES):
    mbtb.MBTB_BodyLoadCADFile(idBody,afile[idBody],bodyDraw[idBody])
    mbtb.MBTB_BodyBuild(idBody, body[idBody], m[idBody],
                        initPos[idBody], initCenterMass[idBody],
                        inertialMatrix[idBody],plugin, fctf[idBody],plugin,fctm[idBody])
    mbtb.MBTB_BodySetVelocity(idBody,initVel[idBody])

#build joint
for idJoint in range(NBJOINTS):
    mbtb.MBTB_JointBuild(idJoint,jointName[idJoint],jointType[idJoint],jointBody1[idJoint],jointBody2[idJoint],jointPos[idJoint])

try:
    for idJoint in range(NBJOINTS):
        mbtb.MBTB_setJointPoints(idJoint,jointPoints[2*idJoint],jointPoints[2*idJoint+1])
except NameError:
    print("run.py, info: joint points not defined.")

for idArtefact in range(NBARTEFACTS):
    cadmbtb.CADMBTB_loadArtefactCADFile(Artefactfile[idArtefact],ArtefactTrans[idArtefact])


mbtb.MBTB_SetDParam(0,TSdeactivateYPosThreshold)
mbtb.MBTB_SetDParam(1,TSdeactivateYVelThreshold)
mbtb.MBTB_SetDParam(2,TSactivateYPosThreshold)
mbtb.MBTB_SetDParam(3,TSactivateYVelThreshold)
mbtb.MBTB_SetDParam(4,TSProjectionMaxIteration)
mbtb.MBTB_SetDParam(5,TSConstraintTol)
mbtb.MBTB_SetDParam(6,TSConstraintTolUnilateral)
mbtb.MBTB_SetDParam(7,TSLevelOfProjection)

mbtb.MBTB_initSimu(stepSize,withProj)
mbtb.MBTB_setGraphicFreq(freqUpdate)
mbtb.MBTB_setOutputFreq(freqOutput)
mbtb.MBTB_setSolverIOption(2,withReduced);
mbtb.MBTB_setSolverIOption(0,solverIt);
mbtb.MBTB_setSolverDOption(0,solverTol);
if gotoPos:
    mbtb.MBTB_moveBodyToPosWithSpeed(0,aPos0,aVel0)
    mbtb.MBTB_moveBodyToPosWithSpeed(1,aPos1,aVel1)
    mbtb.MBTB_moveBodyToPosWithSpeed(2,aPos2,aVel2)

if with3D:
    display.View_Iso()
    display.FitAll()


                         # ord('W'): self._display.SetModeWireFrame,
                         # ord('S'): set_shade_mode,
                         # ord('A'): self._display.EnableAntiAliasing,
                         # ord('B'): self._display.DisableAntiAliasing,
                         # ord('Q'): self._display.SetModeQuickHLR,
                         # ord('E'): self._display.SetModeExactHLR,
                         # ord('F'): self._display.FitAll(
ais_boxshp=None

# Hdf5 IO setup
with IO.Hdf5(broadphase=SpaceFilter(mbtb.MBTB_model())) as io:

    def _run(nsteps):
        fp = mbtb._MBTB_open('simu.txt', 'a')
        currentTimerCmp = mbtb.cvar.sTimerCmp
        for i in range(nsteps):
            mbtb.cvar.sTimerCmp += 1
            if mbtb.cvar.sTimerCmp % mbtb.cvar.sFreqOutput == 0:
                print('STEP Number =',mbtb.cvar.sTimerCmp, '<', nsteps+currentTimerCmp);
            mbtb._MBTB_STEP()
            mbtb._MBTB_displayStep()
            io.outputDynamicObjects()
            io.outputVelocities()
            io.outputContactForces()
            io.outputSolverInfos()
            if mbtb.cvar.sTimerCmp % mbtb.cvar.sFreqOutput == 0:
                mbtb._MBTB_printStep(fp);
        mbtb._MBTB_close(fp)
        mbtb.ACE_PRINT_TIME()


    def STEP(event=None):
        mbtb.MBTB_step()
        io.outputDynamicObjects()
        io.outputVelocities()
        io.outputContactForces()
        io.outputSolverInfos()


    def STEP_1000(event=None):
        ii=0
        while ii < 1000:
            mbtb.MBTB_step()
            io.outputDynamicObjects()
            io.outputVelocities()
            io.outputContactForces()
            io.outputSolverInfos()
            cadmbtb.CADMBTB_DumpGraphic()
            ii=ii+1


    def RUN10(event=None):
        _run(10)

    def RUN(event=None):
        _run(stepNumber)

    def RUN100(event=None):
        _run(100)

    def RUN400(event=None):
        _run(400)


    def RUN1000(event=None):
        _run(1000)

    def RUN7000(event=None):
        _run(7000)


    def RUN10000(event=None):
        _run(10000)

    def RUN50000(event=None):
        _run(50000)

    def RUN300000(event=None):
        _run(300000)

    def QUIT(event=None):
        exit();


    def GRAPHIC_ON(event=None):
        cadmbtb.CADMBTB_setGraphicContext(display.Context)
        v = display.GetView().GetObject()
        cadmbtb.CADMBTB_setGraphicView(v)


    def GRAPHIC_OFF(event=None):
        cadmbtb.CADMBTB_disableGraphic()

    def DISABLE_PROJ(event=None):
        mbtb.MBTB_doProj(0)

    def ENABLE_PROJ(event=None):
        mbtb.MBTB_doProj(1)

    def DISABLE_ONLYPROJ(event=None):
        mbtb.MBTB_doOnlyProj(0)

    def ENABLE_ONLYPROJ(event=None):
        mbtb.MBTB_doOnlyProj(1)

    def GRAPHIC_DUMP(event=None):
        cadmbtb.CADMBTB_DumpGraphic()

    def ENABLE_VERBOSE_MBTB_PRINT_DIST(event=None):
        mbtb.MBTB_print_dist(1)
        cadmbtb.CADMBTB_print_dist(1)

    def DISABLE_VERBOSE_MBTB_PRINT_DIST(event=None):
        mbtb.MBTB_print_dist(0)
        cadmbtb.CADMBTB_print_dist(0)

    def ENABLE_VERBOSE_MBTB_DISPLAY_STEP_BODIES(event=None):
        mbtb.MBTB_displayStep_bodies(1)

    def DISABLE_VERBOSE_MBTB_DISPLAY_STEP_BODIES(event=None):
        mbtb.MBTB_displayStep_bodies(0)

    def ENABLE_VERBOSE_MBTB_DISPLAY_STEP_JOINTS(event=None):
        mbtb.MBTB_displayStep_joints(1)

    def DISABLE_VERBOSE_MBTB_DISPLAY_STEP_JOINTS(event=None):
        mbtb.MBTB_displayStep_joints(0)

    def ENABLE_VERBOSE_MBTB_DISPLAY_STEP_CONTACTS(event=None):
        mbtb.MBTB_displayStep_contacts(1)

    def DISABLE_VERBOSE_MBTB_DISPLAY_STEP_CONTACTS(event=None):
        mbtb.MBTB_displayStep_contacts(0)

    def VIEW_TOP(event=None):
        display.View_Top()
    def VIEW_BOTTOM(event=None):
        display.View_Bottom()
    def VIEW_LEFT(event=None):
        display.View_Left()
    def VIEW_RIGHT(event=None):
        display.View_Right()
    def VIEW_REAR(event=None):
        display.View_Rear()
    def VIEW_FRONT(event=None):
        display.View_Front()
    def VIEW_ISO(event=None):
        display.View_Iso()
    def VIEW_FITALL(event=None):
        display.FitAll()

    def VIEW_TOP(event=None):
        display.View_Top()

    for idBody in range(NBBODIES):
        shape_name = os.path.basename(os.path.splitext(afile[idBody])[0])

        io.addShapeDataFromFile(shape_name, afile[idBody])
        io.addObject('obj-{0}'.format(shape_name), [Avatar(shape_name)],
                     mass=1,
                     position=[0, 0, 0])

    if with3D and __name__ == '__main__':
        cadmbtb.CADMBTB_setIParam(0,dumpGraphic)
        mbtb.MBTB_ContactSetIParam(2,0,0,drawMode)
        add_menu('MBTB')
        add_menu('OPTION')
        add_menu('VERBOSE')
        add_menu('DISPLAY')
        add_menu('VIEW')
        add_function_to_menu('MBTB', STEP)
        add_function_to_menu('MBTB', RUN)
        add_function_to_menu('MBTB', RUN10)
        add_function_to_menu('MBTB', RUN100)
        add_function_to_menu('MBTB', RUN400)
        add_function_to_menu('MBTB', RUN1000)
        add_function_to_menu('MBTB', RUN7000)
        add_function_to_menu('MBTB', RUN10000)
        add_function_to_menu('MBTB', RUN50000)
        add_function_to_menu('MBTB', RUN300000)
        add_function_to_menu('MBTB', QUIT)
        add_function_to_menu('DISPLAY',GRAPHIC_OFF)
        add_function_to_menu('DISPLAY',GRAPHIC_ON)
        add_function_to_menu('DISPLAY',GRAPHIC_DUMP)
        add_function_to_menu('VERBOSE',ENABLE_VERBOSE_MBTB_PRINT_DIST)
        add_function_to_menu('VERBOSE',DISABLE_VERBOSE_MBTB_PRINT_DIST)
        add_function_to_menu('VERBOSE',ENABLE_VERBOSE_MBTB_DISPLAY_STEP_BODIES)
        add_function_to_menu('VERBOSE',DISABLE_VERBOSE_MBTB_DISPLAY_STEP_BODIES)
        add_function_to_menu('VERBOSE',ENABLE_VERBOSE_MBTB_DISPLAY_STEP_JOINTS)
        add_function_to_menu('VERBOSE',DISABLE_VERBOSE_MBTB_DISPLAY_STEP_JOINTS)
        add_function_to_menu('VERBOSE',ENABLE_VERBOSE_MBTB_DISPLAY_STEP_CONTACTS)
        add_function_to_menu('VERBOSE',DISABLE_VERBOSE_MBTB_DISPLAY_STEP_CONTACTS)
        add_function_to_menu('OPTION',DISABLE_PROJ)
        add_function_to_menu('OPTION',ENABLE_PROJ)
        add_function_to_menu('OPTION',DISABLE_ONLYPROJ)
        add_function_to_menu('OPTION',ENABLE_ONLYPROJ)
        add_function_to_menu('VIEW',VIEW_TOP)
        add_function_to_menu('VIEW',VIEW_BOTTOM)
        add_function_to_menu('VIEW',VIEW_RIGHT)
        add_function_to_menu('VIEW',VIEW_LEFT)
        add_function_to_menu('VIEW',VIEW_FRONT)
        add_function_to_menu('VIEW',VIEW_REAR)
        add_function_to_menu('VIEW',VIEW_ISO)
        add_function_to_menu('VIEW',VIEW_FITALL)
        start_display()
    else:
        _run(stepNumber)
