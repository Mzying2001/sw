# 该脚本用于将SimpleWindow代码整合成单个头文件和源文件

import os
import re
from collections import deque

# 当前脚本文件所在目录
basedir = os.path.dirname(__file__)

# inc文件夹路径
incpath = os.path.join(basedir, '../sw/inc')

# 所有的头文件的文件名
headers = []

# 输出文件
outputfile = open(os.path.join(basedir, 'sw_all.h'), 'w', encoding='utf-8')
outputfile.write('// https://github.com/Mzying2001/sw\n\n')
outputfile.write('#pragma once\n')

# 添加文件到列表
for item in sorted(os.listdir(incpath)):
    if os.path.isfile(os.path.join(incpath, item)) and item.endswith('.h'):
        headers.append(item)

# 头文件个数
count = len(headers)

# 文件内容
contents = [''] * count

# 记录已经include的系统文件
included = set()

# 邻接矩阵
graph = [[False for i in range(count)] for i in range(count)]

# 入度列表
indegree = [0] * count


for index in range(0, count):
    headerpath = os.path.join(incpath, headers[index])

    with open(headerpath, 'r', encoding='utf-8') as file:
        for line in file.readlines():
            line = line.rstrip()

            if line == '#pragma once':  # 忽略
                continue

            # 匹配include系统文件
            match = re.search(r'#include\s*<([^>]+)>', line)
            if match:
                incfile = match.group(1)
                if incfile not in included:
                    included.add(incfile)
                continue

            # 匹配include
            match = re.search(r'#include\s*"([^"]+)"', line)
            if match:
                incfile = match.group(1)
                incfileindex = headers.index(incfile)
                graph[incfileindex][index] = True  # 构建邻接矩阵
                indegree[index] += 1               # 当前文件入度加1，用于拓扑排序
                continue

            # 不是include，记录文件内容
            contents[index] += line + '\n'


# 输出include部分（将Windows.h提前是因为CommCtrl.h依赖Windows.h）
outputfile.write('#include <Windows.h>\n')
for incfile in sorted(included):
    if incfile != 'Windows.h':
        outputfile.write(f'#include <{incfile}>\n')


# 记录入度为0点的队列
q = deque()

# 拓扑排序
while (True):
    # 找出入度为0的点
    for i in range(0, count):
        if indegree[i] == 0:
            q.append(i)
    # 队列为空，循环结束
    if not q:
        break
    # 按照顺序输出文件
    while q:
        i = q.popleft()
        indegree[i] = -1
        for j in range(0, count):
            if graph[i][j]:
                indegree[j] -= 1
        outputfile.write(f'\n// {headers[i]}\n')
        outputfile.write(contents[i])


# 关闭文件
outputfile.close()


####################################################################################################

# src文件夹路径
srcpath = os.path.join(basedir, '../sw/src')

# 所有的源文件文件名
sources = []

# 输出文件
outputfile = open(os.path.join(basedir, 'sw_all.cpp'), 'w', encoding='utf-8')
outputfile.write('#include "sw_all.h"\n')

# 输出的源文件代码
code = ''

# 添加文件到列表
for item in sorted(os.listdir(srcpath)):
    if os.path.isfile(os.path.join(srcpath, item)) and (item.endswith('.cpp') or item.endswith('.c')):
        sources.append(item)


for index in range(0, len(sources)):
    sourcepath = os.path.join(srcpath, sources[index])

    code += f'\n// {sources[index]}\n'

    with open(sourcepath, 'r', encoding='utf-8') as file:
        for line in file.readlines():
            line = line.rstrip()

            # 匹配include系统文件
            match = re.search(r'#include\s*<([^>]+)>', line)
            if match:
                incfile = match.group(1)
                if incfile not in included:
                    included.add(incfile)
                    outputfile.write(line + '\n')
                continue

            # 匹配include
            match = re.search(r'#include\s*"([^"]+)"', line)
            if match:
                continue

            # 不是include，记录文件内容
            code += line + '\n'


# 输出代码
outputfile.write(code)

# 关闭文件
outputfile.close()
