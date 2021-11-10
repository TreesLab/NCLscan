## Manual of NCLscan
###### Version: 1.6.5

We have developed a new pipeline, NCLscan, which is rather advantageous in the identification of both intragenic and intergenic "non-co-linear" (NCL) transcripts (fusion, trans-splicing, and circular RNA) from paired-end RNA-seq data. 

--------------


The NCLscan program, document, and test set can be downloaded from our FTP site: ftp://treeslab1.genomics.sinica.edu.tw/NCLscan or GitHub: https://github.com/TreesLab/NCLscan.

Please check the "[release](https://github.com/TreesLab/NCLscan/releases)" page to download NCLscan tool. 



## Note
(2020/05/21)

- Use `bedtools` **v2.29.0** or lower
  - In the latest version (v2.29.2), the default behavior of the option `-name` of `bedtools getfasta` has been changed, and this change would induce parsing errors in `NCLscan`.
- ~Use `novoalign` **v3**~
  - Issue [#21](https://github.com/TreesLab/NCLscan/issues/21#issuecomment-598612660)
  - (2021/11/03) The novoalign v4 is now supported.


## 1. System requirements

   NCLscan runs under the Linux (i.e., Bio-Linux, see also http://nebc.nerc.ac.uk/ for further details) or Mac operating   systems on a 64-bit machine with at least 10 GB RAM. 

   The python scripts used in NCLscan are developed under Python2.7.

## 2. Installation

   The users can download NCLscan.tar.gz file from the "[release](https://github.com/TreesLab/NCLscan/releases)" page, and unzip it with the following command:
  
        > tar xvzf NCLscan.tar.gz


### 2.1.  Configuration

The users can edit the needed paths and some optional parameters in the config file "NCLscan.config".

### 2.2.  External tools

   The following five tools are involved in the NCLscan pipeline:
```
   (1) Bedtools (http://bedtools.readthedocs.org/en/latest/)
   (2) Samtools (http://www.htslib.org/)
   (3) BLAT (https://genome.ucsc.edu)
   (4) BWA (http://bio-bwa.sourceforge.net/)
   (5) Novoalign, novoindex, and novoalign license file (http://www.novocraft.com/)
```
   
   Note: Novoalign is a commercial bioinformatics tool. It can be requested for a limited license for academic/non-profit       researches.

### 2.3.  Reference preparation
 
The genomic sequences and annotation were downloaded from the GENCODE website at http://www.gencodegenes.org/. Given the   human reference genome (GRCh37.p13, http://www.gencodegenes.org/releases/19.html) as an example, the following four reference files are required:

```
   (1) Genome sequence FASTA file in GRCh37.p13 assembly (sequence region names are the same as in the GTFs): GRCh37.p13.genome.fa.gz
   (2) Protein-coding transcript sequences: gencode.v19.pc_transcripts.fa.gz
   (3) Long non-coding RNAs: gencode.v19.lncRNA_transcripts.fa.gz
   (4) Gene annotation: gencode.v19.annotation.gtf.gz
```
Unzip these 4 reference files, and edit the config file "NCLscan.config" with their paths.

### 2.4.  Building the C++ code

NCLscan involved with some C++ programs, the users can build these utils with the following commands:

        > cd NCLscan/bin
        > make

### 2.5.  Building the reference index

The users can use the script "create_reference.py" to build the needed reference and indices with the following command:

        > cd NCLscan
        > ./bin/create_reference.py -c NCLscan.config

    
## 3. Execution of NCLscan

   Usage:

        > ./NCLscan.py -c [Config_file] -pj [Project_name] -o [Output_dir] --fq1 [Fastq_1] --fq2 [Fastq_2]

   An example:

        > ./NCLscan.py -c ./NCLscan.config -pj test_NCLscan -o output --fq1 simu_5X_100PE_1.fastq --fq2 simu_5X_100PE_1.fastq

The test RNA-seq dataset “simu\_5X\_100PE” can be downloaded from our FTP site: ftp://treeslab1.genomics.sinica.edu.tw/NCLscan/test.tar.gz

Note. The input fastq files may be compressed (.fastq.gz) or uncompressed (.fastq).


## 4. NCLscan outputs

Two output files are generated after executing the pipeline: "MyProject.result" (tab-delimited text file) and “MyProject.result.sam” (SAM-format file). The former is the list of the identified NCL events, and the latter records the supporting reads of the identified NCL events. For the SAM format, users can see http://samtools.github.io/hts-specs/SAMv1.pdf for further details.

The column format of "MyProject.result" is described as follows:
```
(1) Chromosome name of the donor side (5'ss) 
(2) Junction coordinate of the donor side
(3) Strand of the donor side
(4) Chromosome name of the acceptor side (3'ss) 
(5) Junction coordinate of the acceptor side
(6) Strand of the acceptor side
(7) Gene name of the donor side
(8) Gene name of the acceptor side
(9) Intragenic (1) or intergenic (0) case
(10) Total number of all supporting reads
(11) Total number of junc-reads
(12) Total number of span-reads
```


## 5. The detailed description of the NCLscan pipeline
   
The NCLscan pipeline includes four steps, which are all involved in the python script (NCLscan.py). The four steps are explained as follows:

- **NCL_Scan1**

NCL_Scan1 aligns the reads against the reference genome and the annotated transcripts using BWA, and then aligns the BWA-unmapped reads against the same references using Novoalign.
	

- **NCL_Scan2**
   
NCL_Scan2 includes three steps: (1) concatenating the two ends of each unmapped read; (2) BLAT-aligning the concatenated sequences against the reference genome; and (3) removing the concatenated sequences with an alternative co-linear explanation. If a read contains a base with sequence quality score that is smaller than the cutoff value (default: 20), such a read is not considered.


- **NCL_Scan3**

NCL_Scan3 generates putative NCL references with putative NCL junction sites for the retained concatenated-sequences.


- **NCL_Scan4**

Note: NCL_Scan4 includes the following steps: 
```
1. Aligning unmapped reads against all the putative NCL references using Novoalign.
2. Retaining the putative NCL references that satisfy all of the three rules: (1) all mapped reads must not have any alternative co-linear explanation; (2) at least one read supports the junction site; and (3) the collection of the supported reads must span the NCL junction boundary by the setting size of span range (default: 50 bp) on both sides of the junction site.
3. Aligning the retrieved putative NCL references against the reference genome with a different set of BLAT parameters.
4. Removing candidates with an alternative co-linear explanation within a single gene or between-in two close genes.
```

 

