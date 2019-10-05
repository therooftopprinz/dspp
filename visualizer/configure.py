#!/usr/bin/python
import os
import sys
import subprocess

CXX      = 'g++'
AR       = 'ar'
MAKE     = 'make'
CXXFLAGS = '-std=c++17 -O0 -ggdb3 -Wall -Werror'

TLD = os.path.dirname(sys.argv[0])+'/'
PWD = os.getcwd()+'/'

class Build:
    def __init__(self):
        self.input_files = []
        self.output_file = ''
        self.src_dir = ''
        self.dependencies = []
        self.external_dependencies = []
        self.target_type = 0
        self.cxxflags = ''
        self.linkflags = ''
    def add_include_paths(self, p):
        print "add_include_paths: ", p
        self.cxxflags = self.cxxflags + ' ' +' '.join(['-I'+TLD+i for i in p])
    def set_cxxflags(self, f):
        self.cxxflags = f
    def set_linkflags(self, f):
        self.linkflags = f
    def set_src_dir(self, d):
        self.src_dir = TLD+d
    def add_src_files(self, f):
        self.name = f
        self.input_files.extend(f)
    def target_executable(self, f):
        self.name = f +"_build"
        self.output_file = f
        self.target_type = 0
    def target_archive(self, f):
        self.name = f +"_build"
        self.output_file = f
        self.target_type = 1
    def add_dependencies(self, d):
        self.dependencies.extend(d)
    def add_external_dependencies(self, d):
        self.external_dependencies.extend(d)
    def generate_make(self):
        output = ''
        objects = [self.name+'/'+i+'.o' for i in self.input_files]
        deps    = [self.name+'/'+i+'.d' for i in self.input_files]
        srcs    = [self.src_dir+i for i in self.input_files]
        print objects
        print deps
        print srcs
        output = output + '-include '+' '.join(deps) + '\n'
        output = output + self.output_file + ':' + ' '.join(self.dependencies)+' '+' '.join(objects) + '\n'
        # target rule
        if (self.target_type == 0):
            output = output + '\t'+ CXX + ' ' + ' '.join(objects) + ' ' + ' '.join(self.dependencies) + ' ' + ' '.join([TLD+i for i in self.external_dependencies]) + ' ' + self.linkflags +  ' -o ' + self.output_file + '\n'
        else:
            output = output + '\t'+ AR + ' rcs ' + self.linkflags + ' ' + self.output_file + ' ' + ' '.join(objects) + '\n'

        # object rule
        for i in range(len(self.input_files)):
            output = output + objects[i] + ':' + srcs[i] + '\n'
            output = output +'\t@mkdir -p ' + os.path.dirname(objects[i]) + '\n'
            output = output +'\t@echo Building '+objects[i]+'..\n'
            output = output +'\t@'+ CXX + ' -MMD ' + self.cxxflags + ' -c ' + srcs[i] + ' -o ' + objects[i] + '\n'

        return output;

def clean_filenames(a):
    return [i.strip().replace('./','') for i in a]
def combine_list(list, list2):
    list.extend(list2)
    return list

print 'configuring for testing'

print 'TLD is ' + TLD
print 'PWD is ' + PWD

TEST_SOURCES = []
SRC_SOURCES  = []

p = subprocess.Popen('cd '+TLD+'test        && find .             | egrep \'\.cpp$\'', shell=True, stdout=subprocess.PIPE)
q = subprocess.Popen('cd '+TLD+'src         && find .             | egrep \'\.cpp$\' | grep -v main.cpp', shell=True, stdout=subprocess.PIPE)

TEST_SOURCES = clean_filenames(p.stdout.readlines())
SRC_SOURCES  = clean_filenames(q.stdout.readlines())

print "TEST_SOURCES", TEST_SOURCES
print "SRC_SOURCES", SRC_SOURCES

gtest = Build()
gtest.set_cxxflags(CXXFLAGS)
gtest.set_src_dir('gtest/')
gtest.add_src_files(['gmock-gtest-all.cc'])
gtest.add_include_paths(['gtest'])
gtest.target_archive('gtest.a')

COMMON_TARGET_INLCUDES = ['src/', 'BFC/include', 'BFC/src']

src = Build()
src.set_cxxflags(CXXFLAGS)
src.add_include_paths(COMMON_TARGET_INLCUDES)
src.set_src_dir('src/')
src.add_src_files(SRC_SOURCES)
src.target_archive('src.a')

test = Build()
test.set_cxxflags(CXXFLAGS)
test.add_include_paths(combine_list(COMMON_TARGET_INLCUDES,['gtest/', 'test/']))
test.set_src_dir('test/')
test.add_src_files(TEST_SOURCES)
test.add_dependencies(['gtest.a', 'src.a'])
# test.add_external_dependencies(['Logless/build/logless.a'])
test.set_linkflags("-lpthread")
test.target_executable('test')

visualizer = Build()
visualizer.set_cxxflags(CXXFLAGS)
visualizer.add_include_paths(COMMON_TARGET_INLCUDES)
visualizer.set_src_dir('src/')
visualizer.add_src_files(["main.cpp"])
visualizer.add_dependencies(['src.a'])
# visualizer.add_external_dependencies(['Logless/build/logless.a',])
visualizer.set_linkflags("-lpthread -lglfw -lGL")
visualizer.target_executable('visualizer')

with open('Makefile','w+') as mf:
    mf.write(gtest.generate_make())
    mf.write(src.generate_make())
    mf.write(test.generate_make())
    mf.write(visualizer.generate_make())