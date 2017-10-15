#!env python

import pandas as pd
import numpy as np

t = pd.read_csv("1h-ckpt-celio.csv", names=["Bandwidth", "MTBF", "Interference", "WORK", "IO", "CKPT", "WASTED", "TOTAL", "seed", "Convergence"])
v = t.groupby(["Interference","seed"]).mean()
v.reset_index(inplace=True)
v['TOTWASTE']=v['CKPT']+v['WASTED']
v['TOTWORK']=v['WORK']+v['IO']
baseline = 1.0/v[v.Interference == "baseline"]['TOTWORK'].mean()
v=v[v.Interference != "baseline"]
v['Computing Ratio']=v['WORK']*baseline
v['IO Ratio']=v['IO']*baseline
v['Checkpoint Ratio']=v['CKPT']*baseline
v['Wasted Computing Ratio']=v['WASTED']*baseline
v['Waste Ratio']=v['TOTWASTE']*baseline
v['Work Ratio']=v['TOTWORK']*baseline
v.reset_index(inplace=True)

p = v.groupby(['Interference', 'Bandwidth', 'MTBF'])['Computing Ratio', 'IO Ratio', 'Checkpoint Ratio', 'Wasted Computing Ratio', 'Waste Ratio', 'Work Ratio']
r = p.describe(percentiles=[.1,.25,.50,.75,.9])
r.to_csv('1h-ckpt-celio.dat',header=False,sep="\t")

t = pd.read_csv("daly-ckpt-celio.csv", names=["Bandwidth", "MTBF", "Interference", "WORK", "IO", "CKPT", "WASTED", "TOTAL", "seed", "Convergence"])
v = t.groupby(["Interference","seed"]).mean()
v.reset_index(inplace=True)
v['TOTWASTE']=v['CKPT']+v['WASTED']
v['TOTWORK']=v['WORK']+v['IO']
baseline = 1.0/v[v.Interference == "baseline"]['TOTWORK'].mean()
v=v[v.Interference != "baseline"]
v['Computing Ratio']=v['WORK']*baseline
v['IO Ratio']=v['IO']*baseline
v['Checkpoint Ratio']=v['CKPT']*baseline
v['Wasted Computing Ratio']=v['WASTED']*baseline
v['Waste Ratio']=v['TOTWASTE']*baseline
v['Work Ratio']=v['TOTWORK']*baseline
v.reset_index(inplace=True)

p = v.groupby(['Interference', 'Bandwidth', 'MTBF'])['Computing Ratio', 'IO Ratio', 'Checkpoint Ratio', 'Wasted Computing Ratio', 'Waste Ratio', 'Work Ratio']
r = p.describe(percentiles=[.1,.25,.50,.75,.9])
r.to_csv('daly-ckpt-celio.dat',header=False,sep="\t")

