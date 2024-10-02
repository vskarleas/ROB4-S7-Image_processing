#############################################################################
# file_input Makefile 
# Project in C - Polytech Sorbonne - 2023/2024 - S6
# Author: Vasileios Filippos Skarleas - All rights reserved.
############################################################################# 
# utilisation des variables internes $@ $< $^ $*
# $@ : correspond au nom de la cible
# $< : correspond au nom de la premiere dependance
# $^ : correspond � toutes les d�pendances
# $* : correspond au nom du file_input sans extension 
#       (dans les regles generiques uniquement)
#############################################################################
# information sur la regle executee avec la commande @echo
# (une commande commancant par @ n'est pas affichee a l'ecran)
#############################################################################

# Chaque exercise ies identifie par un programme principale nomé comme exercise_xxx.c 
# Les programmes peuvent etre base sur d'autres fonctionctions definies aux different file_inputs

#############################################################################
# definition des variables locales
#############################################################################

#############################################################################
### ATTENTION: You ned to change CC comiler to g++ for C++ programs while for C programs you can use CC = clang configured for this machine (universally gcc)
#############################################################################

# C compilers
CC = gcc
CCplus = g++

# chemin d'acces aux librairies (interfaces)
DIR = .

# chemin d'acces aux librairies (binaires)
LIBS = .

# options pour l'�dition des liens
OPTIONS = -L$(LIBS) -lm

# options pour la recherche des file_inputs .o et .h
OPTS = -I$(DIR)

# options de compilation
COMPILOPTS = -g -Wall $(OPTS)

LIBSplus = `pkg-config --cflags --libs opencv4`

GTKplus = `pkg-config --libs --cflags gtk+-3.0`

# liste des executables
EXECUTABLES = main

#############################################################################
# definition des regles
#############################################################################

########################################################
# By default, executables creation
all : $(EXECUTABLES)

########################################################
# General rule (as a reminder)
%.o : %.c %.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module "$*
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

########################################################
# Compilation rules for the different modules

########################################################
# Executables creation in C++

main : main.cpp
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CCplus) -std=c++17 -g main.cpp -o main $(LIBSplus)

#Cleaning repository rule
clean:
	rm -fR $(EXECUTABLES) *.o 
