# simple bullet input and output

import VtkShapes
import shlex
import numpy as np
import h5py

from Siconos.Mechanics.ContactDetection import Contactor

from Siconos.Mechanics.ContactDetection.Bullet import \
    BulletDS, BulletWeightedShape, \
    btCollisionObject, btQuaternion, btTransform, btVector3

from Siconos.Mechanics.ContactDetection.Bullet import \
    cast_BulletR

from Siconos.Kernel import cast_NewtonImpactFrictionNSL

from Siconos.IO import MechanicsIO

import Siconos.Numerics as Numerics


def object_id(obj):
    """returns an unique object identifier"""
    return obj.__hash__()


def apply_gravity(body):
    g = 9.81
    weight = [0, 0, - body.massValue() * g]
    body.setFExtPtr(weight)



def group(h, name):
    try:
        return h[name]
    except KeyError:
        return h.create_group(name)


def data(h, name, nbcolumns):
    try:
        return h[name]
    except KeyError:
        return h.create_dataset(name, (0, nbcolumns),
                                maxshape=(None, nbcolumns))

def add_line(dataset, line):
    dataset.resize(dataset.shape[0] + 1, 0)
    dataset[dataset.shape[0]-1, :] = line

def str_of_file(filename):
    with open(filename, 'r') as f:
        return f.read()

