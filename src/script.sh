#!/bin/bash
echo "1 para gerar automaticamente, 2 para configurar notebook"

read n
#construir ficheiros automaticamente (definidos aqui)
if [[ $n == 1 ]]; then
	#Primeira linha de cada ficheiro
	echo "note1.nb" > note1.nb
	echo "note2.nb" > note2.nb


	#Seguintes linhas de cada ficheiro
	echo "$ ls -l" >> note1.nb
	echo "$| wc" >> note1.nb


	echo "$ man man" >> note2.nb
	echo "$| wc" >> note2.nb
fi

#construir ficheiro linha após linha
if [[ $n == 2 ]]; then
	echo "nome do ficheiro?"
	read nome
	echo $nome > $nome
	echo "inserir linha a linha"
	while [[ true ]]; do
		read line
		echo $line >> $nome
	done
fi
