import random
import numpy as np


def load_data_set(filename):
    data_list = []
    label_list = []
    L=[]
    fr = open(filename)
    for line in fr.readlines():
        line_array = line.strip().split('\t')
        label_list.append(float(line_array[len(line_array)-1])*2-3)
        for i in range(len(line_array)-1):
            L.append(float(line_array[i]))
        data_list.append(L)
        L=[]
    return data_list, label_list


def select_jrand(i, m):
    j = i
    while (j==i):
        j = int(random.uniform(0, m))
    return j


def clipAlpha(aj, H, L):
    if aj > H:
        aj = H
    if L > aj:
        aj = L
    return aj



class optStruct:
    def __init__(self, dataMatIn, label_list, C, toler):
        self.X = dataMatIn
        self.labelMat = label_list
        self.C = C
        self.tol = toler
        self.m = dataMatIn.shape[0]
        self.alphas = np.mat(np.zeros((self.m, 1)))
        self.b = 0
        self.eCache = np.mat(np.zeros((self.m, 2)))



def calc_Ek(struct, k):
    fXk = float(np.multiply(struct.alphas,struct.labelMat).T*(struct.X*struct.X[k,:].T) + struct.b)
    Ek = fXk - float(struct.labelMat[k])
    return Ek



def select_j(i, struct, Ei):
    maxK = -1;  maxDeltaE = 0;  Ej = 0
    struct.eCache[i] = [1, Ei]
    validEcacheList = np.nonzero(struct.eCache[:,0].A)[0]
    if len(validEcacheList) > 1:
        for k in validEcacheList:
            if k == i: continue
            Ek = calc_Ek(struct, k)
            deltaE = abs(Ei - Ek)
            if deltaE > maxDeltaE:
                maxK = k;  maxDeltaE = deltaE;  Ej = Ek
        return maxK, Ej
    else:
        j = select_jrand(i, struct.m)
        Ej = calc_Ek(struct, j)
    return j, Ej


def update_Ek(struct, k):
    Ek = calc_Ek(struct, k)
    struct.eCache[k] = [1, Ek]


def inner_loop(i, struct):
    Ei = calc_Ek(struct, i)
    if ((struct.labelMat[i]*Ei < -struct.tol) and (struct.alphas[i] < struct.C)) or \
        ((struct.labelMat[i]*Ei > struct.tol) and (struct.alphas[i] > 0)):
        j, Ej = select_j(i, struct, Ei)
        alpha_i_old = struct.alphas[i].copy();  alpha_j_old = struct.alphas[j].copy()
        if struct.labelMat[i] != struct.labelMat[j]:
            L = max(0, struct.labelMat[j] - struct.labelMat[i])
            H = min(struct.C, struct.C + struct.labelMat[j] - struct.labelMat[i])
        else:
            L = max(struct.labelMat[j] + struct.labelMat[i] - struct.C, 0)
            H = min(struct.C, struct.labelMat[j] + struct.labelMat[i])
        if L == H:
            print('L == H')
            return 0
        eta = 2.0 * struct.X[i,:]*struct.X[j,:].T - struct.X[i,:]*struct.X[i,:].T - struct.X[j,:]*struct.X[j,:].T
        if eta >= 0:
            print("eta>=0")
            return  0
        struct.alphas[j] -= struct.labelMat[j]*(Ei - Ej)/eta
        struct.alphas[j] = clipAlpha(struct.alphas[j], H, L)
        update_Ek(struct, j)
        if abs(struct.alphas[j]-alpha_j_old) < 0.00001:
            #print("j not moving enough")
            return 0
        struct.alphas[i] += struct.labelMat[j]*struct.labelMat[i]*(alpha_j_old - struct.alphas[j])
        update_Ek(struct, i)
        b1 = struct.b - Ei - struct.labelMat[i] * (struct.alphas[i] - alpha_i_old) * struct.X[i,:]*struct.X[i,:].T - struct.labelMat[j] * (struct.alphas[j] - alpha_j_old) * struct.X[i,:]*struct.X[j,:].T
        b2 = struct.b - Ej - struct.labelMat[i] * (struct.alphas[i] - alpha_i_old) * struct.X[i,:]*struct.X[j,:].T - struct.labelMat[j] * (struct.alphas[j] - alpha_j_old) * struct.X[j,:]*struct.X[j,:].T
        if (0 < struct.alphas[i]) and (struct.C > struct.alphas[i]):
            struct.b = b1
        elif (0 < struct.alphas[j]) and (struct.C > struct.alphas[j]):
            struct.b = b2
        else:
            struct.b = (b1 + b2)/2.0
        return 1
    else:
        return 0


def smo(data_list, label_list, C, toler, max_iter):
    struct = optStruct(np.mat(data_list), np.mat(label_list).transpose(), C, toler)
    iter = 0
    entireSet = True; alphaPairsChanged = 0
    while (iter < max_iter) and ((alphaPairsChanged > 0) or (entireSet)):
        alphaPairsChanged = 0
        if entireSet:
            for i in range(struct.m):
                alphaPairsChanged += inner_loop(i,struct)
                #print("fullSet, iter: %d i:%d, pairs changed %d" % (iter,i,alphaPairsChanged))
            iter += 1
        else:
            nonBoundIs = np.nonzero((struct.alphas.A > 0) * (struct.alphas.A < C))[0]
            for i in nonBoundIs:
                alphaPairsChanged += inner_loop(i,struct)
                #print("non-bound, iter: %d i:%d, pairs changed %d" % (iter,i,alphaPairsChanged))
            iter += 1
        if entireSet: entireSet = False
        elif (alphaPairsChanged == 0): entireSet = True
        print("iteration number: %d" % iter)
    return struct.b,struct.alphas


def calcWs(alphas,data_list,label_list):
    X = np.mat(data_list); labelMat = np.mat(label_list).transpose()
    m,n = X.shape
    w = np.zeros((n,1))
    for i in range(m):
        w += np.multiply(alphas[i]*labelMat[i],X[i,:].T)
    return w

def predict(data_list, label_list, ws, b):
    right = 0
    weight = np.mat(ws)
    dataMat = np.mat(data_list)
    m = dataMat.shape[0]
    for i in range(m):
        y = dataMat[i,:]*weight + b
        if (y*label_list[i] > 0):
            right += 1
    print float(right)/m


def main():
    data_list, label_list =load_data_set('C:\\Users\\11033\\Desktop\\test5\\traindata.txt')
    data_list_test, label_list_test = load_data_set('C:\\Users\\11033\\Desktop\\test5\\testdata.txt')
    b, alpha = smo(data_list, label_list, 10, 0.001, 50)
    ws = calcWs(alpha, data_list, label_list)
    predict(data_list, label_list, ws, b)
    predict(data_list_test, label_list_test, ws, b)


if __name__ == '__main__':
    main()
