#Sets the root directory for this package
ROOTPLAT=/home/piyasak/testEMA/Elektron-SDK1.1.0.linux/Ema/
ARCHTYPE=$(shell uname -m)
OS_VER=$(shell lsb_release -r | sed 's/\.[0-9]//')
CC=/usr/bin/g++
DIST=$(shell lsb_release -i)
CFLAGS += -m64 -D_REENTRANT -DLinux -O3 -DCOMPILE_64BITS -std=c++11 

# check for RedHat Linux first
ifeq ($(findstring RedHat,$(DIST)),RedHat)
	ifeq ($(findstring 6, $(OS_VER)), 6)
		LIB_DIR=$(ROOTPLAT)/Libs/RHEL6_64_GCC444
		OUTPUT_DIR=./RHEL6_64_GCC444
	endif
else 
	ifeq ($(findstring OracleServer, $(DIST)), OracleServer)
		ifeq ($(findstring 6, $(OS_VER)), 6)
			LIB_DIR=$(ROOTPLAT)/Libs/RHEL6_64_GCC444
			OUTPUT_DIR=./OL6_64_GCC444
		else
			LIB_DIR=$(ROOTPLAT)/Libs/OL7_64_GCC482
			OUTPUT_DIR=./OL7_64_GCC482
		endif
	else
		ifeq ($(findstring CentOS,$(DIST)),CentOS) 
			ifeq ($(findstring 7, $(OS_VER)), 7)
				LIB_DIR=$(ROOTPLAT)/Libs/OL7_64_GCC482
				OUTPUT_DIR=./CENTOS7_64_GCC482
			endif
		endif
	endif
endif

ifndef LIB_DIR	
$(error unsupported environment)
endif

CFLAGS += -D_REENTRANT
CLIBS = $(LIB_DIR)
LIB_PATH = -L$(LIB_DIR)
HEADER_INC = -I$(ROOTPLAT)/Src/Include -I$(ROOTPLAT)/Src 
EMA_LIBS = $(LIB_DIR)/Optimized/libema.a
EXE_DIR=$(OUTPUT_DIR)/Optimized
LN_HOMEPATH=../..

OS_NAME= $(shell uname -s)

export COMPILER=gnu
export CC=/usr/bin/g++
CFLAGS += -DLinux 
SYSTEM_LIBS = -lnsl -lpthread -lrt -ldl 
LNCMD = ln -is

$(OUTPUT_DIR)/obj/%.o : %.cpp
	rm -f $@
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(HEADER_INC) -o $@ $<


$(OUTPUT_DIR)/%.o : %.cpp
	rm -f $@
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(HEADER_INC) -o $@ $<

main: emahybridapp

EMAHybridApp_SRC = \
        ClientManager.cpp \
        EMAClient.cpp \
		EMAHybridApp.cpp

EMAHybridApp_HEAD = \
    ClientManager.h \
	EMAClient.h \
    EMAHybridApp.h

EMAHybridApp_objs = $(addprefix $(OUTPUT_DIR)/obj/,$(EMAHybridApp_SRC:%.cpp=%.o))

emahybridapp: $(EMAHybridApp_objs) 
	mkdir -p $(EXE_DIR)
	$(CC) -g -o $(EXE_DIR)/$@ $(EMAHybridApp_objs) $(CFLAGS) $(HEADER_INC) $(LIB_PATH) $(EMA_LIBS) $(SYSTEM_LIBS) 
	cp -f EmaConfig.xml $(EXE_DIR)
	cp -f itemlist.txt $(EXE_DIR)
	cp -f $(ROOTPLAT)/Etc/RDMFieldDictionary $(EXE_DIR)
	cp -f $(ROOTPLAT)/Etc/enumtype.def $(EXE_DIR)
	rm -rf $(OUTPUT_DIR)/obj/
