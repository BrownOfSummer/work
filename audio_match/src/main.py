#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import print_function
import matplotlib.pyplot as plt
import subprocess
import os

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
    for each in Similarity:
        _,_,_,_,_,_,_,_, score, index_hit = each.split(';')
        score = float( score.split('=')[-1] )
        hits.append(index_hit)
        Score.append(score)
    return Score, hits


def PlotData1(texts1, texts2):
    num = min( len(texts1), len(texts2) )
    color = ['r', 'g', 'b', 'c', 'm', 'y', 'k', 'w']
    plt.figure('Score',figsize=(12,8))
    plt.subplot(221)
    plt.title("Dissimilarity",fontsize=12,fontweight='bold')    #
    plt.grid(linestyle = ":")      #
    #plt.xlabel("Audio Segment Index",fontsize=13,fontweight='bold')
    plt.ylabel("Score",fontsize=13,fontweight='bold')
    
    plt.subplot(223)
    plt.title("Similarity",fontsize=12,fontweight='bold')    #
    plt.grid(linestyle = ":")
    plt.xlabel("Audio Segment Index",fontsize=13,fontweight='bold')
    plt.ylabel("Score",fontsize=13,fontweight='bold')

    plt.subplot(222)
    plt.title("Sort by Scores", fontsize=12, fontweight='bold')
    plt.grid(linestyle = ':')
    plt.ylabel("Score",fontsize=13,fontweight='bold')

    plt.subplot(224)
    #plt.title("Similarity",fontsize=12,fontweight='bold')    #
    plt.grid(linestyle = ":")
    plt.xlabel("Audio Segment Index",fontsize=13,fontweight='bold')
    plt.ylabel("Score",fontsize=13,fontweight='bold')
    #plt.xlim(3,21)         #设置x轴的范围
    #plt.ylim(0,100)
    for i in range(num):
        score1, hits1 = decodeDissimilarity(texts1[i]) # Dissimilarity
        score2, hits2 = decodeSimilarity(texts2[i]) # Similarity
        length = min( len(score1), len(score2) )
        score1 = score1[:length]
        score2 = score2[:length]
        seg_order = range( length )
        color_line = color[i] + '-'
        label=str(time_len[i]) + 's'
        plt.subplot(221)
        plt.plot(seg_order, score1, color_line, linewidth=0.8, markersize=0.5, label=label)
        plt.subplot(223)
        plt.plot(seg_order, score2, color_line, linewidth=0.8, markersize=0.5, label=label)
        
        score1.sort()
        score2.sort()
        plt.subplot(222)
        plt.plot(seg_order, score1, color_line, linewidth=0.8, markersize=0.5, label=label)
        plt.subplot(224)
        plt.plot(seg_order, score2, color_line, linewidth=0.8, markersize=0.5, label=label)

    plt.subplot(221)
    plt.legend(loc='upper right', frameon=False)
    plt.subplot(223)
    plt.legend(loc='upper right', frameon=False)
    plt.subplot(222)
    plt.legend(loc='upper left', frameon=False)
    plt.subplot(224)
    plt.legend(loc='upper left', frameon=False)
    plt.savefig('Score.jpg',format='jpg')
    plt.show()

