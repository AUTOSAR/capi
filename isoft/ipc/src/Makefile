#  Disclaimer
#
#  This work (specification and/or software implementation) and the material
#  contained in it, as released by AUTOSAR, is for the purpose of information
#  only. AUTOSAR and the companies that have contributed to it shall not be
#  liable for any use of the work.
#
#  The material contained in this work is protected by copyright and other
#  types of intellectual property rights. The commercial exploitation of the
#  material contained in this work requires a license to such intellectual
#  property rights.
#
#  This work may be utilized or reproduced without any modification, in any
#  form or by any means, for informational purposes only. For any other
#  purpose, no part of the work may be utilized or reproduced, in any form
#  or by any means, without permission in writing from the publisher.
#
#  The work has been developed for automotive applications only. It has
#  neither been developed, nor tested for non-automotive applications.
#
#  The word AUTOSAR and the AUTOSAR logo are registered trademarks.
#  --------------------------------------------------------------------------

ABE_ROOT ?= /abe

CFLAGS += -g -Wall -Wextra -Wno-unused-function
PIC_FLAGS := -fPIC

# ipcbase-dummy should be before ara/core/1.0.0/include to override the one under it.
ifeq ($(IPCBASE), dummy)
	CPPFLAGS += -I$(SRCDIR)/test/ipcbase-dummy/include/
endif
CPPFLAGS += -D_GNU_SOURCE

CPPFLAGS += -I$(SRCDIR)/include
CPPFLAGS += -I$(ABE_ROOT)/ara/core/1.0.0/include

LDFLAGS += -lpthread
LDFLAGS += -lpcre

# ara
ABE_DIR=$(ABE_ROOT)/usr/lib
ABE_ARA_DIR=$(ABE_ROOT)/ara/core/1.0.0/lib
LDFLAGS_DIR += -L$(ABE_DIR)
LDFLAGS_DIR += -L$(ABE_ARA_DIR)
LDFLAGS += -lnai
ifeq ($(IPCBASE), dummy)
	IPCBASE_DIR=$(SRCDIR)/test/ipcbase-dummy/src/.libs
	LDFLAGS_DIR += -L$(IPCBASE_DIR)
	LDFLAGS += -lphipcbase
else
	IPCBASE_DIR=$(ABE_ARA_DIR)
	LDFLAGS += -lisoft_ipcbase
endif

VERSION_MAJOR := 0
VERSION_MINOR := 1
VERSION_PATCH := 0
LIBRARY_NAME := phipc
LIBRARY_LINKER_NAME := lib$(LIBRARY_NAME).so
LIBRARY_SONAME := lib$(LIBRARY_NAME).so.$(VERSION_MAJOR)
LIBRARY_REAL_NAME := lib$(LIBRARY_NAME).so.$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)
LIBRARY_STATIC_NAME := lib$(LIBRARY_NAME).a

OBJS := buffer.o channel_uds.o ipc.o manager.o packet.o session.o server.o server_session.o event.o em/ipc_em.o peer_uds.o client.o
OBJDIR := .libs

.PHONY: mkbuilddir all clean

all: mkbuilddir $(LIBRARY_STATIC_NAME) $(LIBRARY_REAL_NAME)

mkbuilddir:
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/em

clean:
	rm -rf $(OBJDIR)
	- rm em/ipc_em.o

$(LIBRARY_STATIC_NAME) : $(OBJDIR)/$(LIBRARY_STATIC_NAME)

$(LIBRARY_REAL_NAME) : $(OBJDIR)/$(LIBRARY_REAL_NAME)
	(cd $(OBJDIR) && ln -s $@ $(LIBRARY_SONAME))
	(cd $(OBJDIR) && ln -s $@ $(LIBRARY_LINKER_NAME))

$(OBJDIR)/$(LIBRARY_STATIC_NAME): $(addprefix $(OBJDIR)/, $(OBJS))
	$(AR) cr $@ $^
	ranlib $@

$(OBJDIR)/$(LIBRARY_REAL_NAME): $(addprefix $(OBJDIR)/, $(OBJS))
	$(CC) -shared $(PIC_FLAGS) $(CFLAGS) -Wl,-soname -Wl,$(LIBRARY_SONAME) -o $@ ${LDFLAGS_DIR} $^ ${LDFLAGS}

$(OBJDIR)/%.o: %.c mkbuilddir
	$(CC) $(CFLAGS) $(CPPFLAGS) $(PIC_FLAGS) -c -o $@ $<
