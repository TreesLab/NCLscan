NCLscan
=======
##### a pipeline to detect trans-splicing and ciricular RNA using rna-seq
--------------
### Manual of NCLscan pipeline
--------------
The package of NCLscan and the preparation of reference sequences can be downloaded
from our FTP site: ftp://sinica.grc.treeslab/NCLscan or 
GitHub: https://github.com/TreesLab/NCLscan

1. System Requirement

   The NCLscan pipeline is executed on the 64-bit Linux operation system 
   (e.g., Bio-Linux 6; also see http://nebc.nerc.ac.uk/ for further details). 
   The three used aligners (i.e., BWA, Novoalign, and BLAT) were involved in the package
   of NCLscan. They were downloaded from http://bio-bwa.sourceforge.net/,
   http://www.novocraft.com, and https://genome.ucsc.edu, respectively.

2. Preparation

   2.1. Reference sequences
   
   The genomic sequences (FASTA files) were downloaded from the GENCODE website at 
   http://www.gencodegenes.org/. Taking the human reference genome (GRCh37.p13) as 
   an example, the following 4 reference files should be 
   downloaded (http://www.gencodegenes.org/releases/19.html):
   
   (1) Genome sequence FASTA file in GRCh37.p13 assembly (sequence region names are
       the same as in the GTFs): GRCh37.p13.genome.fa.gz
       
   (2) Protein-coding transcript sequences in FASTA format: gencode.v19.pc_transcripts.fa.gz
   
   (3) Long non-coding RNAs in FASTA format: gencode.v19.lncRNA_transcripts.fa.gz
    
   (4) Gene annotation: gencode.v19.annotation.gtf.gz

   2.2. Configuration 
 
   The four reference sequence datasets and two RNA-seqs parameters are listed in order 
   in “config.txt”. The content of config.txt is just like:
   ```txt
   GRCh37.p13.genome.fa
   gencode.v19.pc_transcripts.fa
   gencode.v19.lncRNA_transcripts.fa
   gencode.v19.annotation.gtf
   151
   00
   ```
   Note: The final two parameters, 151 and 500, indicate the maximal read length and fragment 
   size of the used paired-ended RNA-seq data (FASTQ files). If the read length (L) < 151 bp and
   fragment size <= 500 bp, the users don't need to change these two values. If L > 150, 
   the users should change these two parameters to (L+1, L*2 + insert size).
 
   2.3 Setting
 
   All the reference files, the config.txt and the used paired-end RNA-seq data should put in 
   the same sub-directory where NCLscan executes.

3. NCLscan pipeline

   The NCLscan pipeline includes six steps, which are all involved in the batch file: NCLscan.sh

   Usage:
   >./NCLscan.sh 01.fastq 02.fastq MyProject 20

  Note: 01.fastq and 02.fastq are the two files of a set of paired-end RNA-seq data. MyProject is
  the prefix of output filenames (for example). The final parameter, 20, means the cutoff of 
  sequence quality score of reads for the RNA-seqs. The users can changes this cutoff quality score 
  depends on their cases. If 0 is given, it means no filtering any sequences.

  The six steps and their usages are explained as following:

  ⋅⋅⋅Step 1: NCL_Scan0
Aligning reads against the reference genome and the annotated transcripts by BWA. BWA needs its 
own reference genome to do mapping. This reference genome (GRCh37.p13) file can be downloaded from
our FTP site (“bwa_AllRef.fa”), or other websites which provide whole genome data (such as GENCODE,
UCSC, and etc.). If other reference genome for BWA was used, then changing the FASTA filename into 
“bwa_AllRef.fa” and putting in the same sub-directory where NCLscan executes.

Usage:
>./NCL_Scan0.sh 01.fastq 02.fastq MyProject

Step 2: NCL_Scan1
Aligning the reads unmapped by BWA against the reference genome and the annotated transcripts by Novoalign.

Usage:
> NCL_Scan1 MyProject.bwa.unmapped_1.fastq MyProject.bwa.unmapped_2.fastq MyProject

Step 3: NCL_Scan2
Concatenating the two ends for each unmapped read and Blat-aligning the concatenated sequences against 
the reference genome. Removing concatenated sequences with an alternative co-linear explanation. 
Note: A cutoff of sequence quality score of reads for the RNA-seqs is given. 

Usage:
> NCL_Scan2 20

Step 4: NCL_Scan3
Boundary sorting and aligning to find putative Non-co-linear (NCL) junction sites.

Usage:
> NCL_Scan3

Step 5: NCL_Scan4
Filtering and retaining the putative NCL sequences which satisfy all our criterions. 

Usage:
> NCL_Scan4

Step 6: Add_gene_name
Append gene name in the final result according to the gene annotation.

Usage:
> ./get_GeneName_1.1.py


4. NCLscan Outputs

Two output files are generated after executing the pipeline: a tab-delimited text "MyProject.result.gene_name", 
and a SAM-format file “MyProject.result.sam” which contains the supporting reads of the identified non-co-linear events.

The format of "MyProject.result.gene_name" is descripted as (table schema for each column):
(1) Reference Sequence name to identify each non-co-linear event found
(2) Chromosome name of the donor side (5'ss) 
(3) Junction coordinate of the donor side
(4) Strand of the donor side
(5) Chromosome name of the acceptor side (3'ss) 
(6) Junction coordinate of the acceptor side
(7) Strand of the acceptor side
(8) (Reserved column, no use)
(9) Distance to the far left-side end of alignments from the junction site
(10) Distance to the far right-side end of alignments from the junction site
(11) Total number of the supporting reads
(12) (Reserved column, no use)
(13) Gene name of the donor side
(14) Gene name of the acceptor side

The output format of "MyProject.result.sam" is followed as standard SAM format. The users can find the details in http://samtools.github.io/hts-specs/SAMv1.pdf