class Dat():
    """a Dat context manager reads at instantiation the positions and
       orientations of collision objects from :

       - a ref file (default ref.txt) with shape primitives or shape
         url

       - an input .dat file (default is input.dat)

       input format is :
       shaped_id object_group mass px py pz ow ox oy oz vx vy vx vo1 vo2 vo3

       with
         shape_id : line number in ref file (an integer)
         object group : an integer ; negative means a static object
         mass : mass of the object (a float)
         px py pz : position (float)
         ow ox oy oz : orientation (as an unit quaternion)
         vx vy vx vo1 vo2 vo3 : velocity

       It provides functions to output position and orientation during
       simulation (output is done by default in pos.dat)

       output format is : time object_id px py pz ow ox oy oz

       with:
         time : float
         object_id : the object id (int)
         px, py, pz : components of the position (float)
         ow, ox, oy oz : components of an unit quaternion (float)
    """

    def __init__(self, broadphase, osi, shape_filename='ref.txt',
                 input_filename='input.dat',
                 set_external_forces=apply_gravity):
        self._broadphase = broadphase
        self._osi = osi
        self._input_filename = input_filename
        self._static_origins = []
        self._static_orientations = []
        self._static_transforms = []
        self._static_cobjs = []
        self._shape = VtkShapes.Collection(shape_filename)
        self._static_pos_file = None
        self._dynamic_pos_file = None
        self._contact_forces_file = None
        self._solver_traces_file = None
        self._io = MechanicsIO()

        # read data
        with open(self._input_filename, 'r') as input_file:

            with open('bindings.dat', 'w') as bind_file:

                ids = -1
                idd = 1
                for line in input_file:
                    sline = shlex.split(line)
                    if len(sline) > 3:
                        shape_id = int(sline[0])
                        group_id = int(sline[1])
                        mass = float(sline[2])
                        q0, q1, q2, w, x, y, z, v0, v1, v2, v3, v4, v5 =\
                          [ float(i) for i in sline[3:]]

                        if group_id < 0:
                            # a static object
                            static_cobj = btCollisionObject()
                            static_cobj.setCollisionFlags(
                                btCollisionObject.CF_STATIC_OBJECT)
                            origin = btVector3(q0, q1, q2)
                            self._static_origins.append(origin)
                            orientation = btQuaternion(x, y, z, w)
                            self._static_orientations.append(orientation)
                            transform = btTransform(orientation)
                            transform.setOrigin(origin)
                            self._static_transforms.append(transform)
                            static_cobj.setWorldTransform(transform)
                            static_cobj.setCollisionShape(
                                self._shape.at_index(shape_id))
                            self._static_cobjs.append(static_cobj)
                            broadphase.addStaticObject(static_cobj, abs(group_id)-1)
                            bind_file.write('{0} {1}\n'.format(ids, shape_id))
                            ids -= 1

                        else:
                            # a moving object
                            body = BulletDS(BulletWeightedShape(
                                self._shape.at_index(shape_id), mass),
                            [q0, q1, q2, w, x, y, z],
                            [v0, v1, v2, v3, v4, v5])

                              # set external forces
                            set_external_forces(body)

                            # add the dynamical system to the non smooth
                            # dynamical system
                            self._broadphase.model().nonSmoothDynamicalSystem().\
                            insertDynamicalSystem(body)
                            self._osi.insertDynamicalSystem(body)
                            bind_file.write('{0} {1}\n'.format(idd, shape_id))
                            idd += 1

    def __enter__(self):
        self._static_pos_file = open('spos.dat', 'w')
        self._dynamic_pos_file = open('dpos.dat', 'w')
        self._contact_forces_file = open('cf.dat', 'w')
        self._solver_traces_file = open('solv.dat', 'w')
        return self

    def __exit__(self, type_, value, traceback):
        self._contact_forces_file.close()
        self._static_pos_file.close()
        self._dynamic_pos_file.close()
        self._solver_traces_file.close()


    def outputStaticObjects(self):
        """
        Outputs positions and orientations of static objects
        """
        time = self._broadphase.model().simulation().nextTime()
        idd = -1
        for transform in self._static_transforms:
            position = transform.getOrigin()
            rotation = transform.getRotation()
            self._static_pos_file.write(
                '{0} {1} {2} {3} {4} {5} {6} {7} {8}\n'.
                format(time,
                       idd,
                       position.x(),
                       position.y(),
                       position.z(),
                       rotation.w(),
                       rotation.x(),
                       rotation.y(),
                       rotation.z()))
            idd -= 1

    def outputDynamicObjects(self):
        """
        Outputs positions and orientations of dynamic objects
        """
        time = self._broadphase.model().simulation().nextTime()

        positions = self._io.positions(self._broadphase.model())

        times = np.empty((positions.shape[0], 1))
        times.fill(time)

        tidd = np.arange(1,
                         positions.shape[0] + 1).reshape(positions.shape[0], 1)

        np.savetxt(self._dynamic_pos_file,
                   np.concatenate((times, tidd, positions),
                                  axis=1))

    def outputContactForces(self):
        """
        Outputs contact forces
        """
        if self._broadphase.model().nonSmoothDynamicalSystem().\
                topology().indexSetsSize() > 1:
            time = self._broadphase.model().simulation().nextTime()
            for inter in self._broadphase.model().nonSmoothDynamicalSystem().\
                topology().indexSet(1).interactions():
                bullet_relation = cast_BulletR(inter.relation())
                if bullet_relation is not None:
                    nslaw = inter.nslaw()
                    mu = cast_NewtonImpactFrictionNSL(nslaw).mu()
                    nc = bullet_relation.nc()
                    lambda_ = inter.lambda_(1)
                    if(True):
                        jachqt = bullet_relation.jachqT()
                        cf = np.dot(jachqt.transpose(), lambda_)
                        cp = bullet_relation.contactPoint()
                        posa = cp.getPositionWorldOnA()
                        posb = cp.getPositionWorldOnB()
                        self._contact_forces_file.write(
                        '{0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10} {11} {12} {13}\n'.
                        format(time,
                               mu,
                               posa.x(),
                               posa.y(),
                               posa.z(),
                               posb.x(),
                               posb.y(),
                               posb.z(),
                               nc[0], nc[1], nc[2],
                               cf[0], cf[1], cf[2]))

    def outputSolverInfos(self):
        """
        Outputs solver #iterations & precision reached
        """

        time = self._broadphase.model().simulation().nextTime()
        so = self._broadphase.model().simulation().oneStepNSProblem(0).\
            numericsSolverOptions()

        if so.solverId == Numerics.SICONOS_GENERIC_MECHANICAL_NSGS:
            iterations = so.iparam[3]
            precision = so.dparam[2]
            local_precision = so.dparam[3]
        elif so.solverId == Numerics.SICONOS_FRICTION_3D_NSGS:
            iterations = so.iparam[7]
            precision = so.dparam[1]
            local_precision = 0.
        # maybe wrong for others
        else:
            iterations = so.iparam[1]
            precision = so.dparam[1]
            local_precision = so.dparam[2]

        self._solver_traces_file.write('{0} {1} {2} {3}\n'.
                                       format(time, iterations, precision,
                                              local_precision))


