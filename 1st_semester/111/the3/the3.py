# python the3.py map.txt rules.txt 3
# sys.argv = ['the3.py', 'map.txt', 'rules.txt', '3']


import sys


# opening rules.txt and defining rules

with open(sys.argv[2], 'r') as rf:
    rules = []
    for line in rf.readlines():
        rules.append(line.strip())


def execute_rules(cell, nbcells):
    global rules
    nbcount = nbcells.count('*')

    for r in rules:
        a = int(r[2])
        if cell == r[0]:
            if r[1] == '=' and nbcount == a:
                return r[3]
            elif r[1] == '<' and nbcount < a:
                return r[3]
            elif r[1] == '>' and nbcount > a:
                return r[3]

    return cell


# opening map.txt

with open(sys.argv[1], 'r') as mf:
    map = []
    for line in mf.readlines():
        map.append(line.strip())


row_count = len(map)
coloumn_count = len(map[0])


def neigbors_calculator(r, c):
    global row_count
    global coloumn_count

    if 0 < r < row_count - 1:
        if 0 < c < coloumn_count - 1:
            return [map[r-1][c-1], map[r-1][c], map[r-1][c+1], map[r][c-1], map[r][c+1], map[r+1][c-1], map[r+1][c], map[r+1][c+1]]
        elif coloumn_count == 1:
            return [map[r-1][c], map[r+1][c]]
        elif c == 0:
            return [map[r-1][c], map[r-1][c+1], map[r][c+1], map[r+1][c], map[r+1][c+1]]
        else:
            return [map[r-1][c-1], map[r-1][c], map[r][c-1], map[r+1][c-1], map[r+1][c]]
    elif row_count == 1:
        if 0 < c < coloumn_count - 1:
            return [map[r][c-1], map[r][c+1]]
        elif coloumn_count == 1:
            return []
        elif c == 0:
            return [map[r][c+1]]
        else:
            return [map[r][c-1]]
    elif r == 0:
        if 0 < c < coloumn_count - 1:
            return [map[r][c-1], map[r][c+1], map[r+1][c-1], map[r+1][c], map[r+1][c+1]]
        elif coloumn_count == 1:
            return [map[r+1][c]]
        elif c == 0:
            return [map[r][c+1], map[r+1][c], map[r+1][c+1]]
        else:
            return [map[r][c-1], map[r+1][c-1], map[r+1][c]]
    else:
        if 0 < c < coloumn_count - 1:
            return [map[r-1][c-1], map[r-1][c], map[r-1][c+1], map[r][c-1], map[r][c+1]]
        elif coloumn_count == 1:
            return [map[r-1][c]]
        elif c == 0:
            return [map[r-1][c], map[r-1][c+1], map[r][c+1]]
        else:
            return [map[r-1][c-1], map[r-1][c], map[r][c-1]]


# generating generations

for i in range(int(sys.argv[3])):
    next_gen = []

    for row in range(row_count):
        next_gen.append('')
        for coloumn in range(coloumn_count):
            next_gen[row] += execute_rules(map[row][coloumn], neigbors_calculator(row, coloumn))

    map = next_gen[:]


# printing the resulting generation

print '\n'.join(map)
