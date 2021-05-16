
def will_it_be_closed(tree, supertree):
    table = []

    if type(tree[1]) == int:
        if tree[1] == 1:
            table.append((tree[0], 1))
            supertree.remove(tree)
        else:
            table.append((tree[0], 0))
            tree[1] -= 1
        return table
    else:
        subtable = []
        x = 1

        for child in tree[1:]:
            ch = will_it_be_closed(child, tree)
            subtable.append(ch)
            x = x and ch[0][1]

        if len(tree) == 1:
            supertree.remove(tree)

        table += [(tree[0], x)] + subtable
        return table


def which_be_closed(table):
    closed_ones = []

    if table[0][1] == 1:
        closed_ones.append(table[0][0])
    elif len(table) > 1:

        for subtable in table[1:]:
            sb = which_be_closed(subtable)
            closed_ones += sb

    return closed_ones


def chalchiuhtlicue(pipe_system):
    result = []
    covered = [pipe_system]

    while covered:
        result.append(which_be_closed(will_it_be_closed(pipe_system, covered)))
    return result
