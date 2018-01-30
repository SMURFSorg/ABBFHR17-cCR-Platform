import pandas as pd
cst_ckpt = pd.read_csv('1h-ckpt-cielo.csv', 
                       names=["Bandwidth", "MTBF", "Interference", "WORK", "IO", "CKPT", "WASTED", "TOTAL", "seed", "Convergence"])
daly_ckpt = pd.read_csv('daly-ckpt-cielo.csv', 
                        names=["Bandwidth", "MTBF", "Interference", "WORK", "IO", "CKPT", "WASTED", "TOTAL", "seed", "Convergence"])

F=pd.DataFrame()
for bw in [1.6e11, 8e10, 4e10]:
    A=cst_ckpt[((cst_ckpt.Bandwidth==bw) & (cst_ckpt.Interference=="Simple"))].groupby('MTBF').mean().reset_index().set_index('MTBF')
    B=cst_ckpt[((cst_ckpt.Bandwidth==bw) & (cst_ckpt.Interference=="No"))].groupby('MTBF').mean().reset_index().set_index('MTBF')
    R=A/B
    V=pd.DataFrame(index=R.index)
    V['nocoop-1h'] = R['CKPT']
    A=daly_ckpt[((daly_ckpt.Bandwidth==bw) & (daly_ckpt.Interference=="Simple"))].groupby('MTBF').mean().reset_index().set_index('MTBF')
    B=daly_ckpt[((daly_ckpt.Bandwidth==bw) & (daly_ckpt.Interference=="No"))].groupby('MTBF').mean().reset_index().set_index('MTBF')
    R=A/B
    V['nocoop-daly'] = R['CKPT']
    A=cst_ckpt[((cst_ckpt.Bandwidth==bw) & (cst_ckpt.Interference=="BLOCKING_FCFS"))].groupby('MTBF').mean().reset_index().set_index('MTBF')
    B=cst_ckpt[((cst_ckpt.Bandwidth==bw) & (cst_ckpt.Interference=="No"))].groupby('MTBF').mean().reset_index().set_index('MTBF')
    R=A/B
    V['bfifo-1h'] = R['CKPT']
    A=daly_ckpt[((daly_ckpt.Bandwidth==bw) & (daly_ckpt.Interference=="BLOCKING_FCFS"))].groupby('MTBF').mean().reset_index().set_index('MTBF')
    B=daly_ckpt[((daly_ckpt.Bandwidth==bw) & (daly_ckpt.Interference=="No"))].groupby('MTBF').mean().reset_index().set_index('MTBF')
    R=A/B
    V['bfifo-daly'] = R['CKPT']
    V['bw'] = bw
    F = pd.concat([V, F])

F.reset_index(inplace=True)
#F['MTBF'] = F['MBTF']*18860/3600/24/365
F['#node MTBF (y)'] = F['MTBF']*18860/3600/24/365
F.to_csv('ckpt-slowdown.dat', sep='\t', columns=['#node MTBF (y)', 'bw', 'nocoop-1h', 'nocoop-daly', 'bfifo-1h', 'bfifo-daly'], index=False)
