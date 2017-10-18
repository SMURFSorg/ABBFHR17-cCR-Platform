import pandas as pd
import numpy as np
t = pd.read_csv("prosp.csv")
v = t.groupby(['runtype', 'MTBF'])
r = v.describe(percentiles=[.1,.25,.50,.75,.9])
r.to_csv('prosp.dat',header=False,sep="\t")
