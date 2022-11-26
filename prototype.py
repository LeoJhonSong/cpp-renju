from random import randint
from copy import deepcopy


class Player:
    def __init__(self, name='PlayerName'):
        self.name = name
        self.isComputer = False

    def move(self, board):
        return 0, 0, False


class ManPlayer(Player):
    def move(self, board):
        prompt = 'Please input position to put piece: '
        if board.current != board.black:
            prompt += '(if you thing black side loss, press ENTER) '
        position = input(prompt)
        if position:
            return int(position[1:]), ord(position[0]) - 65, False  # row, col, forbidden
        else:
            return 0, 0, True


class ComputerPlayer(Player):
    def __init__(self, name='PlayerName'):
        super().__init__(name)
        self.isComputer = True

    def move(self, board):
        # TODO: may need improvement
        if board.current != board.black:
            if board.check_forbidden():
                return 0, 0, True
            while True:
                opponent_last = board.players[board.black]['last']
                col, row = opponent_last[0] + randint(-2, 2), opponent_last[1] + randint(-2, 2)
                if board.board[row][col] == '─┼─':
                    return row, col, False
        else:
            if board.players[board.black]['last'] is None:
                return 7, 7, False
            else:
                while True:
                    # 找到一个没子且不会形成禁手的地方下
                    temp_board = deepcopy(board.board)
                    opponent_last = board.players[board.white]['last']
                    col, row = opponent_last[0] + randint(-2, 2), opponent_last[1] + randint(-2, 2)
                    if temp_board[row][col] == '─┼─':
                        temp_board[row][col] = board.piece_list[2 * board.black]
                        if not board.check_forbidden(temp_board, (col, row)):
                            return row, col, False


