#!/bin/bash

# 将.h文件修改为.cc文件
for file in *.h; do
	mv "$file" "${file%.h}.cc"
done
