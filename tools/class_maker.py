#!/usr/bin/python
import sys
import os

def createFile(fname, content):
    f = open(fname, "w")
    f.write(content)
    f.close()

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print "Class name required."
        exit(1)
    cname = sys.argv[1]
    utag = cname.upper() + "_H"
    hc = "#ifndef %s\n#define %s\nnamespace cgserver{\n    class %s{\n    public:\n        %s();\n        ~%s();\n    };\n}\n#endif\n" %\
         (utag, utag, cname, cname, cname)
    createFile(cname.lower() + ".h", hc)
    cppc = '#include "%s.h"\nnamespace cgserver{\n    %s::%s(){\n    }\n    %s::~%s(){\n    }\n}' %\
           (cname.lower(), cname, cname, cname, cname)
    createFile(cname.lower() + ".cpp", cppc)    

    
