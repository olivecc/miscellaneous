# COMP0007 Directed Reading

Essays written for the COMP0007 'Directed Reading' module at UCL. These represent my first foray into formal typesetting, and as a result are a little rough around the edges (although the hard limit of 4 pages per summary didn't help either).  
The essays are comprised of summaries of the following notable papers from theoretical computer science (listed in the order in which I wrote their respective summaries):  

* *Gaussian Elimination is not Optimal*, V. Strassen
* *An Axiomatic Basis for Computer Programming*, C. A. R. Hoare
* *Proving Properties of Programs by Structural Induction*, R. M. Burstall
* *Time, Clocks, and the Ordering of Events in a Distributed System*, L. B. Lamport
* *New Directions in Cryptography*, B. W. Diffie and M. E. Hellman

The essays are typeset in LaTeX/BibTeX. One can generate .pdf files with the `pdflatex` and `bibtex` packages with the following:

~~~~
pdflatex paper  
bibtex paper   
pdflatex paper    
pdflatex paper  
~~~~

Where *paper* is the name of the filename of the .tex/.bib files, excluding their file extensions (e.g. *strassen1969gaussian_report").
