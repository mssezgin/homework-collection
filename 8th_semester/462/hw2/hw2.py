import numpy as np
import sys

class Environment:
    CELL_EMPTY    = 0
    CELL_OBSTACLE = 1
    CELL_PUDDLE   = 2
    CELL_GOAL     = 3

    def __init__(self, params):

        # initialize method parameters
        self.method     = params['method']
        self.n_iter     = params['n_iter']
        self.alpha      = params['alpha'] if self.method == 'Q' else None
        self.gamma      = params['gamma']
        self.epsilon    = params['epsilon']
        self.r_regular  = params['r_regular']
        self.r_obstacle = params['r_obstacle']
        self.r_puddle   = params['r_puddle']
        self.r_goal     = params['r_goal']

        # initialize the grid
        self.M, self.N = params['dims']
        self.grid = np.full((M+2, N+2), Environment.CELL_EMPTY, dtype=int)
        self.grid[0, :]        = Environment.CELL_OBSTACLE
        self.grid[self.M+1, :] = Environment.CELL_OBSTACLE
        self.grid[:, 0]        = Environment.CELL_OBSTACLE
        self.grid[:, self.N+1] = Environment.CELL_OBSTACLE
        for (x, y) in params['obstacles']:
            self.grid[y, x] = Environment.CELL_OBSTACLE
        for (x, y) in params['puddles']:
            self.grid[y, x] = Environment.CELL_PUDDLE
        x, y = params['goal']
        self.grid[y, x] = Environment.CELL_GOAL

        self.s_goal = params['goal']

        # initialize markov tuple
        self.S = np.array([(x, y) for x in range(1, self.N+1) for y in range(1, self.M+1)], dtype='i,i')
        self.A = np.array([0, 1, 2, 3], dtype=int) # up, right, down, left
        self.T = self.init_transition()
        self.R = self.init_reward()

    def init_transition(self):
        # hold a dictionary to quickly get state index
        s_to_s_index = {}
        for s_index, s in enumerate(self.S):
            s_to_s_index[tuple(s)] = s_index

        T = np.zeros((self.S.shape[0], self.A.shape[0], self.S.shape[0]))
        for s_index, s in enumerate(self.S):
            # up
            s_up = (s[0], s[1] + 1)
            if self.grid[s_up[1], s_up[0]] == Environment.CELL_OBSTACLE:
                T[s_index, 0, s_index] += 0.8
                T[s_index, 1, s_index] += 0.09
                T[s_index, 2, s_index] += 0.02
                T[s_index, 3, s_index] += 0.09
            else:
                s_up_index = s_to_s_index[s_up]
                T[s_index, 0, s_up_index] = 0.8
                T[s_index, 1, s_up_index] = 0.09
                T[s_index, 2, s_up_index] = 0.02
                T[s_index, 3, s_up_index] = 0.09
            # right
            s_right = (s[0] + 1, s[1])
            if self.grid[s_right[1], s_right[0]] == Environment.CELL_OBSTACLE:
                T[s_index, 0, s_index] += 0.09
                T[s_index, 1, s_index] += 0.8
                T[s_index, 2, s_index] += 0.09
                T[s_index, 3, s_index] += 0.02
            else:
                s_right_index = s_to_s_index[s_right]
                T[s_index, 0, s_right_index] = 0.09
                T[s_index, 1, s_right_index] = 0.8
                T[s_index, 2, s_right_index] = 0.09
                T[s_index, 3, s_right_index] = 0.02
            # down
            s_down = (s[0], s[1] - 1)
            if self.grid[s_down[1], s_down[0]] == Environment.CELL_OBSTACLE:
                T[s_index, 0, s_index] += 0.02
                T[s_index, 1, s_index] += 0.09
                T[s_index, 2, s_index] += 0.8
                T[s_index, 3, s_index] += 0.09
            else:
                s_down_index = s_to_s_index[s_down]
                T[s_index, 0, s_down_index] = 0.02
                T[s_index, 1, s_down_index] = 0.09
                T[s_index, 2, s_down_index] = 0.8
                T[s_index, 3, s_down_index] = 0.09
            # left
            s_left = (s[0] - 1, s[1])
            if self.grid[s_left[1], s_left[0]] == Environment.CELL_OBSTACLE:
                T[s_index, 0, s_index] += 0.09
                T[s_index, 1, s_index] += 0.02
                T[s_index, 2, s_index] += 0.09
                T[s_index, 3, s_index] += 0.8
            else:
                s_left_index = s_to_s_index[s_left]
                T[s_index, 0, s_left_index] = 0.09
                T[s_index, 1, s_left_index] = 0.02
                T[s_index, 2, s_left_index] = 0.09
                T[s_index, 3, s_left_index] = 0.8
        return T

    def init_reward(self):
        R = np.zeros((self.S.shape[0], self.A.shape[0]))
        for s_index, s in enumerate(self.S):
            for a in self.A:
                # find the next state
                if a == 0:
                    s_next = (s[0], s[1] + 1)
                elif a == 1:
                    s_next = (s[0] + 1, s[1])
                elif a == 2:
                    s_next = (s[0], s[1] - 1)
                elif a == 3:
                    s_next = (s[0] - 1, s[1])
                # find reward according to the type of next state
                x, y = s_next
                cell = self.grid[y, x]
                if cell == Environment.CELL_EMPTY:
                    R[s_index, a] = self.r_regular
                elif cell == Environment.CELL_OBSTACLE:
                    R[s_index, a] = self.r_obstacle
                elif cell == Environment.CELL_PUDDLE:
                    R[s_index, a] = self.r_puddle
                elif cell == Environment.CELL_GOAL:
                    R[s_index, a] = self.r_goal
        return R

    def policy_evaluation(self, policy):
        # initialize state values (all zeros)
        V = np.zeros(self.S.shape[0])

        # iterate until state values differ no more than epsilon
        while True:
            V_prev = V.copy()

            i_0 = np.arange(self.S.shape[0])
            i_1 = np.array([policy[s_index] for s_index in range(self.S.shape[0])])
            V = self.R[i_0, i_1] + self.gamma * np.dot(self.T[i_0, i_1], V_prev)

            # check termination
            if np.max(np.abs(V - V_prev)) <= self.epsilon:
                break

        return V

    def policy_improvement(self, V):
        Q = self.R + self.gamma * np.dot(self.T, V)
        policy = np.argmax(Q, axis=1)
        return policy

    def solve_using_policy_iteration(self):
        # initialize policy (all zeros)
        policy = np.zeros(self.S.shape[0], dtype=int)

        # iterate until limit is reached
        for _ in range(self.n_iter):
            policy_prev = policy.copy()
            V = self.policy_evaluation(policy)
            policy = self.policy_improvement(V)

            # check termination
            if np.array_equal(policy, policy_prev):
                break

        return policy

    def get_random_agent_location(self):
        while True:
            x = np.random.randint(1, self.N+1)
            y = np.random.randint(1, self.M+1)
            if self.grid[y, x] == Environment.CELL_EMPTY:
                break
        return (x, y)

    def apply_action(self, s, a):
        # find the next state
        if a == 0:
            s_next = (s[0], s[1] + 1)
        elif a == 1:
            s_next = (s[0] + 1, s[1])
        elif a == 2:
            s_next = (s[0], s[1] - 1)
        elif a == 3:
            s_next = (s[0] - 1, s[1])
        # no move if there is an obstacle
        x, y = s_next
        if self.grid[y, x] == Environment.CELL_OBSTACLE:
            s_next = s
        return s_next

    def epsilon_greedy(self, Q, s_index):
        if np.random.rand() < self.epsilon:
            return np.random.randint(self.A.shape[0])
        else:
            return np.argmax(Q[s_index])

    def solve_using_q_learning(self):
        # hold a dictionary to quickly get state index
        s_to_s_index = {}
        for s_index, s in enumerate(self.S):
            s_to_s_index[tuple(s)] = s_index

        # initialize Q (all zeros)
        Q = np.zeros((self.S.shape[0], self.A.shape[0]))
        s_goal_index = s_to_s_index[self.s_goal]

        # iterate number of episodes times
        for _ in range(self.n_iter):

            # initialize random agent location
            s = self.get_random_agent_location()
            s_index = s_to_s_index[s]

            # iterate until agent reaches the goal
            while s_index != s_goal_index:

                # select an action
                a = self.epsilon_greedy(Q, s_index)

                # find the next state
                s_next = self.apply_action(s, a)
                s_next_index = s_to_s_index[s_next]

                # update action value
                Q[s_index, a] = (1 - self.alpha) * Q[s_index, a] + self.alpha * (self.R[s_index, a] + self.gamma * np.max(Q[s_next_index]))

                # move to the next state
                s = s_next
                s_index = s_next_index

        policy = np.argmax(Q, axis=1)
        return policy

    def solve(self):
        if self.method == 'P':
            return self.solve_using_policy_iteration()
        if self.method == 'Q':
            return self.solve_using_q_learning()


