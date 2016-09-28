#coding: utf-8
import sys
import os

cur_dir = os.path.split(os.path.realpath(__file__))[0]

svn_path = "/home/licheng/Documents/slotdesign"

temple_princess_path = "配置表/主题配置/TemplePrincess"
temple_princess_config_file = ["*.json"]

config_dest = "game_config"

def copy_file(src, dest):
    os.system("cp %s %s" % (src, dest))

def update_svn(svn_dir):
    os.system("cd %s && svn up" % svn_dir)

if __name__ == "__main__":
    cp = svn_path
    if len(sys.argv) != 1:
        cp = sys.argv[1]
    update_svn(cp)
    for f in temple_princess_config_file:
        copy_file(os.path.join(cp, temple_princess_path, f),
                  os.path.join(cur_dir, config_dest, "TemplePrincess"))
