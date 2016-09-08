#!/usr/bin/python

import sys
import commands
import os

CUR = os.path.split(os.path.realpath(__file__))[0]
TMP_DIR = os.path.join(CUR, ".cgservertmp")
BOOST_GIT = "https://github.com/boostorg"
INCLUDE_DIR = os.path.join(CUR, "depend/include")
LIB_DIR = os.path.join(CUR, "depend/lib")

def exe_cmd(cmd):
    s, o = commands.getstatusoutput(cmd)
    if s != 0:
        print "Exe cmd [%s] failed: [%s]" % (cmd, o)
        exit(0)
    print "Exe cmd [%s] done." % cmd

def clone_from_git(git, version, dest):
    cmd = "git clone %s %s" % (git, dest)
    exe_cmd(cmd)
    cmd = "cd %s && git checkout %s" % (dest, version)
    exe_cmd(cmd)

def get_boost_hpp_lib(lib_name, version):
    cmd = "git clone %s/%s %s/%s" % (BOOST_GIT, lib_name, TMP_DIR, lib_name)
    exe_cmd(cmd)
    cmd = "cd %s && git checkout %s" % (os.path.join(TMP_DIR, lib_name), version)
    exe_cmd(cmd)
    cmd = "cp -r %s/include/boost/* %s" % (os.path.join(TMP_DIR, lib_name), os.path.join(INCLUDE_DIR, "boost"))
    exe_cmd(cmd)

def resolve_boost_dependency():
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
    get_boost_hpp_lib("utility", "boost-1.61.0")

def resolve_glog_dependency():
    clone_from_git("https://github.com/google/glog", "v0.3.4",
                   os.path.join(TMP_DIR, "glog"))
    exe_cmd("cd %s && ./configure --prefix=%s" %
            (os.path.join(TMP_DIR, "glog"), os.path.join(CUR, "depend")))
    exe_cmd("cd %s && make && make install" %
            os.path.join(TMP_DIR, "glog"))

def resolve_rapid_json():
    clone_from_git("https://github.com/miloyip/rapidjson", "v1.0.2",
                   os.path.join(TMP_DIR, "rapidjson"))
    exe_cmd("cp -r %s/include/* %s" % \
            (os.path.join(TMP_DIR, "rapidjson"), INCLUDE_DIR))

def resolve_lua():
    # easy install : apt-get install lua5.1 lua5.1-dev
    exe_cmd("apt-get install lua5.1 lua5.1-dev")
    return
    # below is useless
    # 5.1 or other version
    # before make, we should use cmd:
    # sudo apt-get install libreadline6 libreadline6-dev
    tgz = "http://www.lua.org/ftp/lua-5.3.3.tar.gz"
    fname = os.path.basename(tgz)
    exe_cmd("cd %s && wget %s" % (TMP_DIR, tgz))
    exe_cmd("cd %s && tar -zxf %s" % (TMP_DIR, fname))
    exe_cmd("apt-get install libreadline6 libreadline6-dev")
    lua_dir = os.path.join(TMP_DIR, fname.replace(".tar.gz", ""))
    exe_cmd("cd %s && make linux" % lua_dir)
    exe_cmd("cp %s %s" %(os.path.join(lua_dir, "src/liblua.a") , LIB_DIR))

def resolve_mysql_client():
    # for some reason, we use c connector rather than cpp connector
    tgz = "http://dev.mysql.com/get/Downloads/Connector-C/mysql-connector-c-6.1.6-linux-glibc2.5-x86_64.tar.gz"
    fname = os.path.basename(tgz)
    exe_cmd("cd %s && wget %s" % (TMP_DIR, tgz))
    exe_cmd("cd %s && tar -zxf %s" % (TMP_DIR, fname))
    exe_cmd("cp -r %s/include/*  %s" %
            (os.path.join(TMP_DIR, fname.replace(".tar.gz", "")), INCLUDE_DIR))

if __name__ == "__main__":
    if os.geteuid() != 0:
        print "This script must be run as root."
        exit(1)
    os.system("rm -rf %s" % TMP_DIR)
    os.system("mkdir %s" % TMP_DIR)
    #resolve_mysql_client()
    #resolve_rapid_json()
    #resolve_lua()
    resolve_boost_dependency()
    exit(0)
