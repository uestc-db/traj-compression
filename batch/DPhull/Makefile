#File Names
#CFLAGS = -O  -s 
CFLAGS = -g  -DANIMATE 
LDFLAGS = -lm -lX11  
LINTFLAGS = -c 
LINTALLFLAGS = -lm -lgl 
HDRS = DP.h animate.h PH.h
SRCS = DP.c DPhullfast.c PH.c DPhull.c animate.c nonrec.c
H_OBJS = DP.o PH.o DPhull.o animate.o
N_OBJS = DP.o nonrec.o animate.o
OBJS = $(H_OBJS) $(N_OBJS)
UBJS = DP.u  DPhullfast.u nonrec.u
LINTS = DP.ln animate.ln PH.ln DPhull.ln nonrec.ln
EXECS = hull nonrec

all: 	hull nonrec

opt:	$(SRCS) $(HDRS) 
	cc -O4 -s DP.c DPhullfast.c -lm -o hull
	cc -O4 -s DP.c nonrec.c -lm -o nonrec

hull:	$(H_OBJS)
		cc $(CFLAGS) $(H_OBJS) $(LDFLAGS) -o hull

nonrec:	$(N_OBJS)
		cc $(CFLAGS) $(N_OBJS) $(LDFLAGS) -o nonrec

$(OBJS): $(HDRS)

lint:	$(LINTS)

lintall: $(LINTS)
		lint $(LINTALLFLAGS) $(LINTS)

clean:	
		rm $(OBJS) $(UBJS)
		
nuke:	
		rm $(LINTS) $(EXECS) $(OBJS)

print: 	$(HDRS) $(SRCS) 
		lwf -s7 -t4 -l -p-2 $? | lpr
		touch print

.SUFFIXES:	.c .o .ln .h .out .u

.c.o:
		cc $(CFLAGS) -c $<

.c.ln:
		lint $(LINTFLAGS) $(CFLAGS) $<
