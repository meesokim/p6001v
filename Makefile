CC = @g++.exe
AR = @ar.exe
RC = @windres.exe


SRCDIR	= src
SRCDOSD	= $(SRCDIR)/Win32
SRCDDEV	= $(SRCDIR)/device

OBJDIR	= o
OBJDOSD	= $(OBJDIR)/Win32
OBJDDEV	= $(OBJDIR)/device

TARGET	= pc6001v.exe

OBJC	= breakpoint.o common.o config.o console.o cpum.o cpus.o	\
	  d88.o debug.o	device.o disk.o error.o graph.o ini.o intr.o	\
	  io.o joystick.o keyboard.o memory.o movie.o p6t2.o		\
	  p6el.o p6vm.o pc6001v.o pio.o psgfm.o replay.o schedule.o	\
	  sound.o status.o tape.o voice.o vdg.o vsurface.o
OBJDEV  = ay8910.o mc6847.o pd7752.o pd8255.o z80.o z80-dbg.o		\
	  ym2203.o \
	  fmgen/fmgen.o fmgen/psg.o fmgen/opna.o fmgen/fmtimer.o
OBJOSD	= guiWIN32.o osdWIN32.o semaphore.o thread.o
OBJC2   = $(addprefix $(OBJDIR)/,  $(OBJC))
OBJDEV2 = $(addprefix $(OBJDDEV)/, $(OBJDEV))
OBJOSD2 = $(addprefix $(OBJDOSD)/, $(OBJOSD))
OBJS	= $(OBJC2) $(OBJDEV2) $(OBJOSD2)

OBJRES	= $(OBJDOSD)/p6v.ro

DEPENDS = $(OBJS:.o=.d)



ifdef BIT32
CFLG1	= -m32
LFLG1	= -m32
RFLG1	= -F pe-i386
else
CFLG1	= -m64
LFLG1	= -m64
RFLG1	= -F pe-x86-64
endif

CFLG2	= -Wextra -fno-strict-aliasing -mms-bitfields \
	  -MMD -MP \
	  -finput-charset=cp932 -fexec-charset=cp932 \
	  $(shell sdl-config --cflags) $(shell libpng-config --cflags)
#LFLG2	=
LFLG2	= -static-libgcc -static-libstdc++ 

ifndef DEBUG
CFLG3	= -O3 -fomit-frame-pointer
LFLG3	= -s
else
CFLG3	= -g -pg -O0
LFLG3	= -g -pg
endif

CFLAGS	= $(CFLG1) $(CFLG2) $(CFLG3) -I/usr/local/include
LFLAGS	= $(LFLG1) $(LFLG2) $(LFLG3) -L/usr/local/lib


LIBS	= $(shell sdl-config --libs) $(shell libpng-config --libs) -lz -lcomctl32 -lwinmm -lshlwapi
RFLAGS	= $(RFLG1) --language=0411 -J rc -O coff -I./$(SRCDOSD)/res


.PHONY: all build clean

all : build

build : $(TARGET)

$(TARGET) : $(OBJS) $(OBJRES)
	@echo Linking $@...
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(OBJRES) $(LIBS)
ifndef DEBUG
	@strip -s ./$(TARGET)
endif

$(OBJDDEV)/fmgen/%.o : $(SRCDDEV)/fmgen/%.cpp Makefile
	@echo Compiling $<...
	@if [ ! -d $(OBJDDEV)/fmgen ]; then mkdir $(OBJDDEV)/fmgen; fi
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDDEV)/%.o : $(SRCDDEV)/%.cpp Makefile
	@echo Compiling $<...
	@if [ ! -d $(OBJDDEV) ]; then mkdir $(OBJDDEV); fi
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDOSD)/%.o : $(SRCDOSD)/%.cpp Makefile
	@echo Compiling $<...
	@if [ ! -d $(OBJDOSD) ]; then mkdir $(OBJDOSD); fi
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp Makefile
	@echo Compiling $<...
	@if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDOSD)/%.ro : $(SRCDOSD)/%.rc $(SRCDOSD)/id_*.h Makefile
	@echo Compiling resources $<...
	@if [ ! -d $(OBJDOSD) ]; then mkdir $(OBJDOSD); fi
	$(RC) $(RFLAGS) -i $< -o $@

clean:
	@echo Deleting objects ...
	-@$(RM) $(TARGET)
	-@$(RM) $(OBJS)
	-@$(RM) $(OBJRES)
	-@$(RM) $(DEPENDS)
	-@$(RM) -r $(OBJDIR)

-include $(DEPENDS)
