#!/usr/bin/python
# -*- coding:utf-8 -*-
from __future__ import print_function
import matplotlib.pyplot as plt

def decodeData(filepath):
    with open(filepath) as f:
        data = f.readlines()
    data = [int(each) for each in data]
    return data

def PlotData(data):
    plt.figure('Table Count', figsize=(20,8))
    plt.title("Table Count")
    plt.grid(linestyle=':')
    plt.xlabel("HASH slots")
    plt.ylabel("Count")
    plt.plot(data, 'b-', linewidth=0.8)
    plt.savefig('table_cnt.jpg', format='jpg')
    plt.show()

if __name__ == '__main__':
    data = decodeData('./table_cnt.txt')
    PlotData(data)
