import serial
import random


def initUart5():
    ser5 = serial.Serial('com5', 9600, timeout=0.5)
    if not ser5.isOpen():
        ser5.open()
    return ser5


def sendMove(x0, y0, x1, y1):
    move = ""
    move = move + str(x0) + str(y0) + str(x1) + str(y1) + '\0'
    ser.write(move.encode())


def getChessBoard():
    stm_chess_board = ""
    while 1:
        if ser.inWaiting() != 0:
            stm_chess_board = ser.readline()
        if stm_chess_board != "":
            break
    return stm_chess_board


def judge(x0, y0, x1, y1, black, chessboard):
    chess = [[0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0]]
    for i in range(0, 5):
        for j in range(0, 5):
            chess[i][j] = chessboard[i][j]
    score = 0
    chess[x0][y0] = 0
    chess[x1][y1] = 2
    xcount = 0
    for i in range(0, 5):
        if chess[x1][i] != 0:
            xcount = xcount + 1
    if xcount == 3:
        if y1 > 1: # 左边有2个位子及以上
            if chess[x1][y1-1] == 2 and chess[x1][y1-2] == 1:
                score = score + 100
        if 4 > y1 > 0:
            if chess[x1][y1-1] == 2 and chess[x1][y1+1] == 1:
                score = score + 100
            if chess[x1][y1+1] == 2 and chess[x1][y1-1] == 1:
                score = score + 100
        if y1 < 3: # 右边有2个位子及以上
            if chess[x1][y1+1] == 2 and chess[x1][y1+2] == 1:
                score = score + 100
    ycount = 0
    for i in range(0,5):
        if chess[i][y1] != 0:
            ycount = ycount + 1
    if ycount == 3:
        if x1 > 1:
            if chess[x1 - 1][y1] == 2 and chess[x1 - 2][y1] == 1:
                score = score + 100
        if 4 > x1 > 0:
            if chess[x1 - 1][y1] == 2 and chess[x1 + 1][y1] == 1:
                score = score + 100
            if chess[x1 + 1][y1] == 2 and chess[x1 - 1][y1] == 1:
                score = score + 100
        if x1 < 3:
            if chess[x1 + 1][y1] == 2 and chess[x1 + 2][y1] == 1:
                score = score + 100
    score = score + judgeBlack(black, chess)
    return score


def judgeEatWhite(x0, y0, x1, y1, black, chessboard):
    chess = [[0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0]]
    for i in range(0, 5):
        for j in range(0, 5):
            chess[i][j] = chessboard[i][j]
    score = 0
    chess[x0][y0] = 0
    chess[x1][y1] = 1
    xcount = 0
    for i in range(0, 5):
        if chess[x1][i] != 0:
            xcount = xcount + 1
    if xcount == 3:
        if y1 > 1: # 左边有2个位子及以上
            if chess[x1][y1-1] == 1 and chess[x1][y1-2] == 2:
                score = score + 200
        if 4 > y1 > 0:
            if chess[x1][y1-1] == 1 and chess[x1][y1 + 1] == 2:
                score = score + 200
            if chess[x1][y1+1] == 1 and chess[x1][y1 - 1] == 2:
                score = score + 200
        if y1 < 3: # 右边有2个位子及以上
            if chess[x1][y1+1] == 1 and chess[x1][y1+2] == 2:
                score = score + 200
    ycount = 0
    for i in range(0,5):
        if chess[i][y1] != 0:
            ycount = ycount + 1
    if ycount == 3:
        if x1 > 1:
            if chess[x1 - 1][y1] == 1 and chess[x1 - 2][y1] == 2:
                score = score + 200
        if 4 > x1 > 0:
            if chess[x1 - 1][y1] == 1 and chess[x1 + 1][y1] == 2:
                score = score + 200
            if chess[x1 + 1][y1] == 1 and chess[x1 - 1][y1] == 2:
                score = score + 200
        if x1 < 3:
            if chess[x1 + 1][y1] == 1 and chess[x1 + 2][y1] == 2:
                score = score + 200
    return score


