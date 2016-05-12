#!/usr/bin/env python3

import os, os.path, sys
from glob import glob
import lxml.etree

from builder_common import *

def get_classes_conditional(doxy_xml_files, cond):
    """Get classes and members from a list of Doxygen XML files that
       meet the given condition."""
    found = {}
    for xmlfile in doxy_xml_files:
        xml = lxml.etree.parse(xmlfile)
        classes = xml.xpath('.//compounddef[@kind="class" or @kind="struct"]')
        for cl in classes:
            if cond(cl):
                classname = cl.find('./compoundname')
                baseclasses = cl.xpath('./basecompoundref')
                membervars = cl.xpath('.//memberdef[@kind="variable"]/name')
                location = cl.find('./location')
                found[classname.text] = (
                    {'name': classname.text,
                     'bases': [base.text for base in baseclasses],
                     'members': [mem.text for mem in membervars],
                     'filepath': location.attrib['file'],
                     'line': int(location.attrib['line']),
                     'abstract': cl.xpath('@abstract="yes"'),
                    })
    return found

def classes_from_build_path(build_path):
    """Get classes and members from all Doxygen XML files found on the
       provided build path."""
    doxy_xml_path = os.path.join(build_path,'Docs/build/html/doxygen/xml')
    doxy_xml_files = (
        glob(os.path.join(doxy_xml_path, 'class*.xml'))
        + glob(os.path.join(doxy_xml_path, 'struct*.xml')))

    # We want only classes that contain calls to the
    # ACCEPT_SERIALIZATION macro.
    serializable = './/memberdef/name[text()="ACCEPT_SERIALIZATION"]'

    def pred(x):
        return len(x.xpath(serializable)) > 0

    return get_classes_conditional(doxy_xml_files, pred)

def assign_priorities(classes, source_dir):
    """For each class, get its priority to help in ordering the
       declarations in the generated file."""
    for cl in classes.values():
        cl['priority'] = get_priority(cl['name'], source_dir,
                                      cl['filepath'], cl['line'])

def resolve_base_classes(classes):
    """For each class, find those base classes which are also in the list.
       This is to exclude STL and BOOST base classes,
       e.g. enable_shared_from_this<>.  In practice we only want to
       list base classes which are serializable."""
    for cl in classes.values():
        resolved = []
        for base in cl['bases']:
            if base in classes:
                resolved.append(base)
        cl['resolved_bases'] = resolved

def classes_from_headers(all_headers, include_paths):
    """Use compiler preprocessor to find an approximate list of classes
       referenced by a set of headers.  May return some words which
       are not classes."""
    import os, os.path, tempfile, shutil
    classes = []
    try:
        d = tempfile.mkdtemp()
        hpp = os.path.join(d, 'headers.hpp')
        compiled = os.path.join(d, 'out.cpp')
        with open(hpp,'w') as h:
            [print('#include "%s"'%i, file=h) for i in all_headers]
        cxx = 'g++'
        if 'CXX' in os.environ:
            cxx = os.environ['CXX']
        cmd = [cxx, '-o', compiled, '-E']
        for i in include_paths:
            cmd += ['-I', i]
        cmd.append(hpp)
        print(' '.join(cmd))
        os.system(' '.join(cmd))
        with open(compiled, 'r') as out:
            for line in out:
                words = line.split()
                if len(words)>=2 and (words[0]=='class' or words[0]=='struct'):
                    classes.append(words[1])
        return classes
    finally:
        shutil.rmtree(d)

if __name__=='__main__':
    src_path = '../..'
    build_path = '../../bld'

    (include_paths,
     siconos_namespace,
     targets,
     generated_file,
     source_dir,
     generated_header) = parse_args()

    all_headers = get_headers(targets)

    doxygen_classes = classes_from_build_path(build_path)

    header_classes = classes_from_headers(all_headers, include_paths)

    # Find the join of the two lists
    classes = {k: v for k,v in doxygen_classes.items()
               if k in header_classes and not unwanted(k)}

    assign_priorities(classes, src_path)

    resolve_base_classes(classes)

    with open(generated_file, 'w') as dest_file:
        write_header(dest_file, ' '.join(sys.argv), generated_header)
        write_includes(dest_file, all_headers)

        class_list = [
            (cl['name'],
             cl['resolved_bases'],
             [m for m in cl['members'] if not unwanted(m)]
             )
            for cl in sorted(classes.values(),
                             key = lambda k: (k['priority'], k['name']))]

        write_classes(dest_file, class_list)

        with_base = [(cl['name'],
                      cl['priority'])
                     for cl in classes.values()
                     if len(cl['bases'])>0 and not cl['abstract']]

        # filtering is not correct at this point
        # some unwanted classes are necessary
        # (the ones in SiconosFull.hpp) others not (xml)
        # some leads to compilation errors.
        write_register_with_bases(dest_file, with_base)

        write_footer(dest_file)
