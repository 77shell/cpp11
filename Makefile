#
# Delta Electronics, INC
# TPSBU, IESBG
# 
# 2011-Aug-22
# Max <max.yang@delta.com.tw>
#

DBG=y


ifneq ($(DBG),y)
DEBUG=
OPTIMIZE=-O2
PROFILE=
else
DEBUG=-g
OPTIMIZE=-O0 -DNDEBUG
PROFILE=-pg
endif

include ./Makefile.inc

INCLUDEPATH += -iquote$(SRC_ROOT)/include -iquote"/home/max/src/csu.linux/include" -I"/home/max/src/csu.linux/x86/gcc-9.3.0-17/usr/local/include"
LIBSPATH = -Wl,-rpath,/home/max/src/csu.linux/x86/gcc-9.3.0-17/usr/local/lib64


CXXFLAGS += -std=gnu++17 $(DEBUG) $(OPTIMIZE) $(PROFILE) $(INCLUDEPATH) $(LIBSPATH) $(LIBFLAGS)
CFLAGS   += -Wextra -Wno-override-init $(DEBUG) $(OPTIMIZE) $(PROFILE) $(INCLUDEPATH) $(LIBSPATH) $(LIBFLAGS)

COMPILE.cc = $(CXX) $(CXXFLAGS)
COMPILE.c  = $(CC)  $(CFLAGS)


vpath %.cpp .
vpath %.c .

BINS := hello \
	list \
	initializer_list \
	exception_type \
	exception \
	exception_13_5_2 \
	cond_var \
	catch_signals \
	unordered_map \
	shced

.PHONY: impl_pattern
impl_pattern: impl_pattern.cpp impl_pattern_test.cpp
	g++ -o$@ $^

.PHONY: impl
impl: impl.cpp impl.h impl_test.cpp
	g++ -std=c++14 $^ -o $@

.PHONY: all
all: $(BINS)

thread_id: CXXFLAGS += -pthread
#receiver: receiver.o
#	$(CXX) -o $@ $< -lcan

deque: deque.cpp
	$(COMPILE.cc) -o $@ $< -pthread

sha256: sha256.cpp
	$(COMPILE.cc) -I/home/max/src-local/csu.linux/x86/gcc-9.3.0-17/usr/local/include -L/home/max/src-local/csu.linux/x86/gcc-9.3.0-17/usr/local/lib -L/home/max/src-local/csu.linux/x86/gcc-9.3.0-17/usr/local/lib64 -o$@ $< -lcrypto -lssl -lgdbm -ldl

zip: zip.cpp
	$(COMPILE.cc) -L/home/max/src-local/csu.linux/x86/gcc-9.3.0-17/usr/local/lib -I/home/max/src-local/csu.linux/x86/gcc-9.3.0-17/usr/local/include -o $@ $< -lzip -lz -lgdbm -ldl

unzip: unzip.cpp
	$(COMPILE.cc) -L/home/max/src/csu.linux/x86/gcc-9.3.0-17/usr/local/lib -I/home/max/src/csu.linux/x86/gcc-9.3.0-17/usr/local/include  -o$@ $< -lzip -lz -lgdbm -ldl

gdbm_open: gdbm_open.cpp
	$(COMPILE.cc) -L/home/max/src/csu.linux/x86/gcc-9.3.0-17/usr/local/lib -I/home/max/src/csu.linux/x86/gcc-9.3.0-17/usr/local/include -o$@ $< -lzip -lz -lgdbm -ldl

gdbm_load: gdbm_load.c
	$(COMPILE.c) -L/home/max/src/csu.linux/x86/gcc-9.3.0-17/usr/local/lib -I/home/max/src/csu.linux/x86/gcc-9.3.0-17/usr/local/include -I/home/max/src/csu.linux/3rdparty/gdbm-1.22 -iquote/home/max/src/csu.linux/3rdparty/gdbm-1.22/src -o$@ $< -lzip -lz -lgdbm -ldl

cond_var: cond_var.cpp
	$(CXX) -std=c++14 -pthread -o$@ $^

att-20kw-test: att-20kw-test.cpp
	$(CXX) -std=c++14 -pthread -o$@ $^

stack-overflow: stack-overflow.cpp
	$(CXX) -fstack-protector-strong -fsanitize=address -o$@ $^

test_ptr: test_ptr.cpp
	$(CXX) -pthread -o$@ $^

sem: CXXFLAGS += -pthread
ctor: CXXFLAGS += -pthread
shared_ptr2: CXXFLAGS += -pthread
share_ptr: CXXFLAGS += -std=gnu++2a
mutex: CXXFLAGS += -pthread
forward_list: CXXFLAGS += -pthread
datum_cstr: CXXFLAGS += -fstack-protector-all -fsanitize=address
sched: CXXFLAGS = -pthread

%: %.c
	$(COMPILE.c) -o $@ $< $(LIB)

%: %.cpp
	$(COMPILE.cc) -o$@ $< $(LIB)

.PHONY: clean
clean:
	rm -rf *.o $(csu502) db_para.*
	find . -regextype posix-egrep -iregex '.*\.#.*' | xargs rm
	find . -iregex .*~$ | xargs rm
	rm $(BINS) *.o *~ -rf
	find . -type f -exec sh -c "file {} | grep -i elf > /dev/null && rm {}" \;

.PHONY: TAGS
TAGS:
	rm -rf TAGS
	find . -regextype posix-egrep -iregex '.*\.(cpp|c|h)' > search.log
	perl filter.pl search.log | xargs etags -a
	rm -rf search.log