def PlotData2(texts1, texts2):
    num = min( len(texts1), len(texts2) )
    color = ['r', 'g', 'b', 'c', 'm', 'y', 'k', 'w']
    plt.figure('Hits', figsize=(12,8))
    plt.subplot(221)
    plt.title("Dissimilarity",fontsize=12,fontweight='bold')    #
    plt.grid(linestyle = ":")      #
    #plt.xlabel("Audio Segment Index",fontsize=13,fontweight='bold')
    plt.ylabel("HASH Hits",fontsize=13,fontweight='bold')
    
    plt.subplot(223)
    plt.title("Similarity",fontsize=12,fontweight='bold')    #
    plt.grid(linestyle = ":")
    plt.xlabel("Audio Segment Index",fontsize=13,fontweight='bold')
    plt.ylabel("HASH Hits",fontsize=13,fontweight='bold')

    plt.subplot(222)
    plt.title("Sort by hits",fontsize=12,fontweight='bold')    #
    plt.grid(linestyle = ":")
    plt.ylabel("HASH Hits",fontsize=13,fontweight='bold')

    plt.subplot(224)
    plt.grid(linestyle = ":")
    plt.xlabel("Audio Segment Index",fontsize=13,fontweight='bold')
    plt.ylabel("HASH Hits",fontsize=13,fontweight='bold')
    #plt.xlim(3,21)         #设置x轴的范围
    #plt.ylim(0, 100)
    for i in range(num):
        score1, hits1 = decodeDissimilarity(texts1[i]) # Dissimilarity
        score2, hits2 = decodeSimilarity(texts2[i]) # Similarity
        length = min( len(hits1), len(hits2) )
        hits1 = hits1[:length]
        hits2 = hits2[:length]
        seg_order = range( length )
        color_line = color[i] + '-'
        label=str(time_len[i]) + 's'
        plt.subplot(221)
        plt.plot(seg_order, hits1, color_line, linewidth=0.8, markersize=0.5, label=label)
        plt.subplot(223)
        plt.plot(seg_order, hits2, color_line, linewidth=0.8, markersize=0.5, label=label)
        
        hits1.sort()
        hits2.sort()
        plt.subplot(222)
        plt.plot(seg_order, hits1, color_line, linewidth=0.8, markersize=0.5, label=label)
        plt.subplot(224)
        plt.plot(seg_order, hits2, color_line, linewidth=0.8, markersize=0.5, label=label)

    plt.subplot(221)
    plt.legend(loc='upper right', frameon=False)
    plt.subplot(223)
    plt.legend(loc='upper right', frameon=False)
    plt.subplot(222)
    plt.legend(loc='upper left', frameon=False)
    plt.subplot(224)
    plt.legend(loc='upper left', frameon=False)

    plt.savefig('HashHits.jpg',format='jpg')
    plt.show()

def PrepareData(time_len, outDir):
    #time_len = [5, 10, 15]
    for i in range(len(time_len)):
        Dissimilarity_out = 'Dissimilarity_'+str(time_len[i]) + '.txt'
        Dissimilarity_out = os.path.join(outDir, Dissimilarity_out)
        Similarity_out = 'Similarity_'+str(time_len[i]) + '.txt'
        Similarity_out = os.path.join(outDir, Similarity_out)
        if ( not os.path.exists(Dissimilarity_out) ) or ( not os.path.exists(Similarity_out) ):
            command_str1 = "./dissimilar {} {} {}".format("../data/00-1513781798.adna", "../data/tmp42.adna", time_len[i])
            command_str2 = "./similar {} {} {}".format("../data/00-1513781798.adna", "../data/00-1513781798-copy.adna", time_len[i])
            run_sys_command(command_str1)
            run_sys_command(command_str2)
            command_str3 = "mv {} {}".format('Dissimilarity.txt', Dissimilarity_out)
            command_str4 = "mv {} {}".format('Similarity.txt', Similarity_out)
            run_sys_command(command_str3)
            run_sys_command(command_str4)



if __name__ == '__main__':
    outDir = 'MatchOutTxt'
    if not os.path.exists(outDir):
        os.mkdir(outDir)
    #time_len=[5, 10, 15]
    time_len=[10, 20, 30]
    #time_len=[5,10,15,20,30]
    #time_len=[10,20,30,40]
    #time_len=[5,10,15,20,30,40]
    PrepareData(time_len, outDir)
    texts1 = []
    texts2 = []
    for i in range( len(time_len) ):
        Dissimilarity_out = 'Dissimilarity_'+str(time_len[i]) + '.txt'
        Dissimilarity_out = os.path.join(outDir, Dissimilarity_out)
        texts1.append(Dissimilarity_out)
        Similarity_out = 'Similarity_'+str(time_len[i]) + '.txt'
        Similarity_out = os.path.join(outDir, Similarity_out)
        texts2.append(Similarity_out)
    PlotData1(texts1, texts2)
    PlotData2(texts1, texts2)
