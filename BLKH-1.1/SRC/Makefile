CC = gcc
IDIR = INCLUDE
ODIR = OBJ
CFLAGS = -O3 -Wall -I$(IDIR) -g 

_DEPS = GainType.h LKH.h BLKH.h

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = SolveBTSP.o SolveTSP.o SolveTransformedTSP_R.o                   \
       GetTimeUsage.o TwoMax.o BBSSP.o BBSSPA.o BSCSSP.o BAP.o          \
       C.o Distance.o Distance_SPECIAL.o eprintf.o fscanint.o GetTime.o \
       IsCandidate.o printff.o PrintParameters.o Random.o ReadLine.o    \
       ReadParameters.o ReadPenalties.o ReadProblem.o Statistics.o      \
       WriteTour.o

_OBJ_BLKH = BLKHmain.o $(_OBJ)
_OBJ_BLKH_EXP = BLKH_EXPmain.o $(_OBJ)

OBJ_BLKH = $(patsubst %,$(ODIR)/%,$(_OBJ_BLKH))
OBJ_BLKH_EXP = $(patsubst %,$(ODIR)/%,$(_OBJ_BLKH_EXP))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: 
	all clean

all:
	$(MAKE) BLKH
	$(MAKE) BLKH_EXP

BLKH: $(OBJ_BLKH) $(DEPS)
	$(CC) -o ../BLKH $(OBJ_BLKH) $(CFLAGS) $(LDFLAGS) -lm
BLKH_EXP: $(OBJ_BLKH_EXP) $(DEPS)
	$(CC) -o ../BLKH_EXP $(OBJ_BLKH_EXP) $(CFLAGS) $(LDFLAGS) -lm
 
clean:
	/bin/rm -f $(ODIR)/*.o ../BLKH ../BLKH_EXP *~ ._* $(IDIR)/*~ $(IDIR)/._*