class Board:
    """
    属性: {双方名称, 是否计算机属性}, [棋盘], 比分, 回合数, current
    方法: 胜负判断 (根据棋局/时间), 落子 (根据已落子处判断若无效再来), 显示 (棋子分黑白/是否最新), 计分, 主循环 (轮换先后手. 定死第一局人先黑)
    """
    piece_list = ['─●─', '─▲─', '─○─', '─△─']
    black = 0  # 黑方下标
    white = 1
    vectors = [
        (1, 0),
        (0, 1),
        (1, 1),
        (-1, 1)
    ]

    def __init__(self):
        self.board = []
        self.players = [
            {
                'player': ManPlayer('You'),
                'score': 0,
                'last': None
            },
            {
                'player': ComputerPlayer('Computer'),
                'score': 0,
                'last': None
            },
        ]
        self.clear()

    def clear(self):
        self.board = []
        for _ in range(15):
            self.board.append(['─┼─'] * 15)
        self.current = self.black
        for player in self.players:
            player['last'] = None

    def is_overline(self, board, cx, cy):
        """长连禁手检查"""
        for vec in self.vectors:
            line, c = self.line_gen(board, vec, cx, cy)
            left, right = self.find_continue(line, c)
            if right - left + 1 > 5:
                print(f"黑方{self.players[self.black]['player'].name}触发长连禁手!")
                return True
        return False

    def is_double_four(self, board, cx, cy):
        """双四检查, 有两个方向都是再在该方向上最近的空处放一个黑子就能五连"""
        count = 0
        for vec in self.vectors:
            if self.is_four(board, vec, cx, cy) > 0:
                count += 1  # 这个方向上能四
            if count > 1:  # 双四
                print(f"黑方{self.players[self.black]['player'].name}触发双四禁手!")
                return True
        return False

    def is_four(self, board, vec, cx, cy):
        """返回该方向上左右两侧补上一子能使黑子五连的子的个数, 返回2说明是活四, 返回1说明是四, 返回0说明没法四连"""
        line, c = self.line_gen(board, vec, cx, cy)
        count = 0
        for i, relative_distance in enumerate(self.find_continue(line, c)):  # 遍历连子的最左/最右子相对c的距离
            neighbour_position = c + relative_distance + (-1 if i == 0 else 1)  # 连子左/右边最近一个位置
            if 0 <= neighbour_position < len(line) and line[neighbour_position] != self.piece_list[2 * self.white]:  # 旁边可以落子的话
                temp_line = deepcopy(line)
                temp_line[neighbour_position] = self.piece_list[2 * self.black]
                left, right = self.find_continue(temp_line, c)
                if right - left + 1 != 5:  # 五连都形成不了
                    continue
                # 能形成五连, 检查这个黑子是否会触发禁手
                temp_board = deepcopy(board)
                temp_board[cy + (neighbour_position - c) * vec[1]][cx + (neighbour_position - c) * vec[0]] = board[cy][cx]
                if not self.check_forbidden(temp_board, (cx + relative_distance * vec[0], cy + relative_distance * vec[1])):
                    count += 1  # 这个地方落子能让黑子五连
        return count

    def is_double_three(self, board, cx, cy):
        """双活三检查"""
        count = 0
        for vec in self.vectors:
            if self.is_open_three(board, vec, cx, cy):
                count += 1  # 这个方向上有活三
            if count > 1:  # 双活三
                print(f"黑方{self.players[self.black]['player'].name}触发双三禁手!")
                return True
        return False

    def is_open_three(self, board, vec, cx, cy):
        """在该方向上左右任一侧补上一子能使黑子活四则是活三. 返回1是活三, 0则不是"""
        line, c = self.line_gen(board, vec, cx, cy)
        for i, relative_distance in enumerate(self.find_continue(line, c)):  # 遍历连子的最左/最右子相对c的距离
            neighbour_position = c + relative_distance + (-1 if i == 0 else 1)  # 连子左/右边最近一个位置
            if 0 <= neighbour_position < len(line) and line[neighbour_position] != self.piece_list[2 * self.white]:  # 旁边可以落子的话
                temp_board = deepcopy(board)
                temp_board[cy + (neighbour_position - c) * vec[1]][cx + (neighbour_position - c) * vec[0]] = board[cy][cx]
                if self.is_four(temp_board, vec, cx, cy) == 2:
                    # 这侧补一个黑子能形成活四, 检查这个黑子是否会触发禁手
                    if not self.check_forbidden(temp_board, (cx + relative_distance * vec[0], cy + relative_distance * vec[1])):
                        return 1
        return 0

    def check_forbidden(self, temp_board=None, temp_last=None):
        """检查黑方是否触发禁手, 有三种情况:
        1. 电脑玩家执黑时试下一子后调用进行自检
        2. 玩家执白时声明黑方触发禁手, 棋盘调用进行检查
        3. 电脑执白时调用检查黑方是否触发禁手"""
        if temp_last is None:
            temp_last = self.players[self.black]['last']  # self.current可黑可白, 但当前最后落子的一定是黑方
        winner = self.judge(temp_board, temp_last)  # 结果只能是0/black_side/3, 因为当前最后下子的一定是黑方
        if winner == self.black:
            return False  # 黑方五连与禁手同时形成时禁手失效
        board = self.board if temp_board is None else temp_board
        if temp_last is None:
            cx, cy = self.players[self.current]['last']
        else:
            cx, cy = temp_last
        return self.is_overline(board, cx, cy) or self.is_double_four(board, cx, cy) or self.is_double_three(board, cx, cy)

    def find_continue(self, line, c):
        """返回以(cx, cy)为中心, 与(cx, cy)相同的棋子的左下标相对距离和右下标相对距离"""
        left = c
        right = c
        while 0 <= left - 1 < len(line) and line[left - 1] == line[c]:
            left -= 1
        while 0 <= right + 1 < len(line) and line[right + 1] == line[c]:
            right += 1
        return left - c, right - c

    def line_gen(self, board, vec, cx, cy):
        """返回经过(cx, cy)的vec方向的线上所有子 (黑/白/空), 以及(cx, cy)在这个列表中的下标"""
        line = []
        step = 1
        while True:
            step -= 1
            current = (cx + step * vec[0], cy + step * vec[1])
            if 0 <= current[0] <= 14 and 0 <= current[1] <= 14:
                line.insert(0, board[current[1]][current[0]])
            else:
                break
        c = len(line) - 1
        step = 0
        while True:
            step += 1
            current = (cx + step * vec[0], cy + step * vec[1])
            if 0 <= current[0] <= 14 and 0 <= current[1] <= 14:
                line.append(board[current[1]][current[0]])
            else:
                break
        return line, c

    def judge(self, temp_board=None, temp_last=None):
        """返回-1尚无胜负/0黑方胜/1白方胜/2平局"""
        # 0 nothing 1 player1 win 2 player2 win 3 board full, draw
        # 不主动根据禁手判输, 只需判断刚下的一子的四条线是否五连
        board = self.board if temp_board is None else temp_board
        if temp_last is None:
            cx, cy = self.players[self.current]['last']
        else:
            cx, cy = temp_last
        last_piece = board[cy][cx]  # 刚下的棋子, 用于判定五连同色棋子
        side = self.piece_list.index(last_piece) // 2  # 正在判断是否五连的是黑方还是白方
        for vec in self.vectors:  # 遍历四个方向
            line, c = self.line_gen(board, vec, cx, cy)
            left, right = self.find_continue(line, c)
            count = right - left + 1
            if count == 5 or (side == self.white and count > 5):  # 若>5是黑方长连禁手, 但对白方算胜利
                return side
        for row in self.board:
            for piece in row:
                if piece == '─┼─':
                    return -1  # 如果棋盘还没满, 也没有五连则返回0
        return 2

    def move(self):
        # 返回当前棋局状态: -1/0/1/2
        self.draw()
        while True:
            # TODO: time limit for computer
            row, col, forbidden_claim = self.players[self.current]['player'].move(self)
            if forbidden_claim and self.current == self.black:
                print(f"{self.players[self.white]['player'].name}是白方, 没有禁手")
            if forbidden_claim:
                if self.check_forbidden():  # 检查黑方是否真的触发禁手
                    return self.white
                else:
                    print('黑方未形成禁手!')
            else:
                if self.board[row][col] == '─┼─':
                    self.board[row][col] = self.piece_list[2 * int(self.current == self.white)]
                    self.players[self.current]['last'] = (col, row)
                    return self.judge()
                else:
                    print('该位置已有子!')

    def draw(self):
        """draw board, score, current player"""
        print('-' * 45)
        print(f"(black) {self.players[self.black]['player'].name}: {self.players[self.black]['score']}\n(white) {self.players[self.white]['player'].name}: {self.players[self.white]['score']}")
        print(f"current: {self.players[self.current]['player'].name}")
        board = deepcopy(self.board)
        for i, player in enumerate(self.players):
            if player['last'] is not None:
                board[player['last'][1]][player['last'][0]] = self.piece_list[2 * i + 1]
        for line in board:
            print(''.join(line))

    def score(self, state):
        """结束一局后计算分数
        """
        if state == self.black:
            self.players[self.black]['score'] += 2
        elif state == self.white:
            self.players[self.white]['score'] += 2
        elif state == 2:  # 平局
            self.players[self.black]['score'] += 1
            self.players[self.white]['score'] += 1

    def run(self):
        for _ in range(2):
            self.clear()
            while (state := self.move()) == -1:
                self.current ^= 1
            self.score(state)
            self.draw()
            self.players = self.players[::-1]  # 下一局黑白方交换
            input('press Enter to Continue...')
        print('Finish!')


board = Board()
board.run()