class Hdf5():
    """a Hdf5 context manager reads at instantiation the positions and
       orientations of collision objects from :

       - a ref file (default ref.txt) with shape primitives or shape
         url

       - an input .dat file (default is input.dat)

       input format is :
       shaped_id object_group mass px py pz ow ox oy oz vx vy vx vo1 vo2 vo3

       with
         shape_id : line number in ref file (an integer)
         object group : an integer ; negative means a static object
         mass : mass of the object (a float)
         px py pz : position (float)
         ow ox oy oz : orientation (as an unit quaternion)
         vx vy vx vo1 vo2 vo3 : velocity

       It provides functions to output position and orientation during
       simulation (output is done by default in pos.dat)

       output format is : time object_id px py pz ow ox oy oz

       with:
         time : float
         object_id : the object id (int)
         px, py, pz : components of the position (float)
         ow, ox, oy oz : components of an unit quaternion (float)
    """

    def __init__(self, io_filename = 'io.hdf5', mode = 'a',
                 broadphase=None, osi=None, shape_filename=None,
                 input_filename='input.dat',
                 set_external_forces=apply_gravity):
        self._io_filename = io_filename
        self._mode = mode
        self._broadphase = broadphase
        self._osi = osi
        self._input_filename = input_filename
        self._static_origins = []
        self._static_orientations = []
        self._static_transforms = []
        self._static_cobjs = []
        self._shape = None
        self._shapeid = dict()
        self._static_data = None
        self._dynamic_data = None
        self._cf_data = None
        self._solv_data = None
        self._input = None
        self._out = None
        self._data = None
        self._io = MechanicsIO()
        self._set_external_forces = set_external_forces
        self._shape_filename = shape_filename
        self._number_of_shapes = 0
        self._number_of_dynamic_objects = 0
        self._number_of_static_objects = 0

    def __enter__(self):
        self._out = h5py.File(self._io_filename, self._mode)
        self._data = group(self._out, 'data')
        self._ref = group(self._data, 'ref')
        self._static_data = data(self._data, 'static', 9)
        self._dynamic_data = data(self._data, 'dynamic', 9)
        self._cf_data = data(self._data, 'cf', 15)
        self._solv_data = data(self._data, 'solv', 4)
        self._input = group(self._data, 'input')

        if self._shape_filename is None:
            self._shape = VtkShapes.Collection(self._out)
        else:
            self._shape = VtkShapes.Collection(self._shape_filename)

        self.importScene()
        return self

    def __exit__(self, type_, value, traceback):
        self._out.close()

    def importObject(self, position, orientation, velocity, contactors, mass):

        if self._broadphase is not None and 'input' in self._data:
            if mass == 0.:
                # a static object
                for c in contactors:

                    static_cobj = btCollisionObject()
                    static_cobj.setCollisionFlags(
                        btCollisionObject.CF_STATIC_OBJECT)
                    c_origin = btVector3(c.position[0],
                                         c.position[1],
                                         c.position[2])
                    self._static_origins.append(c_origin)
                    (w, x, y, z) = c.orientation
                    c_orientation = btQuaternion(x, y, z, w)
                    self._static_orientations.append(c_orientation)
                    transform = btTransform(c_orientation)
                    transform.setOrigin(c_origin)
                    self._static_transforms.append(transform)
                    static_cobj.setWorldTransform(transform)
                    shape_id = self._shapeid[c.name]
                    static_cobj.setCollisionShape(
                        self._shape.at_index(shape_id))
                    self._static_cobjs.append(static_cobj)
                    self._broadphase.addStaticObject(static_cobj,
                                                     int(c.group))

            else:
                # a moving object
                shape_id = self._shapeid[contactors[0].name]
                body = BulletDS(BulletWeightedShape(
                    self._shape.at_index(shape_id), mass),
                    position + orientation,
                    velocity)

                for contactor in contactors[1:]:
                    shape_id = self._shapeid[contactors[0].name]

                    body.addCollisionObject(self._shape.at_index(shape_id),
                                            contactor.position,
                                            contactor.orientation,
                                            contactor.collision_group)

                # set external forces
                self._set_external_forces(body)

                # add the dynamical system to the non smooth
                # dynamical system
                self._broadphase.model().nonSmoothDynamicalSystem().\
                     insertDynamicalSystem(body)
                self._osi.insertDynamicalSystem(body)

    def importScene(self):
        """
        Import into the broadphase object all the static and dynamic objects
        from hdf5 file
        """

        for shape_name in self._ref:
            self._shapeid[shape_name] = self._ref[shape_name].attrs['id']
            self._number_of_shapes += 1

        self._shape = VtkShapes.Collection(self._out)

        def floatv(v):
            return [float(x) for x in v]

        # read data
        if self._broadphase is not None and 'input' in self._data:
            for (name, obj) in self._input.items():
                mass = obj.attrs['mass']
                position = obj.attrs['position']
                orientation = obj.attrs['orientation']
                velocity = obj.attrs['velocity']
                contactors = [Contactor(ctr_name,
                                        ctr.attrs['group'],
                                        floatv(ctr.attrs['position']),
                                        floatv(ctr.attrs['orientation']))
                              for ctr_name, ctr in obj.items()]

                self.importObject(floatv(position), floatv(orientation),
                                  floatv(velocity), contactors, float(mass))

    def outputStaticObjects(self):
        """
        Outputs positions and orientations of static objects
        """
        time = self._broadphase.model().simulation().nextTime()
        idd = -1
        p = 0
        self._static_data.resize(len(self._static_transforms), 0)

        for transform in self._static_transforms:
            position = transform.getOrigin()
            rotation = transform.getRotation()
            self._static_data[p, :] = \
                [time,
                 idd,
                 position.x(),
                 position.y(),
                 position.z(),
                 rotation.w(),
                 rotation.x(),
                 rotation.y(),
                 rotation.z()]
            idd -= 1
            p += 1

    def outputDynamicObjects(self):
        """
        Outputs positions and orientations of dynamic objects
        """

        current_line = self._dynamic_data.shape[0]

        time = self._broadphase.model().simulation().nextTime()

        positions = self._io.positions(self._broadphase.model())

        self._dynamic_data.resize(current_line + positions.shape[0], 0)

        times = np.empty((positions.shape[0], 1))
        times.fill(time)

        tidd = np.arange(1,
                         positions.shape[0] + 1).reshape(
                             positions.shape[0],
                             1)

        self._dynamic_data[current_line:, :] = np.concatenate((times, tidd,
                                                               positions),
                                                               axis=1)

    def outputContactForces(self):
        """
        Outputs contact forces
        """
        if self._broadphase.model().nonSmoothDynamicalSystem().\
                topology().indexSetsSize() > 1:
            time = self._broadphase.model().simulation().nextTime()
            contact_points = self._io.contactPoints(self._broadphase.model())

            assert(contact_points is not None)

            current_line = self._cf_data.shape[0]
            self._cf_data.resize(current_line + contact_points.shape[0], 0)
            times = np.empty((contact_points.shape[0], 1))
            times.fill(time)

            self._cf_data[current_line:, :] = np.concatenate((times,
                                                              contact_points),
                                                             axis=1)

    def outputSolverInfos(self):
        """
        Outputs solver #iterations & precision reached
        """

        time = self._broadphase.model().simulation().nextTime()
        so = self._broadphase.model().simulation().oneStepNSProblem(0).\
            numericsSolverOptions()

        current_line = self._solv_data.shape[0]
        self._solv_data.resize(current_line + 1, 0)
        if so.solverId == Numerics.SICONOS_GENERIC_MECHANICAL_NSGS:
            iterations = so.iparam[3]
            precision = so.dparam[2]
            local_precision = so.dparam[3]
        elif so.solverId == Numerics.SICONOS_FRICTION_3D_NSGS:
            iterations = so.iparam[7]
            precision = so.dparam[1]
            local_precision = 0.
        # maybe wrong for others
        else:
            iterations = so.iparam[1]
            precision = so.dparam[1]
            local_precision = so.dparam[2]

        self._solv_data[current_line, :] = [time, iterations, precision,
                                            local_precision]

    def insertShapeFromFile(self, name, filename):
        shape = self._ref.require_dataset(name, (1,), dtype=h5py.new_vlen(str),
                                          exact=True)
        shape[:] = str_of_file(filename)
        shape.attrs['id'] = self._number_of_shapes
        self._shapeid[name] = shape.attrs['id']
        self._shape = VtkShapes.Collection(self._out)
        self._number_of_shapes += 1

    def createContactor(self, shape_name, group,
                        position=[0, 0, 0],
                        orientation=[1, 0, 0, 0]):
        return Contactor(shape_name, group, position, orientation)

    def insertObject(self, name, contactors,
                     position,
                     orientation=[1, 0, 0, 0],
                     velocity=[0, 0, 0, 0, 0, 0],
                     mass=0):

        obj = group(self._input, name)
        obj.attrs['mass'] = mass
        obj.attrs['position'] = position
        obj.attrs['orientation'] = orientation
        obj.attrs['velocity'] = velocity
        for contactor in contactors:
            dat = data(obj, contactor.name, 0)
            dat.attrs['group'] = contactor.group
            dat.attrs['position'] = contactor.position
            dat.attrs['orientation'] = contactor.orientation

        self.importObject(position, orientation, velocity, contactors, mass)

        if mass == 0:
            obj.attrs['id'] = - (self._number_of_static_objects + 1)
            self._number_of_static_objects += 1

        else:
            obj.attrs['id'] = (self._number_of_dynamic_objects + 1)
            self._number_of_dynamic_objects += 1
