
all: check lib

NIMBUS_ROOT = .
include $(NIMBUS_ROOT)/Makeinclude

LIBRARY = libnimbus.so

CFLAGS += -fPIC

SCHED_CFILES  = $(wildcard scheduler/*.cc)
WORKER_CFILES = $(wildcard worker/*.cc)
DATA_CFILES   = $(wildcard data/*.cc) $(wildcard data/physbam/*.cc)
SHARED_CFILES = $(wildcard shared/*.cc)

CFILES = $(SCHED_CFILES) $(WORKER_CFILES) $(DATA_CFILES) $(SHARED_CFILES) $(SHARED_BUF_CFILES)
HFILES = $(wildcard *.h)
OBJFILES = $(subst .cc,.o,$(CFILES))


SHARED_PROTO_OBJECT_FILES = $(wildcard shared/protobuf_compiled/*.pb.o)
OBJFILES += $(SHARED_PROTO_OBJECT_FILES)

DATA_PROTO_OBJECT_FILES = $(wildcard data/physbam/protobuf_compiled/*.pb.o)
OBJFILES += $(DATA_PROTO_OBJECT_FILES)


LFLAGS += -lboost_thread-mt -lboost_system-mt -lprotobuf
SHARED_FLAGS = -shared -fPIC

ifdef OS_DARWIN
  LINK_FLAG = -install_name @rpath/$(LIBRARY)
endif

lib: $(LIBRARY)

.PHONY: scheduler_t worker_t data_t shared_t
scheduler_t: shared_t 
	cd scheduler; make; cd ..

worker_t: shared_t
	cd worker; make; cd ..

data_t: shared_t
	cd data; make; cd ..

shared_t:
	cd shared; make; cd ..

$(LIBRARY): shared_t scheduler_t worker_t data_t
	$(CPP) $(SHARED_FLAGS) $(CFLAGS) $(IFLAGS) $(LDFLAGS) $(LFLAGS) $(OBJFILES) -o $(LIBRARY) $(LINK_FLAG)

clean: clean-files
	\rm -f */*.o */*~ */\#*
	\rm -f $(LIBRARY)

