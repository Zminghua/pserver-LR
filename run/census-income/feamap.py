#-*- coding: UTF-8 -*-
#################################################################
#    > File: feamap.py
#    > Author: zhangminghua
#    > Mail: zhangmh1993@163.com
#    > Time: 2017-07-01 09:15:01 PM
#################################################################

import os
import sys
import re
import codecs
import logging
import numpy

def map0(x):
    if x<16:
        return 0
    else:
        return x/2

def map5(x):
    if x<10:
        return 0
    if x<1800:
        return 10+(x/50)
    if x<2600:
        return 1800+(x/100)
    return 2600+(x/1000)

def map16(x):
    if x<100:
        return 0
    if x<3000:
        return 1
    if x<3130:
        return x
    if x<4680:
        return 2
    if x<4800:
        return 3
    if x<4934:
        return 4
    if x<5000:
        return 5
    if x<5100:
        return 6
    if x<5400:
        return 7
    if x<6000:
        return 8
    if x<6300:
        return 9
    if x<6500:
        return 10
    if x<6700:
        return x
    if x<7200:
        return 11
    if x<15800:
        return x
    if x<20000:
        return 12
    if x<25000:
        return x
    if x<34000:
        return 13
    if x<50000:
        return 14
    else:
        return x

def map17(x):
    if x<100:
        return 0
    if x<300:
        return 1
    if x<400:
        return 2
    if x<650:
        return 3
    if x<750:
        return 4
    if x<1450:
        return 5
    if x<1500:
        return 6
    if x<1550:
        return 7
    if x<1570:
        return 8
    if x<1640:
        return 9
    if x<1650:
        return x
    if x<1750:
        return 10
    if x<1760:
        return 11
    if x<1845:
        return 12
    if x<1850:
        return x
    if x<1880:
        return 13
    if x<1920:
        return x
    if x<1975:
        return 14
    if x<1980:
        return x
    if x<2170:
        return 15
    if x<2205:
        return x
    if x<2230:
        return 16
    if x<2300:
        return x
    if x<2370:
        return 17
    if x<2450:
        return x
    if x<2470:
        return 18
    if x<2480:
        return x
    if x<2540:
        return 19
    if x<2590:
        return 20
    if x<2800:
        return 21
    if x<3100:
        return 22
    if x<3600:
        return 23
    if x<3700:
        return 24
    else:
        return 25

def map18(x):
    if x>=30000:
        return 30000+(x/20000)
    if x>=10000:
        return 30000+(x/1000)
    if x>=3000:
        return 3000+(x/500)
    if x>=300:
        return 300+(x/50)
    if x>=100:
        return 100+(x/5)
    if x>1:
        return 1+x/2
    else:
        return x

def map29(x):
    return x

def map38(x):
    if x<8 and x>0:
        return 1
    else:
        return x

def main():

    ins_data = []
    ins_test = []

    values_data = [ set() for j in xrange(40) ]
    values_test = [ set() for j in xrange(40) ]

    #pos_data = numpy.zeros((40,100000))
    #neg_data = numpy.zeros((40,100000))
    #pos_test = numpy.zeros((40,100000))
    #neg_test = numpy.zeros((40,100000))

    logging.info('Loading data ...')
    path_to_data = os.path.abspath('.') + '/census-income.data'
    with codecs.open(path_to_data, 'r', 'utf-8') as fin:
        lines = fin.readlines()
    for i in xrange(len(lines)):
        fields = re.split(r', ', lines[i].strip())
        del fields[24]
        for k in [0,5,16,17,18,29,38]:
            fields[k] = eval('map%d' % k)(int(fields[k]))
        fields[-1] = -1 if fields[-1] == '- 50000.' else 1
        
        ins_data.append(fields)
        for j in xrange(len(fields)-1):
            values_data[j].add(fields[j])
        
        #for k in [0,5,16,17,18,29,38]:
        #    if fields[-1] == -1:
        #        neg_data[k][fields[k]] += 1
        #    else:
        #        pos_data[k][fields[k]] += 1
    
    offset = [1]
    for j in xrange(len(values_data)):
        values_data[j] = list(values_data[j])
        offset.append( offset[-1]+len(values_data[j]) )

    with codecs.open(path_to_data+'.fea', 'w', 'utf-8') as fout:
        for i in xrange(len(ins_data)):
            feas = [str(ins_data[i][-1])]
            for j in xrange(len(ins_data[i])-1):
                feas.append(str(offset[j]+values_data[j].index(ins_data[i][j]))+':1')
            fout.write(' '.join(feas)+'\n')

    path_to_test = os.path.abspath('.') + '/census-income.test'
    with codecs.open(path_to_test, 'r', 'utf-8') as fin:
        lines = fin.readlines()
    for i in xrange(len(lines)):
        fields = re.split(r', ', lines[i].strip())
        del fields[24]
        for k in [0,5,16,17,18,29,38]:
            fields[k] = eval('map%d' % k)(int(fields[k]))
        fields[-1] = -1 if fields[-1] == '- 50000.' else 1

        ins_test.append(fields)
        for j in xrange(len(fields)-1):
            values_test[j].add(fields[j])
        
        #for k in [0,5,16,17,18,29,38]:
        #    if fields[-1] == -1:
        #        neg_test[k][fields[k]] += 1
        #    else:
        #        pos_test[k][fields[k]] += 1

    with codecs.open(path_to_test+'.fea', 'w', 'utf-8') as fout:
        for i in xrange(len(ins_test)):
            feas = [str(ins_test[i][-1])]
            for j in xrange(len(ins_test[i])-1):
                feas.append(str(offset[j]+values_data[j].index(ins_test[i][j]))+':1')
            fout.write(' '.join(feas)+'\n')

    #k = 0
    #for j in xrange(100000):
    #    if pos_data[k][j]+neg_data[k][j]+pos_test[k][j]+neg_test[k][j]>0:
    #        print j,'\t',pos_data[k][j],neg_data[k][j],pos_test[k][j],neg_test[k][j]

    x = 0
    y = 0
    for j in xrange(40):
        print len(values_data[j]),len(values_test[j]),len(set(values_data[j]) - set(values_test[j])),len(set(values_test[j]) - set(values_data[j]))
        x += len(values_data[j])
        y += len(values_test[j])
    print x
    print y


if __name__ == '__main__':
    if len(sys.argv) != 1:
        print 'usage : %s ! ' % sys.argv[0]
        exit(1)
    else:
        fileHandler = logging.FileHandler(os.path.abspath('.')+'/log.'+sys.argv[0], mode='w', encoding='UTF-8')
        formatter = logging.Formatter('%(asctime)s %(filename)s[%(lineno)d] %(levelname)s %(message)s', '%Y-%m-%d %H:%M:%S')
        fileHandler.setFormatter(formatter)
        logger = logging.getLogger()
        logger.setLevel(logging.INFO)
        logger.addHandler(fileHandler)

        main()


