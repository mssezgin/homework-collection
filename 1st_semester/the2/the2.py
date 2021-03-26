
def isCovered(cp_bl, cp_tr, t1_bl, t1_tr, t2_bl, t2_tr):
    success = 'COMPLETELY COVERED'
    fail = 'NOT COMPLETELY COVERED'

    ax, ay = cp_bl
    bx, by = cp_tr
    cx, cy = t1_bl
    dx, dy = t1_tr
    ex, ey = t2_bl
    fx, fy = t2_tr

    c_a = (cx <= ax) and (cy <= ay)
    b_d = (bx <= dx) and (by <= dy)
    e_a = (ex <= ax) and (ey <= ay)
    b_f = (bx <= fx) and (by <= fy)

    if (c_a and b_d) or (e_a and b_f):  # at least one of the tarps covers coal completely
        return success
    elif c_a and ((ax < dx) and (by <= dy)) and ((ex <= dx) and (ey <= ay)) and b_f:  # left part and right part are covered separately
        return success
    elif e_a and ((ax < fx) and (by <= fy)) and ((cx <= fx) and (cy <= ay)) and b_d:
        return success
    elif c_a and ((bx < dx) and (ay <= dy)) and ((ex <= ax) and (ey <= dy)) and b_f:  # top part and bottom part are covered separately
        return success
    elif e_a and ((bx < fx) and (ay <= fy)) and ((cx <= ax) and (cy <= fy)) and b_d:
        return success
    else:
        return fail
