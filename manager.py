#!/user/bin/python

import sys, os, commands
import time

class Manager:
    def __init__(self):
        self.root = ""
        self.file_path = "server.cfg"
        self.content = []
        self.sh_file_path = "manager.sh"

    def readFile(self):
        f = open(self.file_path)
        for line in f:
            self.content.append(line)
        f.close()

    def getTargetLine(self, tag):
        for i in range(0, len(self.content)):
            if self.content[i].startswith(tag) :
                return i
        return -1

    def processLine(self, tag, code, resp):
        idx = self.getTargetLine(tag)
        if idx < 0:
            return
        k_v = self.content[idx].split("=")
        value = k_v[1]
        pairs = value.split(";")
        ret = {}
        for item in pairs:
            x = item.split(":")
            ret[x[0]] = x[1]
        ret[code] = resp
        tmp = ["%s:%s" % (k, v) for k, v in ret.items()]
        self.content[idx] = "%s=%s" % (tag, ";".join(tmp))
        self.toFile()
        
    def toFile(self):
        f = open(self.file_path, "w")
        f.write("".join(self.content))
        f.close()

    def refreshCfg(self, code, resp):
        cmd = "sh %s refresh" % self.sh_file_path
        s, o = commands.getstatusoutput(cmd)
        time.sleep(1)
        if s == 0:
            cmd = 'curl "127.0.0.1:9876/getUserSetting?user_id=%s"' % code
            s, o = commands.getstatusoutput(cmd)
            if s == 0:
                return o == resp
            else:
                print "Run cmd [%s] failed." % cmd
        else:
            print "Run cmd [%s] failed." % cmd
        return False

    def doIt(self, code, resp):
        self.readFile()
        self.processLine("special_setting", code, resp)
        if self.refreshCfg(code, resp):
            print "Config not take effect."
        else:
            print "Done!"

if __name__ == "__main__":
    m = Manager();
    if len(sys.argv) < 3:
        exit(0)
    m.doIt(sys.argv[1], sys.argv[2])
        
