# Source Files
[@main.c](https://github.com/leorge/qmisort/blob/master/%40main.c) : A test program.
### Conventional QuickSort using swaps
[qsort_first.c](https://github.com/leorge/qmisort/blob/master/qsort_first.c) : A pivot is the first element.  
[qsort_kr.c](https://github.com/leorge/qmisort/blob/master/qsort_kr.c) : Kernighan & Ritchie style QuickSort  
[qsort_middle.c](https://github.com/leorge/qmisort/blob/master/qsort_middle.c) : A pivot is the middle element.  
[qsort_med3.c](https://github.com/leorge/qmisort/blob/master/qsort_med3.c)
: A pivot is the median of three elements (fist, middle, last).  
[qsort_3way.c](https://github.com/leorge/qmisort/blob/master/qsort_3way.c) : [Three way partitioning QuickSort](http://algs4.cs.princeton.edu/23quicksort/Quick3way.java.html).  
[dual_pivot.c](https://github.com/leorge/qmisort/blob/master/dual_pivot.c)
: [Dual Pivot QuickSort](http://grepcode.com/file/repository.grepcode.com/java/root/jdk/openjdk/7-b147/java/util/DualPivotQuicksort.java).  
### New QuickSort
[quick_hole.c](https://github.com/leorge/qmisort/blob/master/quick_hole.c)
: Simplest new QuickSort using a pivot hole. A pivot is the last element.  
[pivot_hole.c](https://github.com/leorge/qmisort/blob/master/pivot_hole.c)
: Debugging statements are removed from quick_hole.c.  
[quick_secure.c](https://github.com/leorge/qmisort/blob/master/quick_secure.c) : Secured quick_hole.c.  
[quick_asymm.c](https://github.com/leorge/qmisort/blob/master/quick_asymm.c) : Simplest Asymmetric QuickSort.  
[quick_random.c](https://github.com/leorge/qmisort/blob/master/quick_random.c) : Random pivoting.  
[quck_pivot.c](https://github.com/leorge/qmisort/blob/master/quick_pivot.c) : Various pivoting.  
[asymm_qsort.c](https://github.com/leorge/qmisort/blob/master/asymm_qsort.c) : Final entire Asymmetric QuickSort.  
### Scripts to generate a data sequence
[random.awk](https://github.com/leorge/qmisort/blob/master/random.awk) : Random data sequence.  
[sorted.awk](https://github.com/leorge/qmisort/blob/master/sorted.awk)
: Sorted data (0,1,2,...,n-2,n-1)  
[reverse.awk](https://github.com/leorge/qmisort/blob/master/reverse.awk)
: Sorted data in reverse order (n-1,n-2,....,2,1,0)  
[middle.pl](https://github.com/leorge/qmisort/blob/master/middle.pl)
: Worst data sequence when a pivot is the middle element in the conventional QuickSort.  
[hole_middle.pl](https://github.com/leorge/qmisort/blob/master/hole_middle.pl)
: Worst data sequence when a pivot is the middle element in the Asymmetric QuickSort.  
[quick_hole.pl](https://github.com/leorge[quick_hole.pl)
: **Best** data sequence for quick_hole().   
[median3.awk](https://github.com/leorge/qmisort/blob/master/median3.awk) : Worst data sequence for the median-of-three.  
[valley.awk](https://github.com/leorge/qmisort/blob/master/valley.awk) : n-1,n-3,...,3,1,0,2,4,...,n-4,n-2
or n-1,n-3,...,4,2,0,1,3,...,n-4,n-2  
[KillQuickX.pl](https://github.com/leorge/qmisort/blob/master/KillQuickX.pl)
: Worst data sequence for pseudomedian of nine in [QuickX.java](http://algs4.cs.princeton.edu/23quicksort/QuickX.java.html).  
[n111.awk](https://github.com/leorge/qmisort/blob/master/n111.awk) : n,1,1,1,...   
[n11n.awk](https://github.com/leorge/qmisort/blob/master/n11n.awk) : n,1,1,1,...,1,1,n   
[n1n1.awk](https://github.com/leorge/qmisort/blob/master/n1n1.awk) : n,1,n,1,...,n,1,n,1  
[n1nn.awk](https://github.com/leorge/qmisort/blob/master/n1nn.awk) : n,1,n,n,n,,...,n  
[nn11.awk](https://github.com/leorge/qmisort/blob/master/nn11.awk) : n,n,...,n,1,1,...,1  
[nn1n.awk](https://github.com/leorge/qmisort/blob/master/nn1n.awk) : n,n,...,n,1,n  
[nnn1.awk](https://github.com/leorge/qmisort/blob/master/nnn1.awk) : n,n,...,n,1  
[nnnn.awk](https://github.com/leorge/qmisort/blob/master/nnn1.awk) : n,n,...,n  
[merge.pl](https://github.com/leorge/qmisort/blob/master/merge.pl) : Worst data sequence for MergeSort.  
### Other scirpts
[col.awk](https://github.com/leorge/qmisort/blob/master/col.awk) : Output a tablist for expand(1).  
[esd.awk](https://github.com/leorge/qmisort/blob/master/esd.awk) : Calculate **E**stimated **S**tandard **D**eviation.  
[et.awk](https://github.com/leorge/qmisort/blob/master/et.awk) : Create a table of elapsed time.  
[exp.awk](https://github.com/leorge/qmisort/blob/master/exp.awk)
: Print an expression "=function(cell1, cell2, ...)" for Excel or Calc.