if __name__ == "__main__":

    hw2_input = {}

    # parse file
    in_filepath = sys.argv[1]
    with open(in_filepath, 'r') as f:

        # method
        line = f.readline().strip()
        hw2_input['method'] = line

        # number of iterations
        line = f.readline().strip()
        hw2_input['n_iter'] = int(line)

        # alpha
        if hw2_input['method'] == 'Q':
            line = f.readline().strip()
            hw2_input['alpha'] = float(line)

        # gamma
        line = f.readline().strip()
        hw2_input['gamma'] = float(line)

        # epsilon
        line = f.readline().strip()
        hw2_input['epsilon'] = float(line)

        # dimensions of the grid
        line = f.readline().strip()
        M = int(line.split(' ')[0])
        N = int(line.split(' ')[1])
        hw2_input['dims'] = (M, N)

        # number of obstacles
        line = f.readline().strip()
        n = int(line)
        hw2_input['obstacles'] = []
        for i in range(n):
            # coordinates of the obstacle
            line = f.readline().strip()
            o_x = int(line.split(' ')[0])
            o_y = int(line.split(' ')[1])
            hw2_input['obstacles'].append((o_x, o_y))

        # number of puddles
        line = f.readline().strip()
        n = int(line)
        hw2_input['puddles'] = []
        for i in range(n):
            # coordinates of the puddle
            line = f.readline().strip()
            o_x = int(line.split(' ')[0])
            o_y = int(line.split(' ')[1])
            hw2_input['puddles'].append((o_x, o_y))

        # coordinates of the goal
        line = f.readline().strip()
        x = int(line.split(' ')[0])
        y = int(line.split(' ')[1])
        hw2_input['goal'] = (x, y)

        # rewards
        line = f.readline().strip()
        hw2_input['r_regular']  = float(line.split(' ')[0])
        hw2_input['r_obstacle'] = float(line.split(' ')[1])
        hw2_input['r_puddle']   = float(line.split(' ')[2])
        hw2_input['r_goal']     = float(line.split(' ')[3])

    # create environment object
    environment = Environment(hw2_input)

    # solve problem
    policy = environment.solve()

    # write to file
    out_filepath = sys.argv[2]
    with open(out_filepath, 'w') as f:
        for s_index, a in enumerate(policy):
            x, y = environment.S[s_index] 
            f.write(str(x) + ' ' + str(y) + ' ' + str(a) + '\n')
