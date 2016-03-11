ALL_TOOLS = \
	AssembleExons \
	AssembleFastq \
	AssembleJSeq \
	FastqOut \
	GetInfo \
	GetKey \
	GetNameB4Dot \
	InsertInList \
	JSFilter \
	JSParser \
	JunctionSite2BED \
	MakeBwaRef \
	NCL_Scan1 \
	NCL_Scan2 \
	NCL_Scan3 \
	NCL_Scan4 \
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

AssembleJSeq: Common.o Table.o AssembleJSeq.o
	g++ Common.o Table.o AssembleJSeq.o -o AssembleJSeq

FastqOut: Common.o Table.o Seq.o Map.o FastqOut.o
	g++ Common.o Table.o Seq.o Map.o FastqOut.o -o FastqOut

GetInfo: Common.o Table.o Map.o NCL_head.o GetInfo.o
	g++ Common.o Table.o Map.o NCL_head.o GetInfo.o -o GetInfo

GetKey: Common.o Table.o Map.o NCL_head.o GetKey.o
	g++ Common.o Table.o Map.o NCL_head.o GetKey.o -o GetKey

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

MakeBwaRef:Common.o Table.o NCL_head.o MakeBwaRef.o
	g++ Common.o Table.o NCL_head.o MakeBwaRef.o -o MakeBwaRef

NCL_Scan1: Common.o Table.o NCL_head.o NCL_Scan1.o
	g++ Common.o Table.o NCL_head.o NCL_Scan1.o -o NCL_Scan1

NCL_Scan2: Common.o Table.o NCL_head.o NCL_Scan2.o
	g++ Common.o Table.o NCL_head.o NCL_Scan2.o -o NCL_Scan2

NCL_Scan3: Common.o Table.o NCL_head.o NCL_Scan3.o
	g++ Common.o Table.o NCL_head.o NCL_Scan3.o -o NCL_Scan3

NCL_Scan4: Common.o Table.o NCL_head.o NCL_Scan4.o
	g++ Common.o Table.o NCL_head.o NCL_Scan4.o -o NCL_Scan4

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


install:
	cp $(ALL_TOOLS) ..

clean:
	rm -f *.o
	rm -f $(ALL_TOOLS)

