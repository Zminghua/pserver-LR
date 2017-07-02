#-*- coding: UTF-8 -*-
#################################################################
#    > File: fabfile.py
#    > Author: zhangminghua
#    > Mail: zhangmh1993@163.com
#    > Time: 2017-07-01 02:18:39 PM
#################################################################

import os
import sys
from fabric.api import local,lcd,cd,env,roles,run,execute,put


Main_Path = '~/github/pserver/'
project = 'project.tar.gz'
env.roledefs = {'server0': ['user@*.*.*.*:22',],
                'server1': ['user@localhost:22',]}


@roles('server0')
def task0():
    run('rm -rf %s' % Main_Path)
    run('mkdir -p %s' % Main_Path)
    put(Main_Path+project, Main_Path)
    with cd(Main_Path):
        run('tar -xzvf %s' % project)
        run('rm %s' % project)

    with cd(Main_Path+'ps/run/'):
        run('sh -x ./run0.sh && sleep 1')


def task1():
    with cd(Main_Path):
        local('rm %s' % Main_Path+project)

    with lcd(Main_Path+'ps/run/'):
        local('sh -x ./run1.sh && sleep 1')


def prepare():
    local('make clean')
    local('make')
    local('rm *.o')
    with lcd(Main_Path):
        local('tar -czvf %s ps/ ps-lite/' % project)


def deploy():
    prepare()
    execute(task0)
    task1()


