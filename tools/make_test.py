#!/usr/bin/python
import sys
import os
import time

def createFile(fname, content):
    f = open(fname, "w")
    f.write(content)
    f.close()

ROOT = "/home/licheng/workspace/CgServerPlus"
DEST = os.path.join(ROOT, "tools/test")

def cp(fpath):
    prefix = fpath[len(ROOT):]
    if prefix[0] == '/':
        prefix = prefix[1:]
    dest_path = os.path.join(DEST, prefix)
    dest_dir = os.path.dirname(dest_path)
    if not os.path.exists(dest_dir):
        cmd = "mkdir -p %s" % dest_dir
        os.system(cmd)
    cmd = "cp %s %s" % (fpath, dest_path)
    os.system(cmd)

def processDir(dpath):
    files = os.listdir(dpath)
    for f in files:
        full_path = os.path.join(dpath, f)
        if f.endswith(".h"):
            cp(full_path)
        if os.path.isdir(full_path):
            processDir(full_path)

def stealObj(exclude = []):
    obj_path = os.path.join(ROOT, 'build/obj/*.o')
    dest_path = os.path.join(DEST, 'obj')
    os.system("mkdir %s"% dest_path)
    cmd = "cp %s %s" % (obj_path, dest_path)
    os.system(cmd)
    for ef in exclude:
        cmd = "rm -f %s" % os.path.join(dest_path, ef)
        os.system(cmd)

def clearSpace():
    cmd = "rm %s -rf" % os.path.join(DEST, "*")
    os.system(cmd)

if __name__ == "__main__":
    param = "create"
    if (len(sys.argv) > 1):
        param = sys.argv[1]
    if param == "create":
        # copy .h file to ./tmp >>
        clearSpace()
        processDir(ROOT)
        # copy depend *.o file to ./tmp/obj    
        stealObj(["cgserver.o"])
        # make main entrance
        cmd = "cp %s %s" % (os.path.join(DEST, '../tcode/test_template.cpp'),
                            os.path.join(DEST, 'main.cpp'))
        os.system(cmd)
    elif param == "test":
        objs = [os.path.join(DEST, 'obj', f) for f in os.listdir(os.path.join(DEST, 'obj'))]
        obj_str = " ".join(objs)
        cmd = "cd %s && g++ -std=c++11 -g -o test_main main.cpp %s -l pthread" % (DEST, obj_str)
        os.system(cmd)
        # exe test
        cmd = "cd %s && ./test_main" % DEST
        os.system(cmd)
    elif param == "bk":
        bk_name = "main_%d.cpp" % int(time.time())
        cmd = "cd %s && mv main.cpp %s" % (DEST, os.path.join(DEST, '../tcode', bk_name))
        print cmd
        os.system(cmd)
    elif param == "task1":
        src = os.path.join(ROOT, 'tools/tcode', os.path.basename(sys.argv[2]))
        dest = os.path.join(DEST, 'main.cpp')
        cmd = "cp %s %s" % (src, dest)
        print cmd
        os.system(cmd)

