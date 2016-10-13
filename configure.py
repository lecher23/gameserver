#!/usr/bin/python

import sys
import commands
import os
from optparse import OptionParser

CUR = os.path.split(os.path.realpath(__file__))[0]
TMP_DIR = os.path.join(CUR, ".cgservertmp")
BOOST_GIT = "https://github.com/boostorg"
INCLUDE_DIR = os.path.join(CUR, "depend/include")
LIB_DIR = os.path.join(CUR, "depend/lib")

def exe_cmd_no_exit(cmd):
    s, o = commands.getstatusoutput(cmd)
    if s != 0:
        print "Exe cmd [%s] failed: [%s]" % (cmd, o)
        return False
    print "Exe cmd [%s] done." % cmd
    return True

def exe_cmd(cmd):
    s, o = commands.getstatusoutput(cmd)
    if s != 0:
        print "Exe cmd [%s] failed: [%s]" % (cmd, o)
        exit(0)
    print "Exe cmd [%s] done." % cmd

def exe_cmd_with_retry(cmd, retry_times = 4):
    times = 0
    while not exe_cmd_no_exit(cmd):
        times += 1
        if times >= retry_times:
            exit(1)

def clone_from_git(git, version, dest):
    cmd = "git clone %s %s" % (git, dest)
    exe_cmd_with_retry(cmd)
    cmd = "cd %s && git checkout %s" % (dest, version)
    exe_cmd_with_retry(cmd)

def get_boost_hpp_lib(lib_name, version):
    cmd = "git clone %s/%s.git %s/%s" % (BOOST_GIT, lib_name, TMP_DIR, lib_name)
    exe_cmd_with_retry(cmd)
    cmd = "cd %s && git checkout %s" % (os.path.join(TMP_DIR, lib_name), version)
    exe_cmd_with_retry(cmd)
    cmd = "cp -r %s/include/boost/* %s" % (os.path.join(TMP_DIR, lib_name), os.path.join(INCLUDE_DIR, "boost"))
    exe_cmd(cmd)

def resolve_boost_dependency():
    get_boost_hpp_lib("bind", "boost-1.61.0")
    get_boost_hpp_lib("tokenizer", "boost-1.61.0")
    get_boost_hpp_lib("math", "boost-1.61.0")
    get_boost_hpp_lib("move", "boost-1.61.0")
    get_boost_hpp_lib("container", "boost-1.61.0")
    get_boost_hpp_lib("functional", "boost-1.61.0")
    get_boost_hpp_lib("array", "boost-1.61.0")
    get_boost_hpp_lib("integer", "boost-1.61.0")
    get_boost_hpp_lib("numeric_conversion", "boost-1.61.0")
    get_boost_hpp_lib("lexical_cast", "boost-1.61.0")
    get_boost_hpp_lib("concept_check", "boost-1.61.0")
    get_boost_hpp_lib("detail", "boost-1.61.0")
    get_boost_hpp_lib("range", "boost-1.61.0")
    get_boost_hpp_lib("algorithm", "boost-1.61.0")
    get_boost_hpp_lib("io", "boost-1.61.0")
    get_boost_hpp_lib("utility", "boost-1.61.0")
    get_boost_hpp_lib("regex", "boost-1.61.0")
    get_boost_hpp_lib("bind", "boost-1.61.0")
    get_boost_hpp_lib("type_traits", "boost-1.61.0")
    get_boost_hpp_lib("preprocessor", "boost-1.61.0")
    get_boost_hpp_lib("mpl", "boost-1.61.0")
    get_boost_hpp_lib("static_assert", "boost-1.61.0")
    get_boost_hpp_lib("iterator", "boost-1.61.0")
    get_boost_hpp_lib("smart_ptr", "boost-1.61.0")
    get_boost_hpp_lib("config", "boost-1.61.0")
    get_boost_hpp_lib("predef", "boost-1.61.0")
    get_boost_hpp_lib("assert", "boost-1.61.0")
    get_boost_hpp_lib("core", "boost-1.61.0")
    get_boost_hpp_lib("throw_exception", "boost-1.61.0")
    get_boost_hpp_lib("asio", "boost-1.61.0")
    get_boost_hpp_lib("system", "boost-1.61.0")
    get_boost_hpp_lib("log", "boost-1.61.0")
    get_boost_hpp_lib("date_time", "boost-1.61.0")

