#!/bin/bash

#Primeira linha de cada ficheiro
echo "note1.nb" > note1.nb
echo "note2.nb" > note2.nb




#Seguintes linhas de cada ficheiro
echo "$ ls -l" >> note1.nb
echo "$| wc" >> note1.nb


echo "$ man man" >> note2.nb
echo "$| wc" >> note2.nb