def judgeBlack(black, chessboard):
    score = 0
    for everyblack in black:
        if everyblack[1] > 0:
            score -= judgeEatWhite(everyblack[0], everyblack[1], everyblack[0], everyblack[1] - 1, black, chessboard)
        if everyblack[1] < 4:
            score -= judgeEatWhite(everyblack[0], everyblack[1], everyblack[0], everyblack[1] + 1, black, chessboard)
        if everyblack[0] > 0:
            score -= judgeEatWhite(everyblack[0], everyblack[1], everyblack[0] - 1, everyblack[1], black, chessboard)
        if everyblack[0] < 4:
            score -= judgeEatWhite(everyblack[0], everyblack[1], everyblack[0] + 1, everyblack[1], black, chessboard)
    return score


if __name__ == '__main__':
    ser = initUart5()
    chessBoard = ""
    chess = [[0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0]]
    whiteChess = []
    blackChess = []
    score = -65535
    X0 = 0
    X1 = 0
    Y0 = 0
    Y1 = 0
    while 1:
        chessBoard = getChessBoard()
        for i in range(0, 5):
            for j in range(0, 5):
                chess[i][j] = int(chessBoard[i*5 + j]) - 48
                if chess[i][j] == 1:
                    blackChess.append([i, j])
                elif chess[i][j] == 2:
                    whiteChess.append([i, j])

        for white in whiteChess:
            if white[1] > 1:
                if chess[white[0]][white[1]-1] == 0:
                    upScore = judge(white[0], white[1], white[0], white[1]-1, blackChess, chess)
                    if upScore > score:
                        score = upScore
                        X0 = white[0]
                        Y0 = white[1]
                        X1 = white[0]
                        Y1 = white[1] - 1
                    elif upScore == score and random.random() >= 0.5:
                        score = upScore
                        X0 = white[0]
                        Y0 = white[1]
                        X1 = white[0]
                        Y1 = white[1] - 1

            if white[1] < 4:
                if chess[white[0]][white[1] + 1] == 0:
                    upScore = judge(white[0], white[1], white[0], white[1]+1, blackChess, chess)
                    if upScore > score:
                        score = upScore
                        X0 = white[0]
                        Y0 = white[1]
                        X1 = white[0]
                        Y1 = white[1] + 1
                    elif upScore == score and random.random() >= 0.5:
                        score = upScore
                        X0 = white[0]
                        Y0 = white[1]
                        X1 = white[0]
                        Y1 = white[1] + 1

            if white[0] > 0:
                if chess[white[0] - 1][white[1]] == 0:
                    upScore = judge(white[0], white[1], white[0] - 1, white[1], blackChess, chess)
                    if upScore > score:
                        score = upScore
                        X0 = white[0]
                        Y0 = white[1]
                        X1 = white[0] - 1
                        Y1 = white[1]
                    elif upScore == score and random.random() >= 0.5:
                        score = upScore
                        X0 = white[0]
                        Y0 = white[1]
                        X1 = white[0] - 1
                        Y1 = white[1]

            if white[0] < 4:
                if chess[white[0] + 1][white[1]] == 0:
                    upScore = judge(white[0], white[1], white[0] + 1, white[1], blackChess, chess)
                    if upScore > score:
                        score = upScore
                        X0 = white[0]
                        Y0 = white[1]
                        X1 = white[0] + 1
                        Y1 = white[1]
                    elif upScore == score and random.random() >= 0.5:
                        score = upScore
                        X0 = white[0]
                        Y0 = white[1]
                        X1 = white[0] + 1
                        Y1 = white[1]

        sendMove(X0, Y0, X1, Y1)
        print("(" + str(X0) + ',' + str(Y0) + ') --> (' + str(X1) + ',' + str(Y1) + ')')
        whiteChess.clear()
        blackChess.clear()
        score = -65535

