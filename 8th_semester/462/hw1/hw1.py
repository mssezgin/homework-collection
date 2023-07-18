import numpy as np
import queue


class State:

    def __init__(self, f, g, grid, parent, move):
        self.f      = f
        self.g      = g
        self.grid   = grid.astype(int)
        self.parent = parent
        self.move   = move

    def __lt__(self, other):
        if self.f < other.f:
            return True
        elif self.f > other.f:
            return False
        else:
            if self.g < other.g:
                return True
            elif self.g > other.g:
                return False
            else:
                return False

    def __le__(self, other):
        if self.f < other.f:
            return True
        elif self.f > other.f:
            return False
        else:
            if self.g < other.g:
                return True
            elif self.g > other.g:
                return False
            else:
                return True

    def __gt__(self, other):
        if self.f > other.f:
            return True
        elif self.f < other.f:
            return False
        else:
            if self.g > other.g:
                return True
            elif self.g < other.g:
                return False
            else:
                return False

    def __ge__(self, other):
        if self.f > other.f:
            return True
        elif self.f < other.f:
            return False
        else:
            if self.g > other.g:
                return True
            elif self.g < other.g:
                return False
            else:
                return True

    def __eq__(self, other):
        return np.array_equal(self.grid, other.grid)


class Problem:

    def __init__(self):
        # Cells
        self.EMPTY = 0
        self.WALL  = 1
        self.AGENT = 2
        self.ROCK  = 3
        self.GATE  = 4
        self.BLOCKING_OBJS = [self.WALL, self.ROCK]
        self.FALLING_OBJS  = [self.GATE, self.ROCK, self.AGENT]
        # Moves
        self.MOVES = ['CL', 'L', 'CR', 'R']
        # Other
        self.HEIGHT = 6

    def move_agent(self, grid, move):
        if move == 'L':
            return self.free_fall(self.move_agent_left_or_right(grid, -1))
        elif move == 'CL':
            return self.free_fall(self.move_agent_climb_left_or_right(grid, -1))
        elif move == 'R':
            return self.free_fall(self.move_agent_left_or_right(grid, +1))
        elif move == 'CR':
            return self.free_fall(self.move_agent_climb_left_or_right(grid, +1))
        else:
            print('!!! invalid move !!!')
            return grid

    def move_agent_left_or_right(self, grid, direction):
        new_grid = np.copy(grid)
        [r], [c] = np.where(new_grid == self.AGENT)

        # stay put if right cell is out of the grid
        if c + direction < 0 or c + direction >= self.WIDTH:
            return new_grid

        # stay put if right cell is wall
        if new_grid[r, c + direction] == self.WALL:
            return new_grid

        # check if right cell is rock
        if new_grid[r, c + direction] == self.ROCK:

            # stay put if above cell of rock is not empty
            if new_grid[r + 1, c + direction] != self.EMPTY:
                return new_grid

            # stay put if right cell of rock is out of the grid
            if c + 2 * direction < 0 or c + 2 * direction >= self.WIDTH:
                return new_grid

            # check if right cell of rock is blocking object
            if new_grid[r, c + 2 * direction] in self.BLOCKING_OBJS:

                # stay put if above cell of blocking object is not empty
                if new_grid[r + 1, c + 2 * direction] != self.EMPTY:
                    return new_grid

                # move rock on top of blocking object
                new_grid[r, c + direction] = self.EMPTY
                new_grid[r + 1, c + 2 * direction] = self.ROCK

            # erase rock if right cell of rock is gate
            elif new_grid[r, c + 2 * direction] == self.GATE:
                new_grid[r, c + direction] = self.EMPTY

            else:
                # move rock to right cell
                new_grid[r, c + direction] = self.EMPTY
                new_grid[r, c + 2 * direction] = self.ROCK

        # return success if right cell is gate
        if new_grid[r, c + direction] == self.GATE:
            new_grid[r, c] = self.EMPTY
            return new_grid

        # move agent to right cell
        new_grid[r, c] = self.EMPTY
        new_grid[r, c + direction] = self.AGENT
        return new_grid

    def move_agent_climb_left_or_right(self, grid, direction):
        new_grid = np.copy(grid)
        [r], [c] = np.where(new_grid == self.AGENT)

        # stay put if right cell is out of the grid
        if c + direction < 0 or c + direction >= self.WIDTH:
            return new_grid

        # stay put if right cell is not blocking object
        if new_grid[r, c + direction] not in self.BLOCKING_OBJS:
            return new_grid

        # stay put if above cell of blocking object is wall
        if new_grid[r + 1, c + direction] == self.WALL:
            return new_grid

        # check if above cell of blocking object is rock
        if new_grid[r + 1, c + direction] == self.ROCK:

            # stay put if above cell of rock is not empty
            if new_grid[r + 2, c + direction] != self.EMPTY:
                return new_grid

            # stay put if right cell of rock is out of the grid
            if c + 2 * direction < 0 or c + 2 * direction >= self.WIDTH:
                return new_grid

            # stay put if right cell of rock is blocking object
            if new_grid[r + 1, c + 2 * direction] in self.BLOCKING_OBJS:
                return new_grid

            # erase rock if right cell of rock is gate
            if new_grid[r + 1, c + 2 * direction] == self.GATE:
                new_grid[r + 1, c + direction] = self.EMPTY

            # move rock to right cell
            else:
                new_grid[r + 1, c] = self.EMPTY
                new_grid[r + 1, c + 2 * direction] = self.ROCK

        # return success if right cell is gate
        if new_grid[r + 1, c + direction] == self.GATE:
            new_grid[r, c] = self.EMPTY
            return new_grid

        # move agent to above cell of right cell
        new_grid[r, c] = self.EMPTY
        new_grid[r + 1, c + direction] = self.AGENT
        return new_grid

    def free_fall(self, grid):
        new_grid = np.copy(grid)
        for obj in self.FALLING_OBJS:
            rows, cols = np.where(new_grid == obj)
            for r, c in zip(rows, cols):
                new_grid[r, c] = self.EMPTY
                # fall if below cell is empty
                while new_grid[r - 1, c] == self.EMPTY:
                    r -= 1
                # disappear if below cell is gate
                if new_grid[r - 1, c] == self.GATE:
                    # return success if agent reaches the gate
                    if obj == self.AGENT:
                        pass
                else:
                    new_grid[r, c] = obj
        return new_grid

    def calculate_height(self, col):
        # calculate index of top-most wall
        return np.where(col == self.WALL)[0][-1]

    def calculate_estimated_cost(self, grid):
        if self.is_goal_state(State(None, None, grid, None, None)):
            return 0

        [ra], [ca] = np.where(grid == self.AGENT)
        [rg], [cg] = np.where(grid == self.GATE)

        if ca < cg:
            cols = grid[:, ca:cg + 1].T
        else:
            # cols = grid[:, cg:ca - 1:-1].T
            cols = grid[:, cg:ca + 1][:, ::-1].T

        h = 0
        for col, next_col in zip(cols[:-1], cols[1:]):
            height_col      = self.calculate_height(col)
            height_next_col = self.calculate_height(next_col)
            if height_col > height_next_col - 2:
                h += 1
            else:
                h += height_next_col - height_col - 1
        return h

    def calculate_heuristic(self, g, h):
        # return g + 2 * h
        return g + h

    def is_goal_state(self, s):
        # agent must have been erased from grid when reached goal
        r, c = np.where(s.grid == self.AGENT)
        return r.size == 0

    def solve(self):
        if self.ALGORITHM == 'A*':
            path = self.astar()
        elif self.ALGORITHM == 'IDA*':
            path = self.idastar()

        if path:
            print('SUCCESS')
            moves = []
            while path:
                s = path.pop()
                if s.move:
                    moves.append(s.move)
            print(','.join(moves))
        else:
            print('FAILURE')

    def astar(self):
        open = queue.PriorityQueue()
        closed = []

        # put initial state to open list
        g = 0
        h = self.calculate_estimated_cost(self.grid)
        f = self.calculate_heuristic(g, h)
        s_0 = State(f, g, self.grid, None, None)
        open.put(s_0)

        # loop until open list is empty
        while not open.empty():

            # get the state with smallest heuristic value
            s_x = open.get()

            # return SUCCESS if goal state
            if self.is_goal_state(s_x):
                path = []
                s_p = s_x
                while s_p is not None:
                    path.append(s_p)
                    s_p = s_p.parent
                return path

            # check if state x is already in closed
            try:
                i_s_x = closed.index(s_x)
            except ValueError:
                i_s_x = -1

            # if state x is not in closed
            # or else its heuristic is smaller than previous one,
            if i_s_x == -1 or s_x < closed[i_s_x]:
                # add state x to closed
                closed.append(s_x)
                # expand state x
                for move in self.MOVES:
                    new_grid = self.move_agent(s_x.grid, move)
                    g = s_x.g + 1
                    h = self.calculate_estimated_cost(new_grid)
                    f = self.calculate_heuristic(g, h)
                    s_y = State(f, g, new_grid, s_x, move)
                    open.put(s_y)

        # return FAILURE if open list is empty
        else:
            return []

    def idastar(self):
        # > you can take 200 as the upper bound on 'depth'

        # initial state
        g = 0
        h = self.calculate_estimated_cost(self.grid)
        f = self.calculate_heuristic(g, h)
        s_0 = State(f, g, self.grid, None, None)

        f_max = s_0.f
        while True:
            path, new_f_max = self.limited_search(s_0, f_max)
            if path:
                return path
            f_max = new_f_max

    def limited_search(self, s_x, f_max):

        # return FAILURE if f_max is exceeded
        if s_x.f > f_max:
            return [], s_x.f

        # return SUCCESS if goal state
        if self.is_goal_state(s_x):
            path = []
            s_p = s_x
            while s_p is not None:
                path.append(s_p)
                s_p = s_p.parent
            return path, -1

        new_f_max = []
        # recurse
        for move in self.MOVES:
            new_grid = self.move_agent(s_x.grid, move)
            g = s_x.g + 1
            h = self.calculate_estimated_cost(new_grid)
            f = self.calculate_heuristic(g, h)
            s_y = State(f, g, new_grid, s_x, move)

            path, new_child_f_max = self.limited_search(s_y, f_max)
            if path:
                return path, -1
            else:
                new_f_max.append(new_child_f_max)

        # return FAILURE since no children returned SUCCESS
        return [], min(new_f_max)

    def init_from_file(self, filename):
        with open(filename) as f:
            lines = f.readlines()

        if lines[0].split()[0] == 'A*':
            self.ALGORITHM = 'A*'
        elif lines[0].split()[0] == 'IDA*':
            self.ALGORITHM = 'IDA*'
        else:
            print('!!! invalid search algorithm !!!')

        state = ''
        for line in lines[1:]:
            strings = line.split()
            if strings[0] == 'W':
                # number of specified wall-heights decide the grid's width
                self.WIDTH = len(strings) - 1
                self.grid = self.EMPTY * np.ones((self.HEIGHT, self.WIDTH))
                for col, string in enumerate(strings[1:]):
                    # height is always a nonnegative number
                    height = int(string)
                    self.grid[height::-1, col] = self.WALL
            else:
                state += strings[0] + ','
                for string in strings[1:]:
                    state += string.zfill(2) + ','

        state = state[:-1]
        self.starting_state = state
        self.set_from_state(state)

    # a STATE encoding looks like: "R,02,02,A,01,G,09"
    # STATE should start with 'R,'
    def set_from_state(self, state):
        self.grid = np.where(self.grid == self.WALL, self.grid, np.ones(self.grid.shape) * self.EMPTY)

        reading_cell = self.EMPTY
        for string in state.split(','):
            if string == 'A':
                reading_cell = self.AGENT
            elif string == 'R':
                reading_cell = self.ROCK
            elif string == 'G':
                reading_cell = self.GATE
            else:
                col = int(string)
                # place the object on top
                self.grid[self.HEIGHT - 1, col] = reading_cell
                # let it fall
                self.grid = self.free_fall(self.grid)

    def pretty_print(self, grid = None):
        if grid is None:
            grid = self.grid

        print('=' * (self.WIDTH * 2 + 5))
        for row in grid[self.HEIGHT - 1::-1, :]:
            print('|| ', end='')
            for val in row:
                if val == self.EMPTY:
                    to_disp = ' '
                elif val == self.WALL:
                    to_disp = 'W'
                elif val == self.AGENT:
                    to_disp = 'A'
                elif val == self.ROCK:
                    to_disp = 'R'
                elif val == self.GATE:
                    to_disp = 'G'
                else:
                    to_disp = '*'
                print(f'{to_disp} ', end='')
            print('||')
        print('=' * (self.WIDTH * 2 + 5))


if __name__ == '__main__':
    filename = input()
    p = Problem()
    p.init_from_file(filename)
    # p.pretty_print()
    p.solve()
