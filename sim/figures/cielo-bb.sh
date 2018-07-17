#!/bin/sh

echo "nodes,sfs_bw,bb_bw,sys_mtbf,ckpt_intvl,method,work,io,ckpt,wasted,total,converged" > cielo-bb.csv

awk '$2=="System:" {nodes=$5; bw=$11; bb_bw=$16; mtbf=$25; if($29~/[0-9]+/) intvl=sprintf("%dh", $29/3600); else intvl=$29; } $1=="baseline" { printf("%d,%g,%g,%d,%s,baseline,%g,%g,%g,%g,%g,%d\n", nodes, bw, bb_bw, mtbf, intvl, $5, $6, $7, $8, $9,$13); } $1=="Simple" && $4!="Burst" { printf("%d,%g,%g,%d,%s,Simple,%g,%g,%g,%g,%g,%d\n", nodes, bw, bb_bw, mtbf, intvl, $5, $6, $7, $8, $9,$13); } $1=="No" && $4=="Burst" { printf("%d,%g,%g,%d,%s,No_bb,%g,%g,%g,%g,%g,%d\n", nodes, bw, bb_bw, mtbf, intvl, $8, $9, $10, $11, $12,$16); } $1=="Simple" && $4=="Burst" { printf("%d,%g,%g,%d,%s,Simple_bb,%g,%g,%g,%g,%g,%d\n", nodes, bw, bb_bw, mtbf, intvl, $8, $9, $10, $11, $12, $16); }' cielo-bb.log >> cielo-bb.csv
