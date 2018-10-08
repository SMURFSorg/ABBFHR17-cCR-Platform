#!env python

import pandas as pd
import numpy as np
from scipy import stats

t = pd.read_csv("cielo-bb.csv")
t['totwaste']=t['ckpt']+t['wasted']
t['totwork']=t['work']+t['io']
w = pd.DataFrame()
x = pd.DataFrame()

for intvl in t['ckpt_intvl'].unique():
    print "Doing " + str(intvl)    
    a = t[ t.ckpt_intvl == intvl ]
    for bw in t['sfs_bw'].unique():
        b = a[ a.sfs_bw == bw ]
        for bb_bw in t['bb_bw'].unique():
            c = b[ b.bb_bw == bb_bw ]
            for mtbf in t['sys_mtbf'].unique():
                v = c[ c.sys_mtbf == mtbf ]
                baseline = 1.0/v[v.method == "baseline"]['totwork'].max()
                v=v[v.method != "baseline"]
                v['Computing Ratio']=v['work']*baseline
                v['IO Ratio']=v['io']*baseline
                v['Checkpoint Ratio']=v['ckpt']*baseline
                v['Wasted Computing Ratio']=v['wasted']*baseline
                v['Waste Ratio']=v['totwaste']*baseline
                v['Work Ratio']=v['totwork']*baseline
                w = pd.concat([w, v])
                print len(w)

                no_bb = v[ v.method=="No_bb" ]
                simple_bb = v[ v.method=="Simple_bb" ]
                if( len(no_bb.index) < 3 or len(simple_bb.index) < 3 ):
                    print("Not enough data with " + str(intvl) + ", System BW of " + str(bw) + ", Burst Buffer BW of " + str(bb_bw) + ", and System MTBF of " +str(mtbf) + " to generate statistics");
                d0 = simple_bb['Waste Ratio'].quantile(q=.1) - no_bb['Waste Ratio'].quantile(q=.1)
                d1 = simple_bb['Waste Ratio'].mean() - no_bb['Waste Ratio'].mean()
                d2 = simple_bb['Waste Ratio'].quantile(q=.9) - no_bb['Waste Ratio'].quantile(q=.9)
                x = x.append( { 'ckpt_intvl': intvl, 'sfs_bw': bw, 'bb_bw': bb_bw, 'sys_mtbf': mtbf, 'low_bound': d0, 'avg': d1, 'high_bound': d2}, ignore_index=True )
                
w.reset_index(inplace=True)
x.reset_index(inplace=True)

p = w.groupby(['method', 'ckpt_intvl', 'sfs_bw', 'bb_bw', 'sys_mtbf'])['Computing Ratio', 'IO Ratio', 'Checkpoint Ratio', 'Wasted Computing Ratio', 'Waste Ratio', 'Work Ratio']
r = p.describe(percentiles=[.1,.25,.50,.75,.9])
r.to_csv('cielo-bb.dat',sep="\t")
x.to_csv('cielo-bb-diff.dat',sep="\t",columns=['ckpt_intvl', 'sfs_bw', 'bb_bw', 'sys_mtbf', 'low_bound', 'avg', 'high_bound'])
