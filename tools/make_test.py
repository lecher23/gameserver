#!/usr/bin/python
import sys
import os
import time
import commands

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
    if (len(sys.argv) == 1):
        print "Commands:"
        print "[crt]: Clear test space and create new clean test env."
        print "[test]: Complie test project and run it."
        print "[bk]: Backup main.cpp int test folder."
        print "[task1 src.cpp]: Use src.cpp to replace main.cpp in test folder."
        exit(1)
    else:
        param = sys.argv[1]
        
    if param == "crt":
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
        extern_libs_path = os.path.join(DEST, "libs")
        extern_headers = os.path.join(ROOT, "depend/headers")
        cmd = "cd %s && export LD_LIBRARY_PAHT=%s:$LD_LIBRARY_PAHT;g++ -std=c++11 -g -o test_main main.cpp %s -l pthread -L %s -l lua5.1 -I %s" % (DEST, extern_libs_path, obj_str, extern_libs_path, extern_headers)
        print cmd
        os.system(cmd)
        # exe test
        cmd = "cd %s &&export LD_LIBRARY_PATH=%s:$LD_LIBRARY_PATH; ./test_main" % (DEST, extern_libs_path)
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
    elif param == "task2":
        cmd = "ls %s -lrth | tail -n 1|awk '{print $9}'" % (os.path.join(ROOT, 'tools/tcode'))
        print cmd
        s, o = commands.getstatusoutput(cmd)
        src = os.path.join(ROOT, 'tools/tcode', os.path.basename(o))
        dest = os.path.join(DEST, 'main.cpp')
        cmd = "cp %s %s" % (src, dest)
        print cmd
        os.system(cmd)
