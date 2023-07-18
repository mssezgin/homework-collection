import socket
import sys
import threading
import time


CENG435_PORT      = 0               # set later
CENG435_HOST      = "localhost"
CENG435_PRE_SLEEP = 1
CENG435_TIMEOUT   = 5
CENG435_PERIOD    = 0.01


def initialize_costs(I):
    with open(f"{CENG435_PORT}.costs", "r") as f:
        lines = f.readlines()

    N = int(lines.pop(0))
    neigs = [[int(elem) for elem in line.strip().split()] for line in lines]
    costs = [[float("inf") for _ in range(N)] for _ in range(N)]
    for neig_port, cost in neigs:
        costs[I][neig_port - 3000] = cost
    costs[I][I] = 0

    return N, neigs, costs


def update_costs(I, N, costs, data):
    lines = data.splitlines()

    # update sender's costs to its neighbors
    sender_port = int(lines.pop(0))
    for i, cost in enumerate(lines):
        costs[sender_port - 3000][i] = float(cost)

    # update own costs if any (relaxation)
    is_updated = False
    for j in range(N):
        for k in range(N):
            cost_thru = costs[I][k] + costs[k][j]
            if cost_thru < costs[I][j]:
                costs[I][j] = cost_thru
                is_updated = True

    return costs, is_updated


def send_to_neighbors(I, costs, neigs):
    # prepare own vector
    lines = [str(CENG435_PORT)] + [str(cost) for cost in costs[I]]
    data = "\n".join(lines)

    # send the vector to neighbors
    for neig_port, _ in neigs:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((CENG435_HOST, neig_port))
        s.send(data.encode())
        s.shutdown(socket.SHUT_RDWR)
        s.close()



if __name__ == "__main__":

    # check if port number is provided
    try:
        CENG435_PORT = int(sys.argv[1])
    except:
        sys.exit()

    # variables
    I = CENG435_PORT - 3000
    N = 0
    neigs = []
    costs = []

    # initalize costs and send it to neighbors
    N, neigs, costs = initialize_costs(I)

    # create socket and listen
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.settimeout(CENG435_TIMEOUT)
    s.bind((CENG435_HOST, CENG435_PORT))
    s.listen()

    # send to neigbors
    time.sleep(CENG435_PRE_SLEEP)
    # send_to_neighbors(I, costs, neigs)
    t = threading.Thread(target=send_to_neighbors, args=(I, costs, neigs, ))
    t.start()

    # loop
    while True:
        try:
            # wait for a new vector from neighbors, until timeout
            c, addr = s.accept()
            data = c.recv(1024)
            c.shutdown(socket.SHUT_RDWR)
            c.close()
        except socket.timeout:
            break

        # update costs using coming vector
        costs, is_updated = update_costs(I, N, costs, data.decode())

        # send to neighbors
        if is_updated:
            # send_to_neighbors(I, costs, neigs)
            t = threading.Thread(target=send_to_neighbors, args=(I, costs, neigs, ))
            t.start()

    # close socket
    s.shutdown(socket.SHUT_RDWR)
    s.close()

    # print output
    for j, cost in enumerate(costs[I]):
        if cost != float("inf"):
            print(f"{CENG435_PORT} -{3000 + j} | {int(cost)}")
