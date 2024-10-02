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
EXECUTABLES = test_image apply_geometry contours_simple test_postscript mask segments_simplification test_approx bezier2_simplification bezier3_simplification test_approx3 contours_multiple image_processing image_capture ui bezier2_simplification_manual bezier3_simplification_manual segments_simplification_manual cartoon cartoon_manual image_processing_manual previewer

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

image.o : image.c image.h error.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module image"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

test_image.o : test_image.c image.h error.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module test_image"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

geometry.o : geometry.c geometry.h contour.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du geom2d"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

apply_geometry.o : apply_geometry.c geometry.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module apply_geometry"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

contour.o : contour.c contour.h image.h geometry.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module contour"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

sequence_point.o : sequence_point.c sequence_point.h geometry.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module sequence_point"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

contours_simple.o : contours_simple.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module contours_simple"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

test_postscript.o : test_postscript.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module test_postscript"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

mask.o : mask.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module mask"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

contours_multiple.o : contours_multiple.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module contours_multiple"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

test_approx.o : test_approx.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module test_approx"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

test_approx3.o : test_approx3.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module test_approx3"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

bezier2_simplification.o : bezier2_simplification.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module bezier2_simplification"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

bezier2_simplification_manual.o : bezier2_simplification_manual.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module bezier2_simplification_manual"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

bezier3_simplification.o : bezier3_simplification.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module bezier3_simplification"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

bezier3_simplification_manual.o : bezier3_simplification_manual.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module bezier3_simplification_manual"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

segments_simplification.o : segments_simplification.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module segments_simplification"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

previewer.o : previewer.c
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module segments_simplification"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<

segments_simplification_manual.o : segments_simplification_manual.c contour.h image.h
	@echo ""
	@echo "---------------------------------------------"
	@echo "Compilation du module segments_simplification_manual"
	@echo "---------------------------------------------"
	$(CC) -c $(COMPILOPTS) $<
		
########################################################
# Executables creation in C

test_image : test_image.o image.o 
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

apply_geometry : apply_geometry.o geometry.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

contours_simple : contours_simple.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

test_postscript : test_postscript.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

mask : mask.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

contours_multiple : contours_multiple.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

test_approx : test_approx.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

previewer : previewer.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

test_approx3 : test_approx3.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@


bezier2_simplification : bezier2_simplification.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

bezier2_simplification_manual : bezier2_simplification_manual.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

bezier3_simplification : bezier3_simplification.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

bezier3_simplification_manual : bezier3_simplification_manual.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

segments_simplification : segments_simplification.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

segments_simplification_manual : segments_simplification_manual.o contour.o image.o geometry.o sequence_point.o
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CC) $^ $(OPTIONS) -o $@

########################################################
# Executables creation in C++

image_processing : image_processing.cpp
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CCplus) -std=c++17 -g image_processing.cpp -o image_processing $(LIBSplus)

image_processing_manual : image_processing_manual.cpp
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CCplus) -std=c++17 -g image_processing_manual.cpp -o image_processing_manual $(LIBSplus)

image_capture : image_capture.cpp
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CCplus) -std=c++17 -g image_capture.cpp -o image_capture $(LIBSplus)

ui : ui.cpp
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CCplus) -g -Wall -std=c++17 -g ui.cpp -o ui $(GTKplus)

cartoon : cartoon.cpp
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CCplus) -std=c++17 -g cartoon.cpp -o cartoon $(LIBSplus)

cartoon_manual : cartoon_manual.cpp
	@echo ""
	@echo "---------------------------------------------"
	@echo "Creation de l'executable "$@
	@echo "---------------------------------------------"
	$(CCplus) -std=c++17 -g cartoon_manual.cpp -o cartoon_manual $(LIBSplus)


#Cleaning repository rule
clean:
	rm -fR $(EXECUTABLES) *.o 
