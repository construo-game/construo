#!/bin/sh

# Little script to convert Construo 0.0.x XML files to Construo
# 0.1.0's sexpr format

sed "s/<?xml version=\"1.0\"?>//" | \
sed "s/<spring>/(spring /" | \
sed "s/<\/spring>/)/" | \
sed "s/<first>/(first /" | \
sed "s/<\/first>/)/" | \
sed "s/<second>/(second /" | \
sed "s/<\/second>/)/" | \
sed "s/<construo-scene>/(construo-scene/" | \
sed "s/<\/construo-scene>/)/" | \
sed "s/<cl-vector>//g" | \
sed "s/<\/cl-vector>//g" | \
sed "s/<spring-list>/(springs /" | \
sed "s/<\/spring-list>/)/" | \
sed "s/<[xyz]>/ /g"  | \
sed "s/<\/[xyz]>/ /g" | \
sed "s/<position>/(pos /g"  | \
sed "s/<\/position>/) /g"  | \
sed "s/<velocity>/(velocity /g"  | \
sed "s/<\/velocity>/) /g"  | \
sed "s/<particle-list>/(particles /" | \
sed "s/<\/particle-list>/)/" | \
sed "s/<\/particle>/)/" | \
sed "s/<particle id=\"\([0-9]*\)\">/(particle (id \1) /"

# EOF #