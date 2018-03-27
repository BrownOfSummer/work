#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import print_function
import matplotlib.pyplot as plt
import subprocess

def run_sys_command(command_str):
    print(command_str)
    p = subprocess.Popen(command_str, 
            shell=True, 
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE)
    p.wait()
    stdout, stderr = p.communicate()
    if stderr:
        print('Error: {}'.format(stderr))
    else:
        print(stdout)

def decodeDissimilarity(filepath):
    with open(filepath) as f:
        Dissimilarity = f.readlines()
    hits = []
    Score = []
    for each in Dissimilarity:
        seg_range, seg_len, step_len, score, index_hit = each.split(';')
        #print( seg_range, seg_len, step_len, index_hit )
        score = float( score.split('=')[-1] )
        hits.append(index_hit)
        Score.append(score)

    return Score, hits

def decodeSimilarity(filepath):
    with open(filepath) as f:
        Similarity = f.readlines()
    hits = []
    Score = []
    Step_len1 = []
    Step_len2 = []
    for each in Similarity:
        _,_,step_len1,_,_,step_len2,_,_, score, index_hit = each.split(';')
        score = float( score.split('=')[-1] )
        step_len1 = float( step_len1.split('=')[-1] )
        step_len2 = float( step_len2.split('=')[-1] )
        hits.append(index_hit)
        Score.append(score)
        Step_len1.append(step_len1)
        Step_len2.append(step_len2)
    return Score, hits, Step_len1, Step_len2

if __name__ == '__main__':
    _,_, Step_len1, Step_len2 = decodeSimilarity('./Similarity.txt')
    sum1 = sum(Step_len1)
    sum2 = sum(Step_len2)
    print(sum1 - sum2)
    steps = range(len(Step_len2))
    diff = [Step_len2[i] - Step_len1[i] for i in steps]
    plt.plot(steps, Step_len1, 'r-', label='step_len1')
    plt.plot(steps, Step_len2, 'b-', label='step_len2')
    plt.plot(steps, diff, 'g-', label='diff')
    plt.grid(linestyle='--')
    plt.legend(loc='upper right', frameon=False)
    plt.show()

