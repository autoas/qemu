#! /usr/bin/python3
'''/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2017  AS <parai@foxmail.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
'''
import os
import sys
import re

ROOT = os.path.abspath('../../../..')
sys.path.append('%s/tools' % (ROOT))
from building import *
from generator import Os
from generator import oil
import json


CWD = os.path.dirname(__file__)
if(CWD == ''):
    CWD = os.path.abspath('.')
CTEST = CWD

reHeader = re.compile(r'^(\w+):(.+)$')
reCase = re.compile(r'^\s+([^:]+)$')
reVar = re.compile(r'^\s+(\w+):(\w+)$')


def parse():
    import glob
    fp = open('%s/cfg/ctestcases.cfg' % (CTEST), 'r')
    cfg = []
    target = ''
    case = ''
    id = -1
    for el in fp.readlines():
        if(reHeader.search(el)):
            target, desc = reHeader.search(el).groups()
            cfg.append({'desc': desc, 'target': target, 'case': {}})
            id += 1
        elif(reCase.search(el)):
            case = reCase.search(el).groups()[0].strip().replace(' ', '-')
            cfg[id]['case'][case] = {}
        elif(reVar.search(el)):
            name, val = reVar.search(el).groups()
            cfg[id]['case'][case][name] = val
        else:
            if(el.strip() != ''):
                print('warning:', el)
    fp.close()
    for cc in glob.glob('./Testsuite/std/*') + glob.glob('./Testsuite/ext/*'):
        for sched in glob.glob('%s/*' % (cc)):
            for obj in glob.glob('%s/*' % (sched)):
                for test in glob.glob('%s/*' % (obj)):
                    if(test[-3:] == 'inc'):
                        continue
                    cfg.append({'desc': desc, 'target': 'test', 'case': {test: {}}})
                    id += 1
    return cfg

def genCTEST_CFGH(cfg, path):
    fp = open('%s/ctest_cfg.h' % (path), 'w')
    fp.write('#ifndef _CTEST_CFG_H_\n#define _CTEST_CFG_H_\n\n')
    fp.write('#define CT_ERROR_CHECKING_TYPE	CT_ERROR_CHECKING_%s\n\n' % (cfg['Status']))
    for tsk in cfg['TaskList']:
        if(tsk['Schedule'] == 'FULL'):
            fp.write('#define CT_SCHEDULING_%s CT_PREEMPTIVE\n\n' % (tsk['Name']))
        else:
            fp.write('#define CT_SCHEDULING_%s CT_NON_PREEMPTIVE\n\n' % (tsk['Name']))
    fp.write('#endif\n\n')
    fp.close()

def genCfg(cfg, path):
    with open('%s/OS.json' % (path), 'w') as f:
        json.dump(cfg, f, indent=2)
    Os.Gen_Os(cfg, path)

def telnet(uri, port):
    import socket
    import time
    time.sleep(0.5)  # make sure qemu already on line
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((uri, port))
    sock.setblocking(0)
    timeLeft = 100  # *100ms
    string = ''
    while(timeLeft > 0):
        time.sleep(0.1)
        timeLeft -= 1
        try:
            string += sock.recv(4096*4096).decode('utf-8')
        except BlockingIOError:
            pass
        if((string.find('FAIL') != -1) or (string.find('>> END <<') != -1)):
            break
    sock.close()
    return string


def RunCommand(cmd):
    print(' >> RunCommand "%s"' % (cmd))
    if(os.name != 'nt'):
        cmd += ' > /dev/null'
    if(0 != os.system(cmd)):
        print('FAIL of RunCommand "%s"' % (cmd))
        exit(-1)
        return -1
    return 0


def check(target, case):
    schedfifo = os.getenv('schedfifo')
    print('>> Starting test for %s %s schedfifo="%s" ...' % (target, case, schedfifo))
    if(0 == RunCommand('qemu-system-arm -machine help')):
        qemu = 'qemu-system-arm'
    else:
        qemu = './qemu-system-arm'
        if(not os.path.exists(qemu)):
            RunCommand(
                'wget https://github.com/idrawone/qemu-rpi/raw/master/tools/qemu-system-arm-rpi_UART1.tar.gz && tar xf qemu-system-arm-rpi_UART1.tar.gz')
    cmd = '%s -m 128 -M versatilepb -nographic -kernel out/%s/%s/%s -serial tcp:127.0.0.1:1103,server' % (
        qemu, target, case, target)

    if(os.name == 'nt'):
        cmd = 'start '+cmd
        RunCommand(cmd)
        pid = 1
    else:
        pid = os.fork()
    if(pid == 0):
        RunCommand('pgrep qemu-system-arm | xargs -i kill -9 {}')
        RunCommand(cmd)
        exit(0)
    else:
        result = telnet('127.0.0.1', 1103)
        if(os.name == 'nt'):
            RunCommand('taskkill /IM qemu-system-arm.exe')
        else:
            os.kill(pid, 9)
            RunCommand('pgrep qemu-system-arm | xargs -i kill -9 {}')
        if((result.find('FAIL') != -1) or (result.find('>> END <<') == -1)):
            print('>> Test for %s %s FAIL' % (target, case))
            print(result)
            erp = '>> Test for %s %s FAIL\n' % (target, case)
            erp += result
            exit(-1)
        print('>> Test for %s %s PASS' % (target, case))


def fixCfg(cfg, vv):
    for k,v in cfg.items():
        if type(v) is list:
            for o in v:
                if type(o) is dict:
                    fixCfg(o, vv)
        elif v in vv.keys():
            cfg[k] = vv[v]

def test(target, case, vv):
    if(os.name == 'nt'):
        os.makedirs('src/%s/%s' % (target, case), exist_ok=True)
    else:
        RunCommand('mkdir -p src/%s/%s' % (target, case))
    if(target == 'test'):
        cfg = oil.parse('%s/%s.oil' % (case, target))
    else:
        cfg = oil.parse('%s/etc/%s.oil' % (CTEST, target))
        fixCfg(cfg, vv)
        genCTEST_CFGH(cfg, 'src/%s/%s' % (target, case))
    genCfg(cfg, 'src/%s/%s' % (target, case))
    RunCommand('make dep-os TARGET=%s CASE=%s' % (target, case))
    RunCommand(cmd='make all TARGET=%s CASE=%s' % (target, case))
    check(target, case)


if(__name__ == '__main__'):
    if(len(sys.argv) == 2 and sys.argv[1] == 'all'):
        cfg = parse()
        for v in cfg:
            target = v['target']
            for case, vv in v['case'].items():
                test(target, case, vv)
    elif(len(sys.argv) == 2):
        cfg = parse()
        searchflag = True
        for v in cfg:
            target = v['target']
            if(searchflag and target != sys.argv[1]):
                continue
            searchflag = False
            for case, vv in v['case'].items():
                test(target, case, vv)
    elif(len(sys.argv) == 3):
        cfg = parse()
        for v in cfg:
            target = v['target']
            if(target != sys.argv[1]):
                continue
            for case, vv in v['case'].items():
                if(case != sys.argv[2]):
                    continue
                test(target, case, vv)
    else:
        print('Usage: %s all' % (sys.argv[0]))
        print('       %s target case\nExample:' % (sys.argv[0]))
        cfg = parse()
        count = 0
        for v in cfg:
            target = v['target']
            for case, vv in v['case'].items():
                print('\t%s %s %s' % (sys.argv[0], target, case))
                count += 1
                if(count > 8):
                    count = 0
                    c = input('More <Enter> Exit <q>')
                    if(c == 'q'):
                        exit(0)
