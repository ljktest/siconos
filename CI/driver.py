#!/usr/bin/env python

import sys
from socket import gethostname

from tasks import siconos_default, known_tasks
from subprocess import check_output

from getopt import gnu_getopt, GetoptError

def usage():
    print """
{0} [--tasks=<task1>,<task2>,...] [--show-tasks] 
    """.format(sys.argv[0])


try:
    opts, args = gnu_getopt(sys.argv[1:], '',
                        ['tasks=', 'show-tasks'])

except GetoptError, err:

    sys.stderr.write(str(err))
    usage()
    exit(2)


tasks = None
for o, a in opts:
    if o in ('--tasks',):
        import tasks
        tasks = [getattr(tasks, s) for s in a.split(',')]

    if o in ('--show-tasks',):
        import tasks
        from machinery.ci_task import CiTask
        for s in dir(tasks):
            if isinstance(getattr(tasks, s), CiTask):
                print s
        exit(0)

hostname = gethostname().split('.')[0]

if tasks is None:

    if hostname in known_tasks:
        tasks = known_tasks[hostname]
    else:
        tasks = [siconos_default]

return_code = 0
for task in tasks:

    return_code += task.run()

for task in tasks:

    task.clean()

# clean everything (-> maybe once a week?)
def mklist(sstr):
    return filter(lambda s: s!='', sstr.strip().split('\n'))

running_containers=mklist(check_output(['docker', 'ps', '-q']))
if len(running_containers)>0:
    check_call(['docker', 'kill'] + running_containers)

containers=mklist(check_output(['docker', 'ps', '-a', '-q']))
if len(containers)>0:
    check_call(['docker', 'rm'] + containers)

images=mklist(check_output(['docker', 'images', '-q']))[1:]
if len(images)>0:
    check_call(['docker', 'rmi'] + images)

exit(return_code)