def resolve_glog_dependency():
    clone_from_git("https://github.com/google/glog", "v0.3.4",
                   os.path.join(TMP_DIR, "glog"))
    exe_cmd("cd %s && ./configure --prefix=%s" %
            (os.path.join(TMP_DIR, "glog"), os.path.join(CUR, "depend")))
    exe_cmd("cd %s && make && make install" %
            os.path.join(TMP_DIR, "glog"))

def resolve_redis_client():
    git_dir = os.path.join(TMP_DIR, "hiredis")
    clone_from_git("https://github.com/redis/hiredis.git", "v.13.3", git_dir)
    exe_cmd("cd %s && make" % git_dir)
    exe_cmd("cd %s && cp *.so %s" % (git_dir, LIB_DIR))
    h_dir = os.path.join(INCLUDE_DIR, "hiredis")
    os.system("mkdir -p %s" )
    exe_cmd("cd %s && cp *.h %s" % (git_dir, h_dir))

def resolve_rapid_json():
    clone_from_git("https://github.com/miloyip/rapidjson", "v1.0.2",
                   os.path.join(TMP_DIR, "rapidjson"))
    exe_cmd("cp -r %s/include/* %s" % \
            (os.path.join(TMP_DIR, "rapidjson"), INCLUDE_DIR))

def resolve_lua():
    # easy install : apt-get install lua5.1 lua5.1-dev
    exe_cmd_with_retry("apt-get install lua5.1 lua5.1-dev")

def resolve_mysql_client():
    # for some reason, we use c connector rather than cpp connector
    tgz = "http://dev.mysql.com/get/Downloads/Connector-C/mysql-connector-c-6.1.6-linux-glibc2.5-x86_64.tar.gz"
    fname = os.path.basename(tgz)
    exe_cmd_with_retry("cd %s && wget %s" % (TMP_DIR, tgz))
    exe_cmd("cd %s && tar -zxf %s" % (TMP_DIR, fname))
    exe_cmd("cp -r %s/include/*  %s" %
            (os.path.join(TMP_DIR, fname.replace(".tar.gz", "")), INCLUDE_DIR))

def create_dir():
    os.system("mkdir -p %s" % os.path.join(CUR, "depend/include/boost"))
    os.system("mkdir -p %s" % os.path.join(CUR, "depend/lib"))

def resolve_all():
    resolve_boost_dependency()
    resolve_mysql_client()
    resolve_rapid_json()
    resolve_glog_dependency()
    resolve_redis_client()
    resolve_lua()

if __name__ == "__main__":
    if os.geteuid() != 0:
        print "This script must be run as root."
        exit(1)

    libs_to_resolve = {
        "all": resolve_all,
        "lua": resolve_lua,
        "boost": resolve_boost_dependency,
        "mysql": resolve_mysql_client,
        "rapidjson": resolve_rapid_json,
        "glog": resolve_glog_dependency,
        "redis": resolve_redis_client,
    }

    parser = OptionParser()
    parser.add_option("-t", "--target", dest = "target",
                      help = "target=[%s]" % ", ".join(libs_to_resolve.keys()))
    (options, args) = parser.parse_args()
    if options.target not in libs_to_resolve:
        print "Invalid target value, expected: [%s]" % ", ".join(libs_to_resolve.keys())
        exit(1)
    os.system("rm -rf %s" % TMP_DIR)
    os.system("mkdir %s" % TMP_DIR)
    create_dir()
    libs_to_resolve[options.target]()
    exit(0)
