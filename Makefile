#
#  FOTOMATON. Detector de rostros de la plataforma SWAD
#
#  Copyright (C) 2008  Daniel J. Calandria Hernández &
#                      Antonio Cañas Vargas
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#


BIN=../bin
SRC=.

CXX = g++
CC=g++
CFLAGS=-Wall -O3 $(shell pkg-config --cflags opencv)
CXXFLAGS = -Wall -O3 $(shell pkg-config --cflags opencv)
LDFLAGS =$(shell pkg-config --libs opencv)
TARGETS=create_feature_set train_cascade fotomaton

all: $(TARGETS)

create_feature_set: create_feature_set.o haar_feature.o io_rutines.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
train_cascade: train_cascade.o haar_feature.o haar_classifier.o io_rutines.o util.o training.o adaboost.o cascade.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
fotomaton: haar_feature.o haar_classifier.o io_rutines.o util.o cascade.o fotomaton.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
clean:
	rm -f *.o

