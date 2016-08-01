# CgServerPlus
## This is a http server.

# Depend Glog
## make install infomation (under ubuntu 16.04):

libtool: install: /usr/bin/install -c .libs/libglog.so.0.0.0 /usr/local/lib/libglog.so.0.0.0
libtool: install: (cd /usr/local/lib && { ln -s -f libglog.so.0.0.0 libglog.so.0 || { rm -f libglog.so.0 && ln -s libglog.so.0.0.0 libglog.so.0; }; })
libtool: install: (cd /usr/local/lib && { ln -s -f libglog.so.0.0.0 libglog.so || { rm -f libglog.so && ln -s libglog.so.0.0.0 libglog.so; }; })
libtool: install: /usr/bin/install -c .libs/libglog.lai /usr/local/lib/libglog.la
libtool: install: /usr/bin/install -c .libs/libglog.a /usr/local/lib/libglog.a
libtool: install: chmod 644 /usr/local/lib/libglog.a
libtool: install: ranlib /usr/local/lib/libglog.a
libtool: finish: PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/sbin" ldconfig -n /usr/local/lib
----------------------------------------------------------------------
Libraries have been installed in:
   /usr/local/lib

If you ever happen to want to link against installed libraries
in a given directory, LIBDIR, you must either use libtool, and
specify the full pathname of the library, or use the '-LLIBDIR'
flag during linking and do at least one of the following:
   - add LIBDIR to the 'LD_LIBRARY_PATH' environment variable
     during execution
   - add LIBDIR to the 'LD_RUN_PATH' environment variable
     during linking
   - use the '-Wl,-rpath -Wl,LIBDIR' linker flag
   - have your system administrator add LIBDIR to '/etc/ld.so.conf'

See any operating system documentation about shared libraries for
more information, such as the ld(1) and ld.so(8) manual pages.
----------------------------------------------------------------------
 /bin/mkdir -p '/usr/local/share/doc/glog-0.3.4'
 /usr/bin/install -c -m 644 AUTHORS COPYING ChangeLog INSTALL NEWS README README.windows doc/designstyle.css doc/glog.html '/usr/local/share/doc/glog-0.3.4'
 /bin/mkdir -p '/usr/local/include/glog'
 /usr/bin/install -c -m 644 src/glog/log_severity.h '/usr/local/include/glog'
 /bin/mkdir -p '/usr/local/include/glog'
 /usr/bin/install -c -m 644 src/glog/logging.h src/glog/raw_logging.h src/glog/vlog_is_on.h src/glog/stl_logging.h '/usr/local/include/glog'
 /bin/mkdir -p '/usr/local/lib/pkgconfig'
 /usr/bin/install -c -m 644 libglog.pc '/usr/local/lib/pkgconfig'
make[1]: Leaving directory '/home/licheng/workspace/dev_tools/glog'
