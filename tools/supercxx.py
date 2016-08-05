import commands
import os
import sys

class SuperCxx:
    def __init__(self):
        self.obj_path = []
        self.header_path = []
        self.lib_path = []
        self.libs_required = [] # such as "pthread" "lua5.1"
        self.extra_option = ["-std=c++11", "-g"]
        self.root = ""
        self.cxx_root = "/home/licheng/workspace/cxxtest"
        self.test_root = "/home/licheng/workspace/simpletest"
        self.tmp_h_file = "/home/licheng/workspace/cxxtest/h.tpl"

    def do_test(self, test_name = None):
        os.system("rm -f main")
        if (test_name != None):
            cpp_fpath = self.create_cpp_file(test_name)
            self.run_single_test(cpp_fpath)
            return 
        for f in os.listdir(self.test_root):
            if f.startswith("test") and f.endswith(".h"):
                cpp_fpath = self.create_cpp_file(os.path.join(self.test_root, f))
                self.run_single_test(cpp_fpath)

    def create_h_file(self, dest):
        cmd = "cp %s %s" % (self.tmp_h_file, dest + ".h")
        os.system(cmd)

    def create_cpp_file(self, h_file):
        cpp_file = os.path.basename(h_file)[:-2] + ".cpp"
        cpp_file_path = os.path.join(self.test_root, cpp_file)
        cmd = "%s/bin/cxxtestgen --error-printer  %s -o %s" % \
              (self.cxx_root, h_file, cpp_file_path)
        os.system(cmd)
        return cpp_file_path

    def run_single_test(self, cpp_file):
        print "======[%s]======"% os.path.basename(cpp_file)[:-4]
        libs = " -L " + " -L ".join(self.lib_path) if self.lib_path else ""
        headers = " -I " + " -I ".join(self.header_path) if self.header_path else ""
        objs = " ".join(self.obj_path)
        links = "-l " + " -l ".join(self.libs_required) if self.libs_required else ""
        extra = " ".join(self.extra_option) if self.extra_option else ""
        build_str = "g++ %s -o main %s %s %s %s %s" % (extra, cpp_file, libs, headers, objs, links)
        #print build_str
        o, s = commands.getstatusoutput(build_str)
        if o != 0:
            print "Build test [%s] failed!" % cpp_file
            print s
        self.run_test()

    def get_run_env(self):
        extra = ":".join(self.lib_path)
        os.environ['LD_LIBRARY_PATH'] = extra
        return "LD_LIBRARY_PATH = $LD_LIBRARY_PATH:%s" % extra

    def run_test(self):
        cmd = "echo $LD_LIBRARY_PATH;./main" 
        s, o = commands.getstatusoutput(cmd)
        print o

if __name__ == "__main__":
    sc = SuperCxx()
    root = "/home/licheng/workspace/CgServerPlus"
    obj_path = root + "/build/obj"
    
    obj_exclude = ["cgserver.o"]
    for f in os.listdir(obj_path):
        if f not in obj_exclude:
            sc.obj_path.append(os.path.join(obj_path, f))
    sc.header_path = [os.path.join(root, "depend/headers"),
                      root,
                      os.path.join(root, "tools/test/cxxtest-4.3")]
    sc.lib_path = [os.path.join(root, "depend/libs")]
    sc.libs_required = ["pthread", "lua5.1", "mysqlclient", "glog", "glfw"]
    sc.root = os.path.join(root, "tools/test")
    sc.cxx_root = os.path.join(root, "tools/test/cxxtest-4.3")
    sc.test_root = os.path.join(root, "tools/test")
    sc.tmp_h_file = os.path.join(root, "tools/h.tpl")

    arg_len = len(sys.argv)
    if arg_len == 1:
        print "Commands: "
        print "new [TestName] : create a new test header file."
        print "run : run all test case."
        print "run <casename> : run test case [casename]."
        exit(1)
    if sys.argv[1] == "new":
        if arg_len < 3:
            print "test name is needed."
            exit(1)
        sc.create_h_file(sys.argv[2])
    elif sys.argv[1] == "run":
        if arg_len == 2:
            sc.do_test()
        else:
            sc.do_test(sys.argv[2])
    else:
        print "Invalid input."
            
