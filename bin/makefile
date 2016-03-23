ALL_TOOLS = \
	AssembleExons \
	AssembleFastq \
	FastqOut \
	GetInfo \
	GetKey \
	GetNameB4Dot \
	InsertInList \
	JSFilter \
	JSParser \
	JunctionSite2BED \
	PslChimeraFilter \
	RemoveInList \
	RetainInList \
	RmBadMapping \
	RmColinearPairInSam \
	RmRedundance \
	SeqOut


all: $(ALL_TOOLS)


AssembleExons: Common.o Table.o AssembleExons.o
	g++ Common.o Table.o AssembleExons.o -o AssembleExons

AssembleFastq: Common.o Table.o Seq.o AssembleFastq.o
	g++ Common.o Table.o Seq.o AssembleFastq.o -o AssembleFastq

FastqOut: Common.o Table.o Seq.o Map.o FastqOut.o
	g++ Common.o Table.o Seq.o Map.o FastqOut.o -o FastqOut

GetInfo: Common.o Table.o Map.o GetInfo.o
	g++ Common.o Table.o Map.o GetInfo.o -o GetInfo

GetKey: Common.o Table.o Map.o GetKey.o
	g++ Common.o Table.o Map.o GetKey.o -o GetKey

GetNameB4Dot: Common.o Table.o GetNameB4Dot.o
	g++ Common.o Table.o GetNameB4Dot.o -o GetNameB4Dot

InsertInList: Common.o Table.o Map.o InsertInList.o
	g++ Common.o Table.o Map.o InsertInList.o -o InsertInList

JSFilter: Common.o Table.o Sam.o JSFilter.o
	g++ Common.o Table.o Sam.o JSFilter.o -o JSFilter

JSParser: Common.o Table.o Sam.o JSParser.o
	g++ Common.o Table.o Sam.o JSParser.o -o JSParser

JunctionSite2BED: Common.o Table.o Map.o Annotation.o JunctionSite2BED.o
	g++ Common.o Table.o Map.o Annotation.o JunctionSite2BED.o -o JunctionSite2BED

PslChimeraFilter: Common.o Table.o PslChimeraFilter.o
	g++ Common.o Table.o PslChimeraFilter.o -o PslChimeraFilter

RemoveInList: Common.o Table.o Map.o RemoveInList.o
	g++ Common.o Table.o Map.o RemoveInList.o -o RemoveInList

RetainInList: Common.o Table.o Map.o RetainInList.o
	g++ Common.o Table.o Map.o RetainInList.o -o RetainInList

RmBadMapping: Common.o Table.o RmBadMapping.o
	g++ Common.o Table.o RmBadMapping.o -o RmBadMapping

RmColinearPairInSam: Common.o Table.o Sam.o RmColinearPairInSam.o
	g++ Common.o Table.o Sam.o RmColinearPairInSam.o -o RmColinearPairInSam

RmRedundance: Common.o Table.o Map.o RmRedundance.o
	g++ Common.o Table.o Map.o RmRedundance.o -o RmRedundance

SeqOut: Common.o Table.o Seq.o Map.o SeqOut.o
	g++ Common.o Table.o Seq.o Map.o SeqOut.o -o SeqOut


clean:
	rm -f *.o
	rm -f $(ALL_TOOLS)

