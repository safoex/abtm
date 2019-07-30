# ABTM
Asynchronous Behavior Tree with Memory

### Requirements: 
`libyaml-cpp` (can be installed via `apt` on Ubuntu)

### This is a cutted initial version!
Soon there would be more updates both on features and code quality

### Understand what happens
Look through `config/test.yaml` for tree description, through `config/input.yaml` for input messages, and find `build/test.pdf` with visualized tree. 

### Generate pdf
To generate pdf file with tree description call `dot -Tpdf test_gv_tree.txt > test.pdf` from `build/` directory
OR
simply call `pdf.sh` which creates both `test.pdf` and `states.pdf` (last one contain states of every node after applying all the samples)
